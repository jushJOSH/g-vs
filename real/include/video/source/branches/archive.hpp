#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const std::string &path);
    //~ArchiveBranch();

    bool loadBin(GstBin *bin);
    void unloadBin();
};