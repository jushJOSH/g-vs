#include <video/source/branches/branch.hpp>

#include <video/source/datalines/audioline.hpp>
#include <video/source/datalines/videoline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void PipeBranch::initPadEvent() {
    datalines->bin = this->bin;
    datalines->dtoConfig = this->config;
}

PipeBranch::PipeBranch(const SourceConfigDto &config, const std::string &muxer, const std::string &sink) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    config(config),
    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    decodebin(gst_element_factory_make("decodebin", str(format("%1%_decodebin") % uuid).c_str())),
    muxer(gst_element_factory_make(muxer.c_str(), str(format("%1%_%2%") % uuid % muxer).c_str())),
    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_%2%") % uuid % sink).c_str())),
    datalines(std::make_shared<DataLines>())
{   
    datalines->dtoConfig = this->config;
    g_print("Branch created %s\n", uuid.c_str());
}

PipeBranch::PipeBranch(GstBin* bin, const SourceConfigDto &config, const std::string &muxer, const std::string &sink) 
:   PipeBranch(config, muxer, sink)
{
    if (!loadBin(bin))
        throw std::runtime_error("PipeBranch: failed to link queue and decodebin");

    datalines->bin = this->bin;
    initPadEvent();
}

void PipeBranch::onNewPad(GstElement* element, GstPad* newPad, gpointer userData) {
    g_print ("Received new pad '%s'\n", GST_PAD_NAME (newPad));
    auto linedata = (DataLines*)userData;

    // New data line for video or audio
    std::shared_ptr<DataLine> newLine = nullptr;

    // Check new pad type
    GstCaps *pad_caps = gst_pad_get_current_caps (newPad);
    GstStructure *pad_struct = gst_caps_get_structure (pad_caps, 0);
    const gchar *pad_type = gst_structure_get_name (pad_struct);
    if (g_str_has_prefix (pad_type, "audio/x-raw")) {
        newLine = std::make_shared<AudioLine>(
            linedata->bin,
            linedata->dtoConfig.audioencoding,
            linedata->dtoConfig.quality,
            linedata->dtoConfig.volume);
        g_object_set(std::reinterpret_pointer_cast<AudioLine>(newLine)->getVolume(), "mute", linedata->dtoConfig.mute, NULL);
    }
    else if (g_str_has_prefix(pad_type, "video/x-raw")) {
        newLine = std::make_shared<VideoLine>(
            linedata->bin,
            linedata->dtoConfig.videoencoding,
            VideoLine::strToResolution(linedata->dtoConfig.resolution),
            linedata->dtoConfig.fps,
            linedata->dtoConfig.bitrate
        );
    }
    else return;

    // Linking pad
    if (!newLine->attachToPipeline(newPad) || 
        !gst_element_link(newLine->getEncoder(), linedata->nextelement)) {
        g_print ("Type is '%s' but link failed.\n", pad_type);
        return;
    } else {
        g_print ("Link succeeded (type '%s').\n", pad_type);
    }

    linedata->datalines.push_back(newLine);
    // Unreference the new pad's caps, if we got them
    if (pad_caps != NULL)
        gst_caps_unref (pad_caps);
}

GstElement* PipeBranch::getQueue() const {
    return this->queue;
}

GstElement* PipeBranch::getSink() const {
    return this->sink;
}

void PipeBranch::setMuxer(GstElement* muxer) {
    this->muxer = muxer;
    initPadEvent();
}

void PipeBranch::setSink(GstElement* sink) {
    this->sink = sink;

    if (this->muxer && this->sink)
        gst_element_link(this->muxer, this->sink);
}

bool PipeBranch::loadBin(GstBin *bin) {
    if (this->bin) return true;

    gst_bin_add_many(bin, queue, decodebin, muxer, sink, NULL);
    
    return 
        gst_element_link(queue, decodebin) &&
        gst_element_link(muxer, sink);
}

void PipeBranch::unloadBin() {
    gst_bin_remove_many(bin, queue, decodebin, muxer, sink, NULL);
    bin = nullptr;
}

GstBin*  PipeBranch::getBin() const {
    return bin;
}

GstStateChangeReturn PipeBranch::setState(GstState state) {
    return gst_element_set_state(queue, state);
}

void PipeBranch::updateBitrate(int bitrate) {
    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto encoder = std::reinterpret_pointer_cast<VideoLine>(dataline)->getEncoder();
            g_object_set(encoder, "bitrate", bitrate, NULL);
        }
    }
}

void PipeBranch::updateResolution(const std::string resolution) {
    auto targetResolution = VideoLine::strToResolution(resolution);

    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto scale = std::reinterpret_pointer_cast<VideoLine>(dataline)->getScale();
            g_object_set(scale, "width", targetResolution.width, NULL);
            g_object_set(scale, "height", targetResolution.height, NULL);
        }
    }
}

void PipeBranch::updateFrameRate(int fps) {
    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto rate = std::reinterpret_pointer_cast<VideoLine>(dataline)->getRate();
            g_object_set(rate, "max-rate", fps, NULL);
        }
    }
}

void PipeBranch::updateAudioQuality(double quality) {
    if (quality > 1.0) quality = 1.0;
    else if (quality < 0.0) quality = 0.0;

    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto encoder = std::reinterpret_pointer_cast<AudioLine>(dataline)->getEncoder();
            g_object_set(encoder, "quality", quality, NULL);
        }
    }
}

void PipeBranch::updateAudioVolume(double volume) {
    if (volume > 1.0) volume = 1.0;
    else if (volume < 0.0) volume = 0.0;

    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto volume_g = std::reinterpret_pointer_cast<AudioLine>(dataline)->getVolume();
            g_object_set(volume_g, "volume", volume, NULL);
        }
    }
}

void PipeBranch::mute(bool state) {
    for (auto &dataline : datalines->datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto volume = std::reinterpret_pointer_cast<AudioLine>(dataline)->getVolume();
            g_object_set(volume, "mute", state, NULL);
        }
    }
}