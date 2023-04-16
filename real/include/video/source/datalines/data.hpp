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
    
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);

    GstPadLinkReturn attachToPipeline(GstPad* srcpad);
    void detachFromPipeline();

    GstPad* getPreviousPad() const;
    GstElement* getEncoder() const;
    operator GstElement*();
    operator GstBin*();

    std::string getUUID() const;
    LineType getType() const;

    bool sync();
protected:
    virtual void generateSrcPad() const = 0;
    virtual GstElement* createEncoder() = 0;
    void generateSinkPad() const;

protected:
    LineType type;
    std::string uuid;
    std::string encoder_s;

    GstBin *bin;
    GstElement *queue;
    GstElement *encoder;
    
    GstPad* previousPad;
};