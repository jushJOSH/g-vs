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
    for (auto &filter : filters) gst_element_set_state(*filter, state);
    return gst_element_set_state(GST_ELEMENT(bin), state);
}

std::string PipeBranch::getUUID() const {
    return uuid;
}

std::shared_ptr<DataLine> PipeBranch::createDataline(const std::pair<std::string, GstElement*> &pad) {
    g_print("PipeBranch: creating new dataline\n");
    
    std::shared_ptr<DataLine> newLine;
    auto &[type, g_pad] = pad;

    if (type == "audio/x-raw") {
        newLine = std::make_shared<AudioLine>(
            config.audioencoding,
            config.quality,
            config.volume);
        std::reinterpret_pointer_cast<AudioLine>(newLine)->mute(config.mute);
    }
    else if (type == "video/x-raw") {
        newLine = std::make_shared<VideoLine>(
            config.videoencoding,
            VideoLine::strToResolution(config.resolution),
            config.fps,
            config.bitrate
        );
    }
    else newLine = nullptr;

    return newLine;
}

bool PipeBranch::attachToPipeline(const std::vector<std::pair<std::string, GstElement*>> &pads, GstBin* parentBin) {
    // First of all add branch to pipeline
    gst_bin_add(parentBin, GST_ELEMENT(this->bin));
    
    for (auto &pad : pads) {
        g_print("PipeBranch: creating %s dataline\n", pad.first.c_str());
        
        auto dataline = createDataline(pad);
        gst_bin_add(parentBin, *dataline);

        auto filterSinkPad = gst_element_get_static_pad(*dataline, "sink");
        auto srcPad = gst_element_get_request_pad(pad.second, "src_%u");
        auto branchLinkResult = gst_pad_link(srcPad, filterSinkPad);
        if (branchLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
        {
            g_print("PipeBranch: failed to link dataline and uribindecode\n");
            gst_bin_remove(parentBin, *dataline);
            continue;
        }
        else this->pads.push_back({pad.first, srcPad});

        auto filterSrcPad = gst_element_get_static_pad(*dataline, "src");
        auto branchPad = getSinkPad(dataline->getType());
        if (gst_pad_is_linked(branchPad)) break;
        
        auto filterLinkResult = gst_pad_link(filterSrcPad, branchPad);
        if (filterLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK) {
            g_print("PipeBranch: failed to link dataline and branch\n");
            gst_bin_remove(parentBin, *dataline);
            continue;
        }
        filters.push_back(dataline);
    }
    if (!filters.size())
        gst_bin_remove(parentBin, GST_ELEMENT(this->bin));

    return filters.size();
}

PipeBranch::~PipeBranch() {
    g_print("PipeBranch: destroyed one\n");

    try {
        auto parent = gst_pad_get_parent_element(pads[0].second);
        for (auto &pad : pads)
            gst_element_release_request_pad(parent, pad.second);
    } catch (const std::exception &e) {
        g_print("PipeBranch: pad remove error: %s\n", e.what());
    }
}