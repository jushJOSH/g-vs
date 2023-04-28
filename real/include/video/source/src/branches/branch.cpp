#include <video/source/branches/branch.hpp>
#include <video/source/datalines/audio.hpp>
#include <video/source/datalines/video.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <oatpp/core/base/Environment.hpp>
#include <video/source/pipetree.hpp>

using boost::format;
using boost::str;

PipeBranch::PipeBranch(const std::string &sink, const std::string &muxer, std::shared_ptr<SourceDto> config) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    muxer(gst_element_factory_make(muxer.c_str(), str(format("%1%_%2%") % uuid % muxer).c_str())),
    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_%2%") % uuid % sink).c_str())),
    bin(GST_BIN(gst_bin_new(str(format("%1%_bin") % uuid).c_str())))
{   
    this->config = config == nullptr ? std::make_shared<SourceDto>() : config;
    OATPP_LOGD("PipeBranch", "Branch created %s", uuid.c_str());
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
    OATPP_LOGI("PipeBranch", "creating new filter");
    
    std::shared_ptr<DataLine> newLine;

    if (padType == "audio/x-raw") {
        newLine = std::make_shared<AudioLine>(
            //config->quality,
            config->volume);
        std::reinterpret_pointer_cast<AudioLine>(newLine)->mute(config->mute);
    }
    else if (padType == "video/x-raw") {
        newLine = std::make_shared<VideoLine>(
            config->videoencoding,
            //VideoLine::strToResolution(config->resolution),
            config->fps,
            config->bitrate
        );
    }
    else newLine = nullptr;

    return newLine;
}

GstPadLinkReturn PipeBranch::addFilter(std::shared_ptr<DataLine> filter) {
    auto filterSrcPad = gst_element_get_static_pad(*filter, "src");
    auto branchPad = getSinkPad(filter->getType());

    if (gst_pad_is_linked(branchPad)) return GST_PAD_LINK_WAS_LINKED;

    auto linkResult = gst_pad_link(filterSrcPad, branchPad);
    gst_object_unref(filterSrcPad);

    return linkResult;
}

bool PipeBranch::attachToPipeline(const std::vector<std::pair<std::string, GstElement*>> &pads, GstBin* parentBin) {
    // First of all add branch to pipeline
    gst_bin_add(parentBin, GST_ELEMENT(this->bin));
    if (!sync())
        OATPP_LOGW("PipeBranch", "sync failed!");
    
    for (auto &pad : pads) {
        OATPP_LOGI("PipeBranch", "creating %s dataline", pad.first.c_str());
        
        auto dataline = createFilter(pad.first);
        gst_bin_add(parentBin, *dataline);
        if (!dataline->sync())
            OATPP_LOGW("PipeBranch", "Filter %s sync failed!", dataline->getUUID().c_str());
        
        auto filterLinkResult = addFilter(dataline);
        if (filterLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK) {
            OATPP_LOGE("PipeBranch", "failed to link dataline and branch");
            gst_bin_remove(parentBin, *dataline);
            continue;
        }
        
        auto teeSrcPad = gst_element_request_pad_simple(pad.second, "src_%u");
        auto branchLinkResult = dataline->attachToPipeline(teeSrcPad);
        if (branchLinkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
        {
            OATPP_LOGE("PipeBranch", "failed to link dataline and uribindecode");
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
    OATPP_LOGD("PipeBranch", "destroyed one");
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