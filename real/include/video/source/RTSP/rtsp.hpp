#pragma once

#include <video/source/source.hpp>
#include <api/dto/sourceconfig.hpp>

class RTSPSource : public Source { 
public:
    RTSPSource(
        const std::string &name,
        const std::string &host,
        const Videoserver::Accelerator accelerator,
        const SourceConfigDto& dto
    );

private:
    std::string host;
    SourceConfigDto dtoConfig;

    GstElement* createEncoder(Videoserver::Accelerator accelerator);
    GstElement* createSrc(const std::string &host);
    GstElement* createAppsink();
    GstElement* createFilesink();
};