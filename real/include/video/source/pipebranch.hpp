#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <api/dto/sourceconfig.hpp>

class PipeBranch {
public:
    PipeBranch(const std::string &sink, const SourceConfigDto &config);
    
    GstElement *getQueue();
    GstElement *getSink();

    void setCallback(GCallback callback, gpointer* callbackArg);
    bool loadBin(GstBin* bin);

    void updateEncoder(int bitrate);
    void updateRate(int fps);
    void updateResolution(int width, int heigth);

private:
    GstElement* createEncoder(int accelerator);

private:
    std::string uuid;
    SourceConfigDto dtoConfig;

    GstElement* queue;
    GstElement* videoscale;
    GstElement* videorate;
    GstElement* videoencoder;
    GstElement* sink;
};