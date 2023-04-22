
#include <video/source/pipetree.hpp>

#include <video/source/datalines/audio.hpp>
#include <video/source/datalines/video.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::str;

GstPadProbeReturn PipeTree::branchEosProbe(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
    g_print("Received %s event\n", gst_event_type_get_name(GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info))));
    if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS)
        return GST_PAD_PROBE_OK;
    gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));

    g_print("Entered eos probe call\n");
    RemoveBranch *branchdata = (RemoveBranch*)user_data;
    g_print("Target: %s\n", branchdata->branch->getUUID().c_str());

    auto targetFilter = branchdata->branch->getFilters()[branchdata->currIdx];
    branchdata->branch->removeFilter(targetFilter->getUUID());
    
    if (!branchdata->branch->getFilters().size()) {
        g_print("Remove branch itself\n");
        branchdata->branches->erase(branchdata->branch->getUUID());
        if (branchdata->cbdata != nullptr)
            branchdata->cbdata->cb(branchdata->cbdata->data);
    }

    delete branchdata;

    return GST_PAD_PROBE_DROP;
}

GstPadProbeReturn PipeTree::branchUnlinkProbe(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
    gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
    g_print("Entered pad probe call\n");
    RemoveBranch *branchdata = (RemoveBranch*)user_data;
    g_print("Target: %s\n", branchdata->branch->getUUID().c_str());

    // Setting up new probe for EOS event
    auto filter = branchdata->branch->getFilters()[branchdata->currIdx];
    auto src = gst_element_get_static_pad (filter->getEncoder(), "src");
    gst_pad_add_probe (src, 
        (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
        branchEosProbe, user_data, NULL);
    gst_object_unref (src);
    filter->detachFromPipeline();
    gst_element_send_event(filter->getEncoder(), gst_event_new_eos());

    return GST_PAD_PROBE_OK;
}

int PipeTree::manageBranchQueue(PadInfo& data) {
    g_print("PipeTree: Started queue management\n");

    int successfulCount = 0;
    while (!data.branchQueue.empty()) {
        auto branch = data.branchQueue.front();
        data.branchQueue.pop();

        if (!branch->attachToPipeline(data.createdPads, data.bin))
            continue;

        successfulCount++;
        data.branches[branch->getUUID()] = branch;
    }

    return successfulCount;
}

void PipeTree::onNoMorePads(GstElement* src, PadInfo* data) {
    data->noMorePads = true;
    manageBranchQueue(*data);
}

void PipeTree::onNewPad(GstElement* element, GstPad* newPad, PadInfo* userData) {
    g_print ("PipeBranch: Received new pad '%s'\n", GST_PAD_NAME (newPad));

    auto UUID = boost::uuids::to_string(boost::uuids::random_generator_mt19937()());
    auto tee = gst_element_factory_make("tee", str(format("%1%_tee") % UUID).c_str());

    auto teeSinkPad = gst_element_get_static_pad(tee, "sink");
    gst_bin_add(userData->bin, tee);
    gst_element_sync_state_with_parent(tee);
    auto linkResult = gst_pad_link(newPad, teeSinkPad);
    
    userData->dynamicElements.push_back(tee);

    // Check new pad type
    GstCaps *pad_caps = gst_pad_get_current_caps (newPad);
    GstStructure *pad_struct = gst_caps_get_structure (pad_caps, 0);
    const gchar *pad_type = gst_structure_get_name (pad_struct);

    if (g_str_has_prefix (pad_type, "audio/x-raw"))
        userData->createdPads.push_back({"audio/x-raw", tee});

    else if (g_str_has_prefix(pad_type, "video/x-raw")) 
        userData->createdPads.push_back({"video/x-raw", tee});
    
    if (pad_caps != NULL)
        gst_caps_unref (pad_caps);
}

PipeTree::PipeTree() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    pipeline(gst_pipeline_new(uuid.c_str()))
{
    g_print("Created tree %s\n", uuid.c_str());
    
    // Setting bin for padinfo data
    padinfo.bin = GST_BIN(pipeline);
}

PipeTree::PipeTree(const std::string& source)
:   PipeTree()
{
    setSource(source);
}

PipeTree::~PipeTree() {
    g_print("Deleted tree %s\n", uuid.c_str());

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_bin_remove(GST_BIN(pipeline), source);
    for (auto elem : padinfo.dynamicElements) 
        gst_bin_remove(GST_BIN(pipeline), elem);

    gst_object_unref(pipeline);
}

// All branches must be in queue
// Queue will be solved as soon as noMorePads signal appears
void PipeTree::addBranch(std::shared_ptr<PipeBranch> branch) {
    padinfo.branchQueue.push(branch);

    // If add branch after pad init
    // then add them asap
    if (padinfo.noMorePads)
        manageBranchQueue(padinfo);
}

GstBus* PipeTree::getBus() const {
    return gst_element_get_bus(pipeline);
}

void PipeTree::removeBranch(const std::string& name) {
    if (!padinfo.branches.contains(name)) return;
    
    auto filters = padinfo.branches.at(name)->getFilters();
    for (int i = 0; i < filters.size(); ++i) {
        RemoveBranch *arg = new RemoveBranch{ i, padinfo.branches.at(name), &padinfo.branches, &cbdata };
        gst_pad_add_probe (filters[i]->getPreviousPad(), GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM,
                           branchUnlinkProbe, arg, NULL);
    }
}

GstElement* PipeTree::getSink(const std::string &name) {
    return padinfo.branches.contains(name) 
        ? padinfo.branches[name]->getLastElement() 
        : nullptr;
}

void PipeTree::setSource(const std::string &source) {
    this->source = gst_element_factory_make("uridecodebin", str(format("%1%_uridecodebin") % uuid).c_str());
    gst_bin_add(GST_BIN(pipeline), this->source);
    g_object_set(this->source, "uri", source.c_str(), NULL);
    
    g_signal_connect(this->source, "pad-added", G_CALLBACK(onNewPad), &padinfo);
    g_signal_connect(this->source, "no-more-pads", G_CALLBACK(onNoMorePads), &padinfo);
}

GstStateChangeReturn PipeTree::setState(GstState state) {
    auto stateResult = gst_element_set_state(pipeline, state);
    return stateResult;
}

void PipeTree::setConfig(SourceConfigDto& config) {
    padinfo.config = config;
}

bool PipeTree::noMoreBranches() const {
    return padinfo.branches.empty();
}

void PipeTree::setOnBranchDeleted(const std::function<void(void*)> callback, void* data) {
    this->cbdata.cb = callback;
    this->cbdata.data = data;
}

GstElement* PipeTree::getPipeline() const {
    return pipeline;
}