#include <video/source/branches/screenshot.hpp>
#include <video/source/datalines/videoline.hpp>
#include <video/source/datalines/audioline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <png++/png.hpp>

using boost::format;
using boost::str;

ScreenshotBranch::ScreenshotBranch(const std::string &path)
:   PipeBranch(
        "appsink",
        "mp4mux"
    )
{
    g_print("Created screenshot branch %s\n", uuid.c_str());
    g_signal_connect(sink, "new-sample", G_CALLBACK(onNewSample), &uuid);
}

bool ScreenshotBranch::loadBin(GstBin *bin) {
    if (!this->bin) 
        this->bin = bin;

    gst_bin_add_many(bin, sink, NULL);
    
    return 
        true;
}

void ScreenshotBranch::unloadBin() {
    gst_bin_remove_many(bin, sink, NULL);
    bin = nullptr;
}

GstPad* ScreenshotBranch::getNewPad(DataLine::LineType type) {
    switch (type) {
        case DataLine::LineType::Video:
            g_print("ScreenshotBranch: Getting new pad\n");
        return gst_element_get_static_pad(this->sink, "sink");

        default:
        return nullptr;
    }
}

void ScreenshotBranch::onNewSample(GstElement* src, gpointer arg) {
    
}