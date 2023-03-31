#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

#include <memory>
#include <unordered_map>
#include <mutex>

#include <video/sample/sample.hpp>
#include <video/videoserver/videoserver.hpp>
#include <video/source/pipetree.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/circular_buffer.hpp>

class Source {
protected: 
    struct CallbackArg {
        std::mutex mutex;
        std::shared_ptr<boost::circular_buffer<std::shared_ptr<Sample>>> samples;
    };

public:
    // Test consturctor for source
    Source();
    ~Source();

    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 
    std::shared_ptr<Sample> getSample();

    /// @brief Adds branch to source tree
    /// @param name name of branch. Uuid for example
    /// @param branch connected and configured branch to insert
    /// @return true if connected successfully
    void addTestBranch();
    
    void waitSample() const;

protected:
    std::string uuid;
    PipeTree sourceElements;
    std::shared_ptr<CallbackArg> arg;

    static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data);
};