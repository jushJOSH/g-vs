#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

#include <memory>
#include <unordered_map>
#include <mutex>

#include <video/sample/sample.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/circular_buffer.hpp>

class Source {
private:
    struct PipeElements {
        GstElement* pipeline;
        GstElement* source;
        GstElement* converter;
        GstElement* encoder;
        GstElement* sink;
        GstElement* muxer;
    };

    struct CallbackArg {
        std::mutex mutex;
        std::shared_ptr<boost::circular_buffer<std::shared_ptr<Sample>>> samples;
    };

public:
    Source();
    ~Source();

    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 
    std::shared_ptr<Sample> getSample();
    void waitSample() const;

private:
    std::shared_ptr<std::string> uuid;
    PipeElements sourceElements;
    std::shared_ptr<CallbackArg> arg;

    static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data);
};