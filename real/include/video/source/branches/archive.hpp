#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const SourceConfigDto& config);
    ~ArchiveBranch();

    bool loadBin(GstBin* bin);
    void unloadBin();

    void updateEncoder(int bitrate);
    void updateResolution(const std::string resolution);
    void updateFrameRate(int fps);
    void updateAudioQuality(double quality);
    void updateAudioVolume(double volume);
    void mute(bool state);

    GstStateChangeReturn  setState(GstState state = GST_STATE_PLAYING);

private:
    GstElement* muxer;
    GstElement* sink;
};