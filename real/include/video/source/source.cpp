#include <video/source/source.hpp>

Source::Source() {
    pipeline = std::make_shared<GstElement>(gst_pipeline_new("pipeline"));
    sourceElements.source = gst_element_factory_make("videotestsrc", "src");
    sourceElements.converter = gst_element_factory_make("videoconvert", "converter");
    sourceElements.encoder = gst_element_factory_make("x264enc", "encoder");
    sink = std::make_shared<GstElement>(gst_element_factory_make("appsink", "sink"));
}

Source::~Source() {
    
}