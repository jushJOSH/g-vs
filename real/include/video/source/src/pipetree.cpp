#include <video/source/pipetree.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <regex>

using boost::format;
using boost::str;

GstPadProbeReturn PipeTree::eventProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
    if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS)
        return GST_PAD_PROBE_OK;
    
    g_print("Entered eos probe call\n");

    PipeBranch* userBranch = (PipeBranch*)user_data;

    gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
    userBranch->unloadBin();

    return GST_PAD_PROBE_DROP;
}

GstPadProbeReturn PipeTree::padProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
    g_print("Entered pad probe call\n");
    
    PipeBranch* userBranch = (PipeBranch*)user_data;
    GstPad *src, *sink;

    // Removing probe
    gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
    
    // Setting up new probe for EOS event
    src = gst_element_get_static_pad (userBranch->getQueue(), "src");
    gst_pad_add_probe (src, 
        (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
        eventProbeCallback, user_data, NULL);
    gst_object_unref (src);

    sink = gst_element_get_static_pad (userBranch->getQueue(), "sink");
    gst_pad_send_event (sink, gst_event_new_eos ());
    gst_object_unref (sink);

    return GST_PAD_PROBE_OK;
}   

PipeTree::PipeTree() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    pipeline(gst_pipeline_new(uuid.c_str())),
    decoder(gst_element_factory_make("decodebin", str(format("%1%_decodebin") % uuid).c_str())),
    converter(gst_element_factory_make("videoconvert", str(format("%1%_videoconvert") % uuid).c_str())),
    tee(gst_element_factory_make("tee", str(format("%1%_tee") % uuid).c_str()))
{
    g_print("Created tree %s\n", uuid.c_str());
    
    // Add to pipeline
    gst_bin_add_many(GST_BIN(pipeline), decoder, converter, tee, NULL);
}

PipeTree::PipeTree(const std::string& source)
:   PipeTree()
{
    if (!setSource(source)) 
        throw std::runtime_error("Error creating PipeTree object: Link failed on setting source");
}

PipeTree::~PipeTree() {
    g_print("Deleted tree %s\n", uuid.c_str());

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

GstPadLinkReturn PipeTree::addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch) {
    branch->loadBin(GST_BIN(pipeline));
    
    // Creating new pad and then link it
    GstPad* newSrcPad = gst_element_request_pad_simple(tee, "src_%u");
    GstPad* branchSinkPad = gst_element_get_static_pad(branch->getQueue(), "sink");

    auto linkResult = gst_pad_link(newSrcPad, branchSinkPad);
    if (linkResult == GstPadLinkReturn::GST_PAD_LINK_OK) {
        branches[name] = branch;
        teePads[name] = newSrcPad;
    }

    gst_element_set_state(branch->getQueue(), GST_STATE_PLAYING);

    return linkResult;
}

void PipeTree::removeBranch(const std::string& name) {
    if (!branches.contains(name)) return;

    gst_pad_add_probe (teePads[name], 
        GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
        padProbeCallback, branches[name].get(), NULL);
}

GstElement* PipeTree::getSink(const std::string &name) {
    return branches.contains(name) 
        ? branches[name]->getSink() 
        : nullptr;
}

bool PipeTree::setSource(const std::string &source) {
    std::regex protocolRegex("(\\w+):/");
    std::smatch match;
    if (!std::regex_search(source, match, protocolRegex)) return false;
    
    std::string protocol = match.str(1);
    std::string sourceType = str(boost::format("%1%src") % protocol);
    
    setState(GST_STATE_NULL);

    this->source = gst_element_factory_make(sourceType.c_str(), str(format("%1%_source") % uuid).c_str());
    gst_bin_add(GST_BIN(pipeline), this->source);
    g_object_set(this->source, "location", source.c_str(), NULL);

    return 
        gst_element_link(this->source, decoder) &&
        gst_element_link(decoder, converter) &&
        gst_element_link(converter, tee);
}

GstStateChangeReturn PipeTree::setState(GstState state) {
    return gst_element_set_state(pipeline, state);
}