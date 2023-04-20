#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>
#include <glib.h>

#include <memory>
#include <unordered_map>
#include <mutex>

#include <video/source/pipetree.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

class PipeTree;

class Source {
public:
    struct BusCallbackData { 
        GCallback callback;
        void* data;
    };

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
    void setConfig(SourceConfigDto& config);
    bool isEmpty() const;
    void setRemoveBranchCallback(const std::function<void(void*)> callback, void* data);

    // Branch management
    // Stream
    std::shared_ptr<StreamBranch> runStream(const std::string &hlsFolder);

    // Archive
    std::shared_ptr<ArchiveBranch> runArchive(const std::string &path);
    
    void removeBranch(const std::string& branchid);

    void addBusCallback(const std::string &message, BusCallbackData data);

protected:
    std::string source;
    std::string uuid;
    std::unique_ptr<PipeTree> sourceElements;

    GstBus* bus;
};