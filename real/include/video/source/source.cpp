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
:   uuid(std::make_shared<std::string>(boost::uuids::to_string(boost::uuids::random_generator_mt19937()()))),
    arg(std::make_shared<CallbackArg>())
{   
    g_print("Source created %s\n", (*uuid).c_str());

    arg->samples = std::make_shared<boost::circular_buffer<std::shared_ptr<Sample>>>(10);

    sourceElements.sink = gst_element_factory_make("appsink", "sink");
    sourceElements.muxer = gst_element_factory_make("multipartmux", "muxer");
    sourceElements.source = gst_element_factory_make("videotestsrc", "src");
    sourceElements.encoder = gst_element_factory_make("x264enc", "encoder");
    sourceElements.pipeline = gst_pipeline_new("pipeline");
    sourceElements.converter = gst_element_factory_make("videoconvert", "converter");

    gst_bin_add_many(GST_BIN(sourceElements.pipeline), sourceElements.source, sourceElements.encoder, sourceElements.converter, sourceElements.muxer, sourceElements.sink, NULL);
    g_object_set(G_OBJECT(sourceElements.sink), "emit-signals", TRUE, NULL);
    g_signal_connect(sourceElements.sink, "new-sample", G_CALLBACK(on_new_sample), (gpointer*)arg.get());

    /* Установка связей между элементами */
    auto isLinkedOk = gst_element_link_many(sourceElements.source, sourceElements.converter, sourceElements.encoder, sourceElements.muxer, sourceElements.sink, NULL);
    if (!isLinkedOk)
    {
        g_print("Some shit lets go again");
    }
}

GstStateChangeReturn Source::setState(GstState state) {
    return gst_element_set_state(sourceElements.pipeline, state);
}

Source::~Source() {
    g_print("Source deleted %s\n", (*uuid).c_str());

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