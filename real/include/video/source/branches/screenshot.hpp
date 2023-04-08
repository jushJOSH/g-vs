#pragma once

#include <video/source/branches/branch.hpp>

class ScreenshotBranch : public PipeBranch { 
public:
    ScreenshotBranch(const std::string &path);
    ScreenshotBranch(GstBin* bin, const std::string &path);
    //~ScreenshotBranch();

    GstPad* getNewPad(DataLine::LineType type);
    std::string getPath() const;

    bool loadBin(GstBin *bin);
    void unloadBin();

private: 
    std::string path;
};