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
    bin(nullptr)
{   
    g_print("Branch created %s\n", uuid.c_str());
}

GstElement* PipeBranch::getSink() const {
    return this->sink;
}

void PipeBranch::setMuxer(GstElement* muxer) {
    this->muxer = muxer;
}

void PipeBranch::setSink(GstElement* sink) {
    this->sink = sink;
}

GstBin*  PipeBranch::getBin() const {
    return bin;
}

GstStateChangeReturn PipeBranch::setState(GstState state) {
    for (auto &filter : filters) {
        if (filter->setState(state) == GST_STATE_CHANGE_FAILURE)
            return GST_STATE_CHANGE_FAILURE;
    }

    return gst_element_set_state(muxer, state);
}

std::string PipeBranch::getUUID() const {
    return uuid;
}

GstElement *PipeBranch::getMuxer() const {
    return muxer;
}

std::shared_ptr<DataLine> PipeBranch::createDataline(const std::pair<std::string, GstPad*> &pad) {
    g_print("PipeBranch: creating new dataline\n");
    
    std::shared_ptr<DataLine> newLine;
    auto &[type, g_pad] = pad;

    if (type == "audio/x-raw") {
        newLine = std::make_shared<AudioLine>(
            bin,
            config.audioencoding,
            config.quality,
            config.volume);
        g_object_set(std::reinterpret_pointer_cast<AudioLine>(newLine)->getVolume(), "mute", config.mute, NULL);
    }
    else if (type == "video/x-raw") {
        newLine = std::make_shared<VideoLine>(
            bin,
            config.videoencoding,
            VideoLine::strToResolution(config.resolution),
            config.fps,
            config.bitrate
        );
    }
    else newLine = nullptr;

    return newLine;
}

bool PipeBranch::attachToPipeline(const std::vector<std::pair<std::string, GstPad*>> &pads) {
    for (auto &pad : pads) {
        g_print("StreamBranch: creating %s dataline\n", pad.first.c_str());
        
        auto dataline = createDataline(pad);

        auto filterPad  = dataline->generateSrcPad();
        auto newBranchPad = getNewPad(dataline->getType());

        if (gst_pad_is_linked(newBranchPad)) break;

        auto filterLinkResult = gst_pad_link(filterPad, newBranchPad);
        if (filterLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK) {
            g_print("StreamBranch: failed to link dataline and branch\n");
            return false;
        }

        auto branchLinkResult = dataline->attachToPipeline(pad.second);
        if (branchLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
        {
            g_print("StreamBranch: failed to link dataline and uribindecode\n");
            return false;
        }

        filters.push_back(dataline);
    }

    return true;
}