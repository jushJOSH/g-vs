#pragma once

#include <video/source/branches/branch.hpp>
#include <boost/circular_buffer.hpp>
#include <video/sample/sample.hpp>

#include <condition_variable>
#include <mutex>

class StreamBranch : public PipeBranch { 
private:
struct BranchReadyArg {
    std::atomic_bool ready = false;
    std::condition_variable* untilBranchReady = nullptr;
    std::mutex *commonBranchMutex = nullptr;
};

public:
    StreamBranch();
    StreamBranch(std::condition_variable* untilBranchReady, std::mutex *commonBranchMutex);
    ~StreamBranch();

    GstPad* getSinkPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::shared_ptr<Sample> getSample();
    bool isReady() const;

    bool loadBin();
    void unloadBin();

private:
    static GstFlowReturn sampleAquired(GstElement* appsink, BranchReadyArg *arg);
    
    BranchReadyArg arg;
};