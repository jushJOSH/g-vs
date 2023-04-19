#pragma once

#include <video/source/branches/branch.hpp>
#include <boost/circular_buffer.hpp>

#include <condition_variable>
#include <mutex>

class StreamBranch : public PipeBranch { 
public:
    StreamBranch(const std::string &playlistFolder, const std::string &playlistId);
    ~StreamBranch();

    GstPad* getSinkPad(DataLine::LineType type);
    GstElement *getFirstElement() const;

    bool loadBin();
    void unloadBin();

private:
    std::string createdFolder;
};