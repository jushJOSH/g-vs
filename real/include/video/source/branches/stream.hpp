#pragma once

#include <video/source/branches/branch.hpp>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch();
    //~StreamBranch();

    bool loadBin(GstBin *bin);
    void unloadBin();

    void setCallback(GCallback callback, gpointer *callbackArg); 
};