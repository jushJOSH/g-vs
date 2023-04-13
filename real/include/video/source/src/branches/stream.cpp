#include <video/source/branches/stream.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>
#include <video/sample/sample.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

// TODO make queue lenght in config
StreamBranch::StreamBranch()
:   PipeBranch(
        "appsink",
        "multipartmux"
    ),
    arg(std::make_shared<CallbackArg>())
{
    g_print("Created stream branch %s\n", uuid.c_str());
    arg->samples = std::make_shared<boost::circular_buffer<std::shared_ptr<Sample>>>(10);

    if (!loadBin())
        throw std::runtime_error("Could not link elements for some reason...");
}

bool StreamBranch::loadBin() {
    gst_bin_add_many(bin, muxer, sink, NULL);
    return gst_element_link(muxer, sink);
}

void StreamBranch::unloadBin() {
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(bin, muxer, sink, NULL);
}

GstPad* StreamBranch::getSinkPad(DataLine::LineType type) {
    auto newPad = gst_element_get_request_pad(muxer, "sink_%u");

    auto padName = std::string(gst_pad_get_name(newPad));
    auto ghostName = str(format("%1%_%2%_ghost") % uuid % padName);

    auto ghostPad = gst_ghost_pad_new(
            ghostName.c_str(),
            newPad
    );
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(newPad);

    return ghostPad;
}

void StreamBranch::setCallback(GCallback callback) {
    g_print("StreamBranch: Setting callback\n");

    g_object_set(G_OBJECT(sink), "emit-signals", TRUE, NULL);
    g_signal_connect(sink, "new-sample", callback, arg.get());
}

std::shared_ptr<Sample> StreamBranch::getSample() {
    std::lock_guard<std::mutex> locker(arg->mutex);
    
    auto copy = arg->samples->front();
    arg->samples->pop_front();

    return copy;
}

void StreamBranch::waitSample() const {
    while (!arg->samples->size());
}

GstFlowReturn StreamBranch::onNewSample(GstElement* appsink, CallbackArg *data)  {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

    std::lock_guard<std::mutex> locker(data->mutex);
    data->samples->push_back(std::make_shared<Sample>(sample));

    return GST_FLOW_OK;
}

GstElement *StreamBranch::getFirstElement() const {
    return this->muxer;
}

StreamBranch::~StreamBranch() {
    g_print("StreamBranch: destroyed one\n");

    gst_element_send_event(GST_ELEMENT(bin), gst_event_new_eos());
    unloadBin();
}