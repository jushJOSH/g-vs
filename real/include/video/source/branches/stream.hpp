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
    ~StreamBranch();

    GstPad* getNewPad(DataLine::LineType type);
    GstElement *getFirstElement() const;
    std::shared_ptr<Sample> getSample();
    void waitSample() const;

    bool loadBin();
    void unloadBin();

    void setCallback(GCallback callback);

public:
    static GstFlowReturn onNewSample(GstElement* appsink, CallbackArg *data);

private:
    std::shared_ptr<CallbackArg> arg;
};