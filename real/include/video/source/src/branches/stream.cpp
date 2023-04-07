#include <video/source/branches/stream.hpp>
#include <video/source/datalines/videoline.hpp>
#include <video/source/datalines/audioline.hpp>

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
}

bool StreamBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(bin, muxer, sink, NULL);
    
    return gst_element_link(muxer, sink);
}

void StreamBranch::unloadBin() {
    gst_bin_remove_many(bin, muxer, sink, NULL);
    bin = nullptr;
}

GstPad* StreamBranch::getNewPad(DataLine::LineType type) {
    switch(type) {
        case DataLine::LineType::Audio:
        g_print("StreamBranch: Getting new audio pad\n");
        return gst_element_get_request_pad(muxer, "audio_%u");

        case DataLine::LineType::Video:
        g_print("StreamBranch: Getting new video pad\n");
        return gst_element_get_request_pad(muxer, "video_%u");

        default:
        return nullptr;
    }
}

void StreamBranch::setCallback(GCallback callback, gpointer callbackArg) {
    g_signal_connect(sink, "new-sample", callback, callbackArg);
}