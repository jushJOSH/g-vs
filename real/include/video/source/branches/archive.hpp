#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const SourceConfigDto& config);
    ~ArchiveBranch();

    bool loadBin(GstBin* bin) override;
    void unloadBin() override;

// Got confused with inheritance. Mb fixable with it but idc
    void updateEncoder(int bitrate);
protected:
    GstElement* createEncoder(int accelerator);

private:
    GstElement* muxer;
};