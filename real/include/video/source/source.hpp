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
    Source(const std::string& source, SourceConfigDto& config);
    ~Source();

    // Get/Set things
    std::string getUUID() const;
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);
    void setConfig(SourceConfigDto& config);

    // Branch management
    // Stream
    std::string runStream(GCallback callback);
    void stopStream(const std::string& name);

    // Archive
    std::string runArchive();
    void stopArchive(const std::string &name);
    
    // Screenshot
    std::string runScreenshot();
    void stopScreenshot(const std::string &name);
    
    // Sample things
    std::shared_ptr<Sample> getSample();
    void waitSample() const;
    static GstFlowReturn onNewSample(GstElement* appsink, CallbackArg *data);

protected:
    std::string source;

    std::string uuid;
    std::shared_ptr<PipeTree> sourceElements;
    std::shared_ptr<CallbackArg> arg;
};