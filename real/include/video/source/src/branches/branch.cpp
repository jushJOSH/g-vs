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

bool PipeBranch::syncState() {
    bool result = true;
    for (auto &filter : filters) result = result && gst_element_sync_state_with_parent(*filter);
    return result && gst_element_sync_state_with_parent(GST_ELEMENT(bin));
}

std::string PipeBranch::getUUID() const {
    return uuid;
}

std::shared_ptr<DataLine> PipeBranch::createFilter(const std::string &padType) {
    g_print("PipeBranch: creating new filter\n");
    
    std::shared_ptr<DataLine> newLine;

    if (padType == "audio/x-raw") {
        newLine = std::make_shared<AudioLine>(
            config.audioencoding,
            config.quality,
            config.volume);
        std::reinterpret_pointer_cast<AudioLine>(newLine)->mute(config.mute);
    }
    else if (padType == "video/x-raw") {
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

GstPadLinkReturn PipeBranch::addFilter(std::shared_ptr<DataLine> filter) {
    auto filterSrcPad = gst_element_get_static_pad(*filter, "src");
    auto branchPad = getSinkPad(filter->getType());

    if (gst_pad_is_linked(branchPad)) GST_PAD_LINK_WAS_LINKED;

    auto linkResult = gst_pad_link(filterSrcPad, branchPad);
    gst_object_unref(filterSrcPad);

    return linkResult;
}

bool PipeBranch::attachToPipeline(const std::vector<std::pair<std::string, GstElement*>> &pads, GstBin* parentBin) {
    // First of all add branch to pipeline
    gst_bin_add(parentBin, GST_ELEMENT(this->bin));
    if (!sync())
        g_print("PipeBranch: sync failed!\n");
    
    for (auto &pad : pads) {
        g_print("PipeBranch: creating %s dataline\n", pad.first.c_str());
        
        auto dataline = createFilter(pad.first);
        gst_bin_add(parentBin, *dataline);
        if (!dataline->sync())
            g_print("Filter %s: sync failed!\n", dataline->getUUID().c_str());

        auto filterLinkResult = addFilter(dataline);
        if (filterLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK) {
            g_print("PipeBranch: failed to link dataline and branch\n");
            gst_bin_remove(parentBin, *dataline);
            continue;
        }
        
        auto teeSrcPad = gst_element_request_pad_simple(pad.second, "src_%u");
        auto branchLinkResult = dataline->attachToPipeline(teeSrcPad);
        if (branchLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
        {
            g_print("PipeBranch: failed to link dataline and uribindecode\n");
            gst_bin_remove(parentBin, *dataline);
            continue;
        }
        
        filters.push_back(dataline);
    }
    if (!filters.size())
    {
        gst_element_set_state(GST_ELEMENT(this->bin), GST_STATE_NULL);
        gst_bin_remove(parentBin, GST_ELEMENT(this->bin));
    }

    return filters.size();
}

PipeBranch::~PipeBranch() {
    g_print("PipeBranch: destroyed one\n");
}

std::vector<std::shared_ptr<DataLine>> PipeBranch::getFilters() const {
    return filters;
}

void PipeBranch::removeFilter(const std::string &uuid) {
    auto pos = std::find_if(filters.begin(), filters.end(), 
    [uuid](const std::shared_ptr<DataLine>& elem) {
        return elem->getUUID() == uuid;
    });

    if (pos == filters.end()) return;

    filters.erase(pos);
}

bool PipeBranch::sync() {
    return gst_element_sync_state_with_parent(GST_ELEMENT(bin));
}