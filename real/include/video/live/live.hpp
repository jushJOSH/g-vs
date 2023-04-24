#pragma once

#include <oatpp/core/macro/component.hpp>

#include <api/dto/config.hpp>

#include <video/source/source.hpp>
#include <video/source/branches/stream.hpp>

#include <utils/filemanager.hpp>
#include <gio/gio.h>

#include <types.hpp>

class Videoserver;

typedef std::chrono::_V2::system_clock::time_point timestamp;

class LiveHandler {
private:
    struct ReadyNotificationBundle {
        LiveHandler *issuer;

        std::atomic_bool ready = false;
        std::mutex commonMutex;
        std::condition_variable cv;

        std::atomic_int segmentCount = 0;
    };

public:
    LiveHandler(const std::string &sourceUri);
    ~LiveHandler();

    oatpp::String getPlaylist();
    VSTypes::OatResponse getSegment(const oatpp::String &requestedSegment);

    bool isReady() const;
    std::mutex &getMutex();
    std::condition_variable &getCv();
    timestamp getLastTimestamp() const;
    int getBias() const;
    int getSegmentDuration() const;

    std::string getSourceUUID() const;
    std::string getSourceUri() const;
    std::string getBranchUUID() const;
    GFileMonitor* getWatchdog() const;

    oatpp::String guessMime(const oatpp::String &filename) const;

private:
    static void folderChanged(GFileMonitor *m, GFile* file, GFile *other, GFileMonitorEvent event, gpointer data);

private:
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
    
    ReadyNotificationBundle bundle;
    std::shared_ptr<HLSConfig> hlsconfig;
    
    GFileMonitor *hlswatchdog;

    std::string source_uri;
    std::string source_uuid;
    std::string branch_uuid;
    std::shared_ptr<StaticFilesManager> fileManager;
    
    timestamp lastSegmentRequest;
};