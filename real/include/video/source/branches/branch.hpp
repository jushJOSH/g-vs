#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <video/source/datalines/data.hpp>
#include <api/dto/sourceconfig.hpp>

class PipeBranch
{
private: 
    struct DataLines {
        GstBin* bin;
        GstElement* nextelement;
        SourceConfigDto dtoConfig;
        std::vector<std::shared_ptr<DataLine>> datalines;
    };

public:
    PipeBranch(const std::string& sink, const std::string& muxer = "");
    //virtual ~PipeBranch();

    // Getters
    virtual GstPad *getNewPad(DataLine::LineType type) = 0;
    GstElement *getSink() const;
    GstElement *getMuxer() const;
    GstBin* getBin() const;
    std::string getUUID() const;

    // Update params
    void updateBitrate(int bitrate);
    void updateResolution(const std::string resolution);
    void updateFrameRate(int fps);
    void updateAudioQuality(double quality);
    void updateAudioVolume(double volume);
    void mute(bool state);

    // Setters
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);
    void setMuxer(GstElement* muxer);
    void setSink(GstElement* sink);

    // Bin ops
    virtual bool loadBin(GstBin *bin) = 0;
    virtual void unloadBin() = 0;
    
protected:
    static void onNoMorePads(GstElement* src, GstElement *next);

protected:
    std::string uuid;
    std::shared_ptr<DataLines> datalines;
    SourceConfigDto config;

    GstElement* muxer;
    GstElement* sink;

    GstBin *bin;
};