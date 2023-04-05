#pragma once

#include <video/source/branches/branch.hpp>

class ScreenshotBranch : public PipeBranch { 
public:
    ScreenshotBranch(const std::string &screenPath);
    //~ScreenshotBranch();

    bool loadBin(GstBin *bin);
    void unloadBin();
};