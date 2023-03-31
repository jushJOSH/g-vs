#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

#include <memory>
#include <unordered_map>
#include <mutex>

#include <video/sample/sample.hpp>
#include <video/source/pipetree.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/circular_buffer.hpp>

class PipeTree;

class Source {
protected: 
    struct CallbackArg {
        std::mutex mutex;
        std::shared_ptr<boost::circular_buffer<std::shared_ptr<Sample>>> samples;
    };

public:
    // Test consturctor for source
    Source();
    Source(const std::string& source);
    ~Source();

    std::shared_ptr<Sample> getSample();
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);

    bool addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch);
    //bool Source::addTestBranch();
    
    void waitSample() const;

protected:
    std::string uuid;
    PipeTree sourceElements;
    std::shared_ptr<CallbackArg> arg;

    static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data);
};