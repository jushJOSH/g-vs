#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <memory>

class Source {
public:
    Source();
    virtual ~Source();

private:
    struct PipeElements {
        GstElement* source;
        GstElement* converter;
        GstElement* encoder;
    };

    PipeElements sourceElements;
    std::shared_ptr<GstElement> pipeline;
    std::shared_ptr<GstElement> sink;
};