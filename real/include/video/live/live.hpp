#pragma once

#include <oatpp/core/macro/component.hpp>

#include <api/dto/config.hpp>

#include <video/source/source.hpp>
#include <video/source/branches/stream.hpp>

#include <utils/filemanager.hpp>

#include <types.hpp>

class Videoserver;

class LiveHandler {
public:
    LiveHandler(const std::string &sourceUri);
    ~LiveHandler();

    oatpp::String getPlaylist();
    VSTypes::OatResponse getSegment(const oatpp::String &requestedSegment);
    std::string getUUID() const;

    oatpp::String guessMime(const oatpp::String &filename) const;

private:
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

    std::shared_ptr<Source> source;
    std::shared_ptr<StreamBranch> streamBranch;
    std::shared_ptr<StaticFilesManager> fileManager;
};