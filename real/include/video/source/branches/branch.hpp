#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <api/dto/sourceconfig.hpp>

class PipeBranch {
public:
    PipeBranch(const SourceConfigDto &config);
    virtual ~PipeBranch();

    GstElement *getQueue();
    GstElement *getSink();

    void setCallback(GCallback callback, gpointer* callbackArg);
    virtual bool loadBin(GstBin* bin);
    virtual void unloadBin();

    void updateRate(int fps);
    void updateResolution(int width, int heigth);

protected:
    std::string uuid;
    SourceConfigDto dtoConfig;

    GstElement* queue;
    GstElement* videoscale;
    GstElement* videorate;
    GstElement* videoencoder;
    GstElement* sink;

    GstBin* bin;
};