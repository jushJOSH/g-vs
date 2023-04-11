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
}

ArchiveBranch::ArchiveBranch(GstBin* bin, const std::string &path)
:   ArchiveBranch(path)
{
    if (!loadBin(bin))
        throw std::runtime_error("Could not link elements for some reason...");
}

bool ArchiveBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(this->bin, muxer, sink, NULL);
    return gst_element_link(muxer, sink);
}

void ArchiveBranch::unloadBin() {
    gst_bin_remove_many(bin, muxer, sink, NULL);
    bin = nullptr;
}

GstPad* ArchiveBranch::getNewPad(DataLine::LineType type) {
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

std::string ArchiveBranch::getPath() const {
    return this->path;
}