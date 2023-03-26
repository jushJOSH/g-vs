#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

#include <memory>
#include <unordered_map>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <video/sample/sample.hpp>

class Source {
public:
    Source();
    ~Source();

    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 
    boost::uuids::uuid getUUID() const;
    std::tuple<bool, Sample> getSample() const;

private:
    std::shared_ptr<boost::uuids::uuid> uuid;

    struct PipeElements {
        GstElement* pipeline;
        GstElement* source;
        GstElement* converter;
        GstElement* encoder;
        GstElement* sink;
    };
    
    PipeElements sourceElements;
    
    static std::unordered_map<std::string, std::tuple<bool, Sample>> sampleCloset;
    static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data);
};