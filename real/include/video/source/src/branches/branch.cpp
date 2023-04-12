#include <video/source/branches/branch.hpp>

#include <video/source/datalines/audio.hpp>
#include <video/source/datalines/video.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;


PipeBranch::PipeBranch(const std::string &sink, const std::string &muxer) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    muxer(gst_element_factory_make(muxer.c_str(), str(format("%1%_%2%") % uuid % muxer).c_str())),
    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_%2%") % uuid % sink).c_str())),
    bin(GST_BIN(gst_bin_new(str(format("%1%_bin") % uuid).c_str())))
{   
    g_print("Branch created %s\n", uuid.c_str());
}

GstElement* PipeBranch::getLastElement() const {
    return this->sink;
}

GstBin* PipeBranch::getBin() const {
    return bin;
}

GstStateChangeReturn PipeBranch::setState(GstState state) {
    return gst_element_set_state(GST_ELEMENT(bin), state);
}

std::string PipeBranch::getUUID() const {
    return uuid;
}

std::shared_ptr<DataLine> PipeBranch::createDataline(const std::pair<std::string, GstPad*> &pad) {
    g_print("PipeBranch: creating new dataline\n");
    
    std::shared_ptr<DataLine> newLine;
    auto &[type, g_pad] = pad;

    if (type == "audio/x-raw") {
        newLine = std::make_shared<AudioLine>(
            config.audioencoding,
            config.quality,
            config.volume);
        g_object_set(std::reinterpret_pointer_cast<AudioLine>(newLine)->getVolume(), "mute", config.mute, NULL);
        gst_bin_add(this->bin, GST_ELEMENT(newLine->getBin()));
    }
    else if (type == "video/x-raw") {
        newLine = std::make_shared<VideoLine>(
            config.videoencoding,
            VideoLine::strToResolution(config.resolution),
            config.fps,
            config.bitrate
        );
        gst_bin_add(this->bin, GST_ELEMENT(newLine->getBin()));
    }
    else newLine = nullptr;

    return newLine;
}

bool PipeBranch::attachToPipeline(const std::vector<std::pair<std::string, GstPad*>> &pads) {
    for (auto &pad : pads) {
        g_print("PipeBranch: creating %s dataline\n", pad.first.c_str());
        
        auto dataline = createDataline(pad);

        auto filterPad  = dataline->generateSrcPad();
        auto newBranchPad = getNewPad(dataline->getType());

        if (gst_pad_is_linked(newBranchPad)) break;

        auto filterLinkResult = gst_pad_link_full(filterPad, newBranchPad,
                                                (GstPadLinkCheck)(GST_PAD_LINK_CHECK_DEFAULT ^ GST_PAD_LINK_CHECK_HIERARCHY));
        if (filterLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK) {
            g_print("PipeBranch: failed to link dataline and branch\n");
            return false;
        }

        auto branchLinkResult = gst_pad_link_full(pad.second, dataline->generateSinkPad(),
                                                 (GstPadLinkCheck)(GST_PAD_LINK_CHECK_DEFAULT ^ GST_PAD_LINK_CHECK_HIERARCHY));
        if (branchLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
        {
            g_print("PipeBranch: failed to link dataline and uribindecode\n");
            return false;
        }

        filters.push_back(dataline);
    }

    return true;
}

void PipeBranch::updateBitrate(int bitrate) {
    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Video) {
            auto encoder = std::reinterpret_pointer_cast<VideoLine>(filter)->getEncoder();
            g_object_set(encoder, "bitrate", bitrate, NULL);
        }
    }
}

void PipeBranch::updateResolution(const std::string resolution) {
    auto targetResolution = VideoLine::strToResolution(resolution);

    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Video) {
            auto scale = std::reinterpret_pointer_cast<VideoLine>(filter)->getScale();
            g_object_set(scale, "width", targetResolution.width, NULL);
            g_object_set(scale, "height", targetResolution.height, NULL);
        }
    }
}

void PipeBranch::updateFrameRate(int fps) {
    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Video) {
            auto rate = std::reinterpret_pointer_cast<VideoLine>(filter)->getRate();
            g_object_set(rate, "max-rate", fps, NULL);
        }
    }
}

void PipeBranch::updateAudioQuality(double quality) {
    if (quality > 1.0) quality = 1.0;
    else if (quality < 0.0) quality = 0.0;

    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Audio) {
            auto encoder = std::reinterpret_pointer_cast<AudioLine>(filter)->getEncoder();
            g_object_set(encoder, "quality", quality, NULL);
        }
    }
}

void PipeBranch::updateAudioVolume(double volume) {
    if (volume > 1.0) volume = 1.0;
    else if (volume < 0.0) volume = 0.0;

    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Audio) {
            auto volume_g = std::reinterpret_pointer_cast<AudioLine>(filter)->getVolume();
            g_object_set(volume_g, "volume", volume, NULL);
        }
    }
}

void PipeBranch::mute(bool state) {
    for (auto &filter : filters) {
        if (filter->getType() == DataLine::LineType::Audio) {
            auto volume = std::reinterpret_pointer_cast<AudioLine>(filter)->getVolume();
            g_object_set(volume, "mute", state, NULL);
        }
    }
}

PipeBranch::~PipeBranch() {
    g_print("PipeBranch: destroyed one\n");
}