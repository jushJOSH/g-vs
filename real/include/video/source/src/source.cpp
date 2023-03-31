#include <video/source/source.hpp>

#include <atomic>

GstFlowReturn Source::on_new_sample(GstElement* appsink, gpointer data)  {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

    // Setting new sample to selected uuid
    auto arg = (Source::CallbackArg*)data;

    std::lock_guard<std::mutex> locker(arg->mutex);
    arg->samples->push_back(std::make_shared<Sample>(sample));

    return GST_FLOW_OK;
}

Source::Source() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    arg(std::make_shared<CallbackArg>())
{   
    g_print("Test source created %s\n", uuid.c_str());

    arg->samples = std::make_shared<boost::circular_buffer<std::shared_ptr<Sample>>>(10);

    sourceElements.tee = gst_element_factory_make("tee", "splitter");
    sourceElements.source = gst_element_factory_make("videotestsrc", "src");
    sourceElements.decoder = gst_element_factory_make("decodebin", "decoder");
    sourceElements.pipeline = gst_pipeline_new("pipeline");
    sourceElements.converter = gst_element_factory_make("videoconvert", "converter");

    /* Установка связей между элементами */
    auto isLinkedOk = gst_element_link_many(sourceElements.source, sourceElements.decoder, sourceElements.converter, sourceElements.tee, NULL);
    if (!isLinkedOk)
    {
        g_print("Some shit lets go again");
    }
}

GstStateChangeReturn Source::setState(GstState state) {
    return gst_element_set_state(sourceElements.pipeline, state);
}

Source::~Source() {
    g_print("Source deleted %s\n", uuid.c_str());

    /* Остановка пайплайна и освобождение ресурсов */
    gst_element_set_state(sourceElements.pipeline, GST_STATE_NULL);
    gst_object_unref(sourceElements.pipeline);
}

std::shared_ptr<Sample> Source::getSample() {
    std::lock_guard<std::mutex> locker(arg->mutex);

    auto copy = arg->samples->front();
    arg->samples->pop_front();

    return copy;
}

void Source::waitSample() const {
    while (!arg->samples->size());
}

bool Source::addBranch(const std::string &name, PipeBranch& branch) {
    bool isLinked = gst_element_link(sourceElements.tee, branch.queue);
    if (isLinked) sourceElements.branches[name] = std::make_shared<PipeBranch>(branch);

    return isLinked;
}

bool Source::addTestBranch() {
    PipeBranch branch;
    branch.queue = 
}