#pragma once

#include <types.hpp>
#include <iostream>

namespace VSMisc {
    template<class DTO_T>
    oatpp::Int32 fetchId(const VSTypes::OatQueryResult &qResult) {
        // Fetching id
        auto receivedData = qResult->fetch<oatpp::Vector<oatpp::Object<DTO_T>>>();

        if (receivedData->empty()) return -1; 
        else return receivedData[0]->id;
    }
};

    //     #include <iostream>
    // void printConfig(const SourceConfigDto& config) {
    //     std::cout
    //         << "Source settings\n"
    //         << "cache_mode: " << config.cache_mode->c_str() << "\n"
    //         << "archive_path: " << config.archive_path.getValue("") << "\n"
    //         << "\n"
    //         << "Videoline settings\n"
    //         << "fps: " << config.fps.getValue(-1) << "\n"
    //         << "resolution: " << config.resolution.getValue("") << "\n"
    //         << "encoding: " << config.videoencoding.getValue("") << "\n"
    //         << "\n"
    //         << "Audioline settings\n"
    //         << "mute: " << std::boolalpha << config.mute.getValue(false) << "\n"
    //         << "quality: " << config.quality.getValue(-1.0) << "\n"
    //         << "volume: " << config.volume.getValue(-1.0) << "\n"
    //         << "encoding: " << config.audioencoding.getValue("") << "\n"
    //         << std::endl;
    // }