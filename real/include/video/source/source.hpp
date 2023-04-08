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

class PipeTree;

class Source {
public:
    // Test consturctor for source
    Source();
    Source(const std::string& source);
    Source(const std::string& source, SourceConfigDto& config);
    ~Source();

    // Get/Set things
    std::string getUUID() const;
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);
    std::shared_ptr<StreamBranch::CallbackArg> getArg();
    void setConfig(SourceConfigDto& config);

    // Branch management
    // Stream
    void runStream(std::shared_ptr<StreamBranch> branch);

    // Archive
    std::string runArchive(std::shared_ptr<ArchiveBranch> branch);
    
    // Screenshot
    std::string makeScreenshot(std::shared_ptr<ScreenshotBranch> branch);
    
    // Sample things
    std::shared_ptr<Sample> getSample();
    void waitSample() const;
    static GstFlowReturn onNewSample(GstElement* appsink, StreamBranch::CallbackArg *data);
    
protected:
    std::string source;

    std::string uuid;
    std::shared_ptr<PipeTree> sourceElements;
};