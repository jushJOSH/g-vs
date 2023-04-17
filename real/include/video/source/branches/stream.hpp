#pragma once

#include <video/source/branches/branch.hpp>
#include <boost/circular_buffer.hpp>
#include <video/sample/sample.hpp>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch();
    ~StreamBranch();

    GstPad* getSinkPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::shared_ptr<Sample> getSample();
    bool isReady() const;

    bool loadBin();
    void unloadBin();

private:
    static GstFlowReturn sampleAquired(GstElement* appsink, std::atomic_bool *isReady);
    
    std::atomic_bool ready = false;
};