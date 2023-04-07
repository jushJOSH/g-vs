#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const std::string &path);
    //~ArchiveBranch();

    GstPad* getNewPad(DataLine::LineType type);

    bool loadBin(GstBin *bin);
    void unloadBin();

private:
    std::string path;
};