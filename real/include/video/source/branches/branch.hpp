#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <video/source/datalines/data.hpp>
#include <api/db/dto/source.hpp>

#include <misc.hpp>

class PipeBranch
{
public:
    PipeBranch(const std::string& sink, const std::string& muxer = "", std::shared_ptr<SourceDto> config = nullptr);
    virtual ~PipeBranch();

    // Getters
    std::vector<std::shared_ptr<DataLine>> getFilters() const;
    virtual GstPad *getSinkPad(DataLine::LineType type) = 0;
    virtual GstElement *getFirstElement() const = 0;
    GstElement *getLastElement() const;
    std::string getUUID() const;
    GstBin* getBin() const;

    // Update params
    void updateResolution(const std::string resolution);
    void updateAudioQuality(double quality);
    void updateAudioVolume(double volume);
    void updateBitrate(int bitrate);
    void updateFrameRate(int fps);
    void mute(bool state);

    // Setters
    GstPadLinkReturn addFilter(std::shared_ptr<DataLine> filter);
    void setMuxer(GstElement* muxer);
    void setSink(GstElement* sink);
    bool syncState();

    // Remove
    void removeFilter(const std::string &uuid);

    // Bin ops
    virtual void unloadBin() = 0;
    virtual bool loadBin() = 0;
    
    // Attach to pipeline
    bool attachToPipeline(const std::vector<std::pair<std::string, GstElement*>> &pads, GstBin* parentBin);
    bool sync();

protected:
    std::shared_ptr<DataLine> createFilter(const std::string &padType);

protected:
    std::vector<std::shared_ptr<DataLine>> filters;
    std::string uuid;

    std::shared_ptr<SourceDto> config;

    GstElement* muxer;
    GstElement* sink;
    GstBin *bin;
};