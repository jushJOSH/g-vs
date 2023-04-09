#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <string>

// Setting to change
// - Quality
// - Volume
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

    GstPad* generateNewPad();

    virtual GstElement *getFirstElement() const = 0;
    GstElement *getTee() const;
    std::string getUUID() const;
    LineType getType() const;

protected:
    virtual GstElement* createEncoder() = 0;

protected:
    LineType type;
    std::string uuid;
    std::string encoder_s;

    GstBin *bin = nullptr;
    GstElement *tee;
};