#pragma once

#include <oatpp/core/macro/component.hpp>

#include <api/dto/config.hpp>

#include <video/source/source.hpp>
#include <video/source/branches/stream.hpp>

#include <utils/filemanager.hpp>

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

        std::atomic_int packetCount = 0;
        gulong callback;
    };

public:
    LiveHandler(const std::string &sourceUri);
    ~LiveHandler();

    oatpp::String getPlaylist();
    VSTypes::OatResponse getSegment(const oatpp::String &requestedSegment);
    std::string getUUID() const;

    bool isReady() const;
    std::mutex &getMutex();
    std::condition_variable &getCv();
    timestamp getLastTimestamp() const;
    int getBias() const;
    int getSegmentDuration() const;

    std::shared_ptr<Source> getSource() const;
    std::string getSourceUri() const;

    oatpp::String guessMime(const oatpp::String &filename) const;

private:
    static bool onElementMessage(GstBus *bus, GstMessage *message, gpointer data);

private:
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
    
    ReadyNotificationBundle bundle;
    std::shared_ptr<HLSConfig> hlsconfig;

    std::string source_uri;
    std::shared_ptr<Source> source;
    std::shared_ptr<StreamBranch> streamBranch;
    std::shared_ptr<StaticFilesManager> fileManager;
    
    timestamp lastSegmentRequest;
};