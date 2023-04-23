#include <video/source/branches/screenshot.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <oatpp/core/base/Environment.hpp>

using boost::format;
using boost::str;

ScreenshotBranch::ScreenshotBranch(const std::string &path)
:   PipeBranch(
        "filesink"
    )
{
    OATPP_LOGD("ScreenshotBranch", "Created screenshot branch %s", uuid.c_str());
    g_object_set(sink, "location", path.c_str(), NULL);
    
    if (!loadBin())
        throw std::runtime_error("Error linking elements in ScreenshotBranch");
}

bool ScreenshotBranch::loadBin() {
    gst_bin_add_many(bin, sink, NULL);

    return true;
}

void ScreenshotBranch::unloadBin() {
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(bin, sink, NULL);
}

GstPad* ScreenshotBranch::getSinkPad(DataLine::LineType type) {
    GstPad* newPad = nullptr;

    switch(type) {
        case DataLine::LineType::Video:
        OATPP_LOGI("ScreenshotBranch", "Getting new video pad");
        newPad = gst_element_get_static_pad(sink, "sink");

        default:
        return nullptr;
    }

    if (gst_pad_is_linked(newPad)) return nullptr;
    auto padName = gst_pad_get_name(newPad);
    auto ghostName = str(format("%1%_%2%_ghost") % uuid % padName).c_str();
    auto ghostPad = gst_ghost_pad_new(
        ghostName,
        newPad
    );
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(newPad);

    return gst_element_get_static_pad(GST_ELEMENT(bin), ghostName);
}

std::string ScreenshotBranch::getPath() const {
    return this->path;
}

GstElement *ScreenshotBranch::getFirstElement() const {
    return this->sink;
}

ScreenshotBranch::~ScreenshotBranch() {
    OATPP_LOGD("ScreenshotBranch", "destroyed one");

    unloadBin();
}