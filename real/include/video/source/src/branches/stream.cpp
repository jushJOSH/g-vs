#include <video/source/branches/stream.hpp>
#include <video/source/datalines/video.hpp>
#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

StreamBranch::StreamBranch(std::shared_ptr<HLSConfig> config, bool sync)
:   PipeBranch(
        "hlssink",
        "mpegtsmux"
    ),
    identity(gst_element_factory_make("identity", NULL))
{
    OATPP_LOGD("StreamBranch", "Created stream branch %s", uuid.c_str());
    
    if (!loadBin())
        throw std::runtime_error("Could not link elements for some reason...");

    g_object_set(identity, "sync", true, NULL);
    g_object_set(sink, "playlist-root", config->playlist_root.c_str(), NULL);
    g_object_set(sink, "playlist-location", config->playlist_loc.c_str(), NULL);
    g_object_set(sink, "location", config->segment_loc.c_str(), NULL);
    g_object_set(sink, "target-duration", config->target_duration, NULL);
    g_object_set(sink, "playlist-length", config->playlist_length, NULL);
}

StreamBranch::StreamBranch(const std::string &playlistFolder, const std::string &playlistId, int targetDuration, int playlistLenght, int bias)
:   StreamBranch(std::make_shared<HLSConfig>(
        playlistFolder, playlistId, targetDuration, playlistLenght, bias 
    ))
{}

bool StreamBranch::loadBin() {
    gst_bin_add_many(bin, sink, muxer, identity, NULL);
    return gst_element_link_many(muxer, identity, sink, NULL);
}

void StreamBranch::unloadBin() {
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(bin, sink, identity, muxer, NULL);
}

GstPad* StreamBranch::getSinkPad(DataLine::LineType type) {
    GstPad* newPad = nullptr;
    OATPP_LOGI("StreamBranch", "Getting new pad");
    newPad = gst_element_request_pad_simple(muxer, "sink_%d");

    if (gst_pad_is_linked(newPad)) return nullptr;
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

GstElement *StreamBranch::getFirstElement() const {
    return this->muxer;
}

StreamBranch::~StreamBranch() {
    OATPP_LOGD("StreamBranch", "destroyed one");

    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
}