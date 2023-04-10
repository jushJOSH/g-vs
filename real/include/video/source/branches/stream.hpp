#pragma once

#include <video/source/branches/branch.hpp>
#include <boost/circular_buffer.hpp>
#include <video/sample/sample.hpp>

class StreamBranch : public PipeBranch { 
public:
    struct CallbackArg {
        std::mutex mutex;
        std::shared_ptr<boost::circular_buffer<std::shared_ptr<Sample>>> samples;
    };

    StreamBranch();
    StreamBranch(GstBin* bin);
    //~StreamBranch();

    GstPad* getNewPad(DataLine::LineType type);
    std::shared_ptr<Sample> getSample();
    void waitSample() const;

    bool loadBin(GstBin *bin);
    void unloadBin();

    void setCallback(GCallback callback);

public:
    static GstFlowReturn onNewSample(GstElement* appsink, CallbackArg *data);

private:
    std::shared_ptr<CallbackArg> arg;
};