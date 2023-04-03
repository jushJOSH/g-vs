#pragma once

#include <video/source/branches/branch.hpp>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch(const SourceConfigDto &config);
    //~StreamBranch();

    void setCallback(GCallback callback, gpointer *callbackArg);

protected:
    void initPadEvent() override;    
};