#include <video/source/branches/screenshot.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

ScreenshotBranch::ScreenshotBranch(const std::string &path)
:   PipeBranch(
        "filesink"
    )
{
    g_print("Created screenshot branch %s\n", uuid.c_str());
    g_object_set(sink, "location", path.c_str(), NULL);
}

ScreenshotBranch::ScreenshotBranch(GstBin* bin, const std::string &path)
:   ScreenshotBranch(path)
{
    loadBin(bin);
}

bool ScreenshotBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(this->bin, sink, NULL);
    
    return true;
}

void ScreenshotBranch::unloadBin() {
    gst_bin_remove_many(bin, sink, NULL);
    bin = nullptr;
}

GstPad* ScreenshotBranch::getNewPad(DataLine::LineType type) {
    switch(type) {
        case DataLine::LineType::Video:
        g_print("ScreenshotBranch: Getting new video pad\n");
        return gst_element_get_static_pad(sink, "sink");

        default:
        return nullptr;
    }
}

std::string ScreenshotBranch::getPath() const {
    return this->path;
}
