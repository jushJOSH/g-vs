#pragma once

#include <video/source/branches/branch.hpp>

class ScreenshotBranch : public PipeBranch { 
public:
    ScreenshotBranch(const std::string &screenPath);
    //~ScreenshotBranch();

    GstPad* getNewPad(DataLine::LineType type);

    static void makeSnapshot(GstBuffer* buffer, GstMapInfo info);
    bool loadBin(GstBin *bin);
    void unloadBin();

private:
    static void onNewSample(GstElement* src, gpointer arg);
    
    std::string path;
};