#include <video/source/branches/archive.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

ArchiveBranch::ArchiveBranch(const std::string &path)
:   PipeBranch(
        "filesink",
        "mp4mux"
    )
{
    g_print("Created archive branch %s\n", uuid.c_str());
    g_object_set(sink, "location", path.c_str(), NULL);

    if (!loadBin())
        throw std::runtime_error("Could not link elements for some reason...");

}

bool ArchiveBranch::loadBin() {
    gst_bin_add_many(this->bin, muxer, sink, NULL);
    return gst_element_link(muxer, sink);
}

void ArchiveBranch::unloadBin() {
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(bin, muxer, sink, NULL);
}

GstPad* ArchiveBranch::getSinkPad(DataLine::LineType type) {
    GstPad* newPad = nullptr;
    switch(type) {
        case DataLine::LineType::Audio:
        g_print("StreamBranch: Getting new audio pad\n");
        newPad = gst_element_request_pad_simple(muxer, "audio_%u");

        case DataLine::LineType::Video:
        g_print("StreamBranch: Getting new video pad\n");
        newPad = gst_element_request_pad_simple(muxer, "video_%u");

        default:
        return nullptr;
    }

    auto padName = gst_pad_get_name(newPad);
    auto ghostPadName = str(format("%1%_%2%_ghost") % uuid % padName).c_str();
    auto ghostPad = gst_ghost_pad_new(
            ghostPadName,
            newPad
    );
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(newPad);

    return gst_element_get_static_pad(GST_ELEMENT(bin), ghostPadName);
}

std::string ArchiveBranch::getPath() const {
    return this->path;
}

GstElement *ArchiveBranch::getFirstElement() const {
    return this->muxer;
}

ArchiveBranch::~ArchiveBranch() {
    g_print("ArchiveBranch: destroyed one\n");

    unloadBin();
}