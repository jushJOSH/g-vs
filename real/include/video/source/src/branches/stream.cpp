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

StreamBranch::StreamBranch()
:   PipeBranch(
        "appsink",
        "multipartmux"
    )
{
    g_print("Created stream branch %s\n", uuid.c_str());

    if (!loadBin())
        throw std::runtime_error("Could not link elements for some reason...");

    g_object_set(sink, "emit-signals", true, NULL);
    g_signal_connect(sink, "new-sample", G_CALLBACK(sampleAquired), &arg);
}

StreamBranch::StreamBranch(std::condition_variable* untilBranchReady, std::mutex *commonBranchMutex) 
:   StreamBranch()
{
    arg.untilBranchReady = untilBranchReady;
    arg.commonBranchMutex = commonBranchMutex;
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
    auto newPad = gst_element_request_pad_simple(muxer, "sink_%u");

    auto padName = std::string(gst_pad_get_name(newPad));
    auto ghostName = str(format("%1%_ghost") % padName);

    auto ghostPad = gst_ghost_pad_new(
            ghostName.c_str(),
            newPad
    );
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(newPad);

    return ghostPad;
}

std::shared_ptr<Sample> StreamBranch::getSample() {
    return std::make_shared<Sample>(gst_app_sink_pull_sample(GST_APP_SINK(this->sink)));
}

GstElement *StreamBranch::getFirstElement() const {
    return this->muxer;
}

StreamBranch::~StreamBranch() {
    g_print("StreamBranch: destroyed one\n");

    unloadBin();
}

GstFlowReturn StreamBranch::sampleAquired(GstElement* appsink, BranchReadyArg *arg) {
    auto sample = Sample(gst_app_sink_pull_sample(GST_APP_SINK(appsink)));
    if (!sample.isNull()) {
        g_print("StreamBranch: lets think branch is ready to transmit funny\n");
        if (arg->commonBranchMutex != nullptr && arg->untilBranchReady != nullptr) {
            std::lock_guard<std::mutex> lock(*arg->commonBranchMutex);
            arg->ready = true;
            arg->untilBranchReady->notify_all();
        }
        else arg->ready = true;
        g_object_set(appsink, "emit-signals", false, NULL);
    }

    return GST_FLOW_OK;
}

bool StreamBranch::isReady() const {
    return arg.ready;
}