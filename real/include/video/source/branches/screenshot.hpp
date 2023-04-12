#pragma once

#include <video/source/branches/branch.hpp>

class ScreenshotBranch : public PipeBranch { 
public:
    ScreenshotBranch(const std::string &path);
    ~ScreenshotBranch();

    GstPad* getNewPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::string getPath() const;

    bool loadBin();
    void unloadBin();

private: 
    std::string path;
};