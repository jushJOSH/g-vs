#include <pipeline/pipeline.hpp>
#include <brick/brick.hpp>

GPipeline::GPipeline(const std::string& pipeName) 
: self(gst_pipeline_new(pipeName.c_str())), pipelineBus(*this)
{}

GPipeline::GPipeline(GstElement* rawPipe) 
: self(rawPipe), pipelineBus(*this)
{}

GPipeline::~GPipeline() {
    // Free resources
    g_print("Pipe %s deleted\n", GST_ELEMENT_NAME(self));
    gst_element_set_state (self, GST_STATE_NULL);
    g_object_unref(self);
}

GstStateChangeReturn GPipeline::updateState(GstState state) {
    return gst_element_set_state(self, state);
}

bool GPipeline::addToPipeline(GstElement* elem) {
    return gst_bin_add(GST_BIN(self), elem);
}

GstElement* GPipeline::addToPipelineMany(const std::initializer_list<GBrick>& list) {
    for (GstElement* elem : list) 
        if (!addToPipeline(elem))
            return elem;

    return nullptr;
}

GstElement* GPipeline::addToPipelineMany(const std::initializer_list<GstElement*>& list) {
    for (GstElement* elem : list) 
        if (!addToPipeline(elem))
            return elem;

    return nullptr;
}

bool GPipeline::removeFromPipeline(GstElement* elem) {
    return gst_bin_remove(GST_BIN(self), elem);
}

GstElement* GPipeline::removeFromPipelineMany(const std::initializer_list<GstElement*>& list) {
    for (auto elem : list)
        if (!removeFromPipeline(elem))
            return elem;

    return nullptr;
}

GBus &GPipeline::getBus() {
    return pipelineBus;
}

void GPipeline::setPipelineClock(GstClock* clock) {
    gst_pipeline_use_clock(GST_PIPELINE(self), clock);
}

void GPipeline::restorePipelineClock() {
    gst_pipeline_auto_clock(GST_PIPELINE(self));
}

GBus GPipeline::reloadBus() {
    gst_object_unref((GstBus*)pipelineBus);
    this->pipelineBus = GBus(*this);
    return this->pipelineBus;
}