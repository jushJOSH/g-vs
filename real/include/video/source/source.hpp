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
    std::string getSource() const;
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING);
    std::shared_ptr<StreamBranch::CallbackArg> getArg();
    void setConfig(SourceConfigDto& config);
    bool isEmpty() const;

    // Branch management
    // Stream
    std::shared_ptr<StreamBranch> runStream();

    // Archive
    std::shared_ptr<ArchiveBranch> runArchive(const std::string &path);
    
    void removeBranch(const std::string& branchid);

protected:
    std::string source;

    GstPipeline* pipeline;

    std::string uuid;
    std::unique_ptr<PipeTree> sourceElements;
};