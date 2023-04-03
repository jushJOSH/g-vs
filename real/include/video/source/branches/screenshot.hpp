#pragma once

#include <video/source/branches/branch.hpp>

class ScreenshotBranch : public PipeBranch { 
public:
    ScreenshotBranch(const SourceConfigDto& config);
    //~ScreenshotBranch();

    bool loadBin(GstBin *bin) override;
    void unloadBin() override;

protected:
    void initPadEvent() override;    
};