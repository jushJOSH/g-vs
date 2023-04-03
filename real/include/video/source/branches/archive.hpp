#pragma once

#include <video/source/branches/branch.hpp>

class ArchiveBranch : public PipeBranch {
public:
    ArchiveBranch(const SourceConfigDto& config);
    //~ArchiveBranch();

protected:
    void initPadEvent() override;    
};