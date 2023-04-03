#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <video/source/datalines/dataline.hpp>
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
    PipeBranch(const SourceConfigDto &config, const std::string& muxer = "", const std::string& sink = "");
    PipeBranch(GstBin* bin, const SourceConfigDto &config, const std::string& muxer = "", const std::string& sink = "");
    //virtual ~PipeBranch();

    // Getters
    GstElement *getQueue() const;
    GstElement *getSink() const;
    GstBin* getBin() const;

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
    virtual bool loadBin(GstBin *bin);
    virtual void unloadBin();
    
protected:
    static void onNewPad(GstElement* element, GstPad* newPad, gpointer userData);
    virtual void initPadEvent();

protected:
    std::string uuid;
    std::shared_ptr<DataLines> datalines;
    SourceConfigDto config;

    GstElement *queue;
    GstElement *decodebin;
    GstElement* muxer;
    GstElement* sink;

    GstBin *bin;
};