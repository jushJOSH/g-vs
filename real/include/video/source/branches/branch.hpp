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
        SourceConfigDto dtoConfig;
        std::vector<std::shared_ptr<DataLine>> datalines;
    };

public:
    PipeBranch(const SourceConfigDto &config);
    virtual ~PipeBranch();

    GstElement *getQueue() const;
    virtual GstElement *getSink() const = 0;

    virtual void setCallback(GCallback callback, gpointer *callbackArg) = 0;
    virtual bool loadBin(GstBin *bin) = 0;
    virtual void unloadBin() = 0;
    
private:
    static void onNewPad(GstElement* element, GstPad* newPad, gpointer userData);

protected:
    std::string uuid;
    std::shared_ptr<DataLines> datalines;
    SourceConfigDto config;

    GstElement *queue;
    GstElement *decodebin;

    GstBin *bin;
};