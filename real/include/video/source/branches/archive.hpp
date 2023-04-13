#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const std::string &path);
    ~ArchiveBranch();

    GstPad* getSinkPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::string getPath() const;

    bool loadBin();
    void unloadBin();

private:
    std::string path;
};