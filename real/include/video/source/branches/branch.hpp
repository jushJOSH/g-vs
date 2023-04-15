#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <video/source/datalines/data.hpp>
#include <api/dto/sourceconfig.hpp>

class PipeBranch
{
public:
    PipeBranch(const std::string& sink, const std::string& muxer = "");
    virtual ~PipeBranch();

    // Getters
    std::vector<std::shared_ptr<DataLine>> getFilters() const;
    virtual GstPad *getSinkPad(DataLine::LineType type) = 0;
    virtual GstElement *getFirstElement() const = 0;
    std::vector<GstPad*> getPads() const;
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
    bool syncState();
    void setMuxer(GstElement* muxer);
    void setSink(GstElement* sink);

    // Bin ops
    virtual void unloadBin() = 0;
    virtual bool loadBin() = 0;
    
    // Attach to pipeline
    bool attachToPipeline(const std::vector<std::pair<std::string, GstElement*>> &pads, GstBin* parentBin);

protected:
    std::shared_ptr<DataLine> createDataline(const std::string &padType);

protected:
    std::vector<std::shared_ptr<DataLine>> filters;
    std::vector<GstPad*> pads;
    std::string uuid;

    SourceConfigDto config;

    GstElement* muxer;
    GstElement* sink;
    GstBin *bin;
};