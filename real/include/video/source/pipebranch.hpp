#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <api/dto/sourceconfig.hpp>

class PipeBranch {
public:
    PipeBranch(const std::string &sink, const SourceConfigDto &config);

    GstElement *getQueue();
    GstElement *getSink();

    void setCallback();
    void setPlay();
    bool loadBin(GstBin* bin);

private:
    GstElement* createEncoder(Videoserver::Accelerator accelerator);

private:
    std::string uuid;
    SourceConfigDto dtoConfig;

    GstElement* queue;
    GstElement* videoscale;
    GstElement* videorate;
    GstElement* videoencoder;
    GstElement* sink;
};