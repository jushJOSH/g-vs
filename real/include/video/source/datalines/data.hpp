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
    bool attachToPipeline(GstElement *before);
    GstPadLinkReturn attachToPipeline(GstPad *before);
    void detachFromPipeline(GstElement *before);
    bool detachFromPipeline(GstPad *before);

    GstPad* generateNewPad();

    GstElement *getQueue() const;
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
    GstElement *queue;
    GstElement *tee;
};