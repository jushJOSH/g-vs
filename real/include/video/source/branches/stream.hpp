#pragma once

#include <video/source/branches/branch.hpp>

#include <utils/hlsconfig.hpp>

#include <condition_variable>
#include <mutex>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch(std::shared_ptr<HLSConfig> config);
    StreamBranch(const std::string &playlistFolder, const std::string &playlistId, int targetDuration, int playlistLenght, int bias = 2);
    ~StreamBranch();

    GstPad* getSinkPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::shared_ptr<HLSConfig> getConfig() const;

    bool loadBin();
    void unloadBin();

private:
    std::shared_ptr<HLSConfig> config;
};