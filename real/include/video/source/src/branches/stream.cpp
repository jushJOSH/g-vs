#include <video/source/branches/stream.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <filesystem>

using boost::format;
using boost::str;

StreamBranch::StreamBranch(const std::string &playlistFolder, const std::string &playlistId)
:   PipeBranch(
        "hlssink",
        "mpegtsmux"
    )
{
    g_print("Created stream branch %s\n", uuid.c_str());
    
    if (!loadBin())
        throw std::runtime_error("Could not link elements for some reason...");
    
    //g_object_set(sink, "playlist-root", playlistRootFolder.c_str(), NULL);
    auto playlist_root = str(format("static/%s") % playlistId);
    auto segment_loc = str(format("%s/%s/segment\%\%05d.ts") % playlistFolder % playlistId);
    auto playlist_loc = str(format("%s/%s/playlist.m3u8") % playlistFolder % playlistId);

    g_object_set(sink, "playlist-root", playlist_root.c_str(), NULL);
    g_object_set(sink, "playlist-location", playlist_loc.c_str(), NULL);
    g_object_set(sink, "location", segment_loc.c_str(), NULL);
    
    this->createdFolder = str(format("%s/%s") % playlistFolder % playlistId);
    bool folderCreated = std::filesystem::create_directories(this->createdFolder);
    if (!folderCreated)
        throw std::runtime_error(str(format("%s folder cannot be created!") % this->createdFolder));
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
    auto newPad = gst_element_request_pad_simple(muxer, "sink_%d");

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

GstElement *StreamBranch::getFirstElement() const {
    return this->muxer;
}

StreamBranch::~StreamBranch() {
    g_print("StreamBranch: destroyed one\n");

    unloadBin();

    std::filesystem::remove_all(createdFolder);
}