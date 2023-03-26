#include <video/source/source.hpp>

#include <tuple>

std::unordered_map<std::string, std::tuple<bool, Sample>> Source::sampleCloset = {};

GstFlowReturn Source::on_new_sample(GstElement* appsink, gpointer data)  {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
    // GstBuffer *buffer = gst_sample_get_buffer(sample);

    g_print("New frame arrived!\n");

    // Setting new sample to selected uuid
    auto uuid = (boost::uuids::uuid*)data;
    auto s_uuid = boost::uuids::to_string(*uuid);
    Source::sampleCloset[s_uuid] = {true, sample};

    return GST_FLOW_OK;
}

boost::uuids::uuid Source::getUUID() const {
    return *uuid;
}

Source::Source() 
: uuid(std::make_shared<boost::uuids::uuid>(boost::uuids::random_generator_mt19937()()))
{
    g_print("Source created %s\n", boost::uuids::to_string(*uuid).c_str());

    sourceElements.sink = gst_element_factory_make("appsink", "sink");
    sourceElements.source = gst_element_factory_make("videotestsrc", "src");
    sourceElements.encoder = gst_element_factory_make("x264enc", "encoder");
    sourceElements.pipeline = gst_pipeline_new("pipeline");
    sourceElements.converter = gst_element_factory_make("videoconvert", "converter");

    gst_bin_add_many(GST_BIN(sourceElements.pipeline), sourceElements.source, sourceElements.encoder, sourceElements.converter, sourceElements.sink, NULL);
    g_object_set(G_OBJECT(sourceElements.sink), "emit-signals", TRUE, NULL);
    g_signal_connect(sourceElements.sink, "new-sample", G_CALLBACK(on_new_sample), (gpointer*)uuid.get());

    /* Установка связей между элементами */
    auto isLinkedOk = gst_element_link_many(sourceElements.source, sourceElements.converter, sourceElements.encoder, sourceElements.sink, NULL);
    if (!isLinkedOk)
    {
        g_print("Some shit lets go again");
    }
}

GstStateChangeReturn Source::setState(GstState state) {
    return gst_element_set_state(sourceElements.pipeline, state);
}

Source::~Source() {
    /* Остановка пайплайна и освобождение ресурсов */
    gst_element_set_state(sourceElements.pipeline, GST_STATE_NULL);
    gst_object_unref(sourceElements.pipeline);

    g_print("Source destroyed %s\n", boost::uuids::to_string(*uuid).c_str());
}

std::tuple<bool, Sample> Source::getSample() const {
    return Source::sampleCloset.at(boost::uuids::to_string(*uuid));
}