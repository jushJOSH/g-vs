#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <string>

class DataLine
{
public:
    struct Resolution
    {
        int width;
        int height;
    };

    enum class LineType {
        Video, Audio
    };

public:
    DataLine(LineType type, const std::string& encoder);

    virtual void loadBin(GstBin *bin) = 0;
    virtual void unloadBin() = 0;
    virtual bool attachToPipeline(GstElement *before) = 0;
    virtual GstPadLinkReturn attachToPipeline(GstPad *before) = 0;
    virtual void detachFromPipeline(GstElement *before) = 0;
    virtual bool detachFromPipeline(GstPad *before) = 0;
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);

    virtual GstPad* generateSrcPad() = 0;
    GstPad* generateSinkPad();

    virtual GstElement *getFirstElement() const = 0;
    virtual GstElement *getLastElement() const = 0;
    std::string getUUID() const;
    LineType getType() const;

protected:
    virtual GstElement* createEncoder() = 0;

protected:
    LineType type;
    std::string uuid;
    std::string encoder_s;

    GstBin *bin = nullptr;
    GstElement *queue;
};