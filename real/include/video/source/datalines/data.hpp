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

    operator GstElement*();
    operator GstBin*();

    std::string getUUID() const;
    LineType getType() const;

protected:
    virtual void generateSrcPad() const = 0;
    void generateSinkPad() const;
    virtual GstElement* createEncoder() = 0;

protected:
    LineType type;
    std::string uuid;
    std::string encoder_s;

    GstBin *bin;
    GstElement *queue;
};