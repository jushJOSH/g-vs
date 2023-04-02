#pragma once

#include <video/source/branches/branch.hpp>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch(const SourceConfigDto &config);
    ~StreamBranch();

    void updateEncoder(int bitrate);
    void updateFps(int fps);
    void updateResolution();

protected:
    GstElement* createEncoder(int accelerator);
};