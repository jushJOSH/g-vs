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

void PipeTree::onNewRtspPad(GstElement *src, GstPad *newPad, GstElement* tee) {
    GstPad *sink_pad = gst_element_get_static_pad(tee, "sink");
    
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

    /* If our converter is already linked, we have nothing to do here */
	if (gst_pad_is_linked(sink_pad)) {
		g_print("Sink pad from %s already linked. Ignoring.\n", GST_ELEMENT_NAME(src));
		goto exit;
	}

	g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(newPad), GST_ELEMENT_NAME(src));

	/* Check the new pad's name */
	if (!g_str_has_prefix(GST_PAD_NAME(newPad), "recv_rtp_src_")) {
		g_print("It is not the right pad. Need recv_rtp_src_. Ignoring.\n");
		goto exit;
	}

	/* Check the new pad's type */
	new_pad_caps = gst_pad_query_caps(newPad, NULL);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);

	/* Attempt the link */
	if (!gst_pad_link(newPad, sink_pad)) {
		g_print("Type is '%s' but link failed.\n", new_pad_type);
	}
	else {
		g_print("Link succeeded (type '%s').\n", new_pad_type);
	}

exit:
	/* Unreference the new pad's caps, if we got them */
	if (new_pad_caps != NULL)
		gst_caps_unref(new_pad_caps);

	/* Unreference the sink pad */
	gst_object_unref(sink_pad);
}

PipeTree::PipeTree() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    pipeline(gst_pipeline_new(uuid.c_str())),
    tee(gst_element_factory_make("tee", str(format("%1%_tee") % uuid).c_str()))
{
    g_print("Created tree %s\n", uuid.c_str());
    
    // Add to pipeline
    gst_bin_add_many(GST_BIN(pipeline), tee, NULL);
}

PipeTree::PipeTree(const std::string& source)
:   PipeTree()
{
    setSource(source);
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

void PipeTree::setSource(const std::string &source) {
    std::regex protocolRegex("(\\w+):/");
    std::smatch match;
    if (!std::regex_search(source, match, protocolRegex)) return;
    
    std::string protocol = match.str(1);
    std::string sourceType = str(boost::format("%1%src") % protocol);

    this->source = gst_element_factory_make(sourceType.c_str(), str(format("%1%_source") % uuid).c_str());
    gst_bin_add_many(GST_BIN(pipeline), this->source, this->tee, NULL);
    g_object_set(this->source, "location", source.c_str(), NULL);

    g_signal_connect(this->source, "pad-added", G_CALLBACK(onNewRtspPad), tee);
}

GstStateChangeReturn PipeTree::setState(GstState state) {
    return gst_element_set_state(pipeline, state);
}