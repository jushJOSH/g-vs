#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <unordered_map>
#include <queue>

#include <video/source/branches/branch.hpp>
#include <video/source/datalines/dataline.hpp>
#include <video/source/branches/archive.hpp>
#include <video/source/branches/stream.hpp>
#include <video/source/branches/screenshot.hpp>

class PipeBranch;

class PipeTree {
private:
    struct PadInfo {
        GstBin* bin;
        SourceConfigDto config;
        std::vector<std::shared_ptr<DataLine>> datalines;
        std::queue<std::shared_ptr<PipeBranch>> branchQueue;
    };

public:
    PipeTree();
    PipeTree(const std::string &source);
    PipeTree(const SourceConfigDto& config, const std::string &source);
    ~PipeTree();

    void setSource(const std::string& source);
    void setBin(GstBin* bin);
    void removeBranch(const std::string &name);
    void addBranch(std::shared_ptr<PipeBranch> branch);
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 
    void setConfig(SourceConfigDto& config);

    GstElement* getSink(const std::string &name);

    void updateBitrate(int bitrate);
    void updateResolution(const std::string resolution);
    void updateFrameRate(int fps);
    void updateAudioQuality(double quality);
    void updateAudioVolume(double volume);
    void mute(bool state);

private:
    static GstPadProbeReturn padProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);
    static GstPadProbeReturn eventProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);
    static void onNewPad(GstElement *src, GstPad *newPad, PadInfo* data);
    static void onErrorCallback(GstBus *bus, GstMessage *msg, gpointer data);
    static GstPadLinkReturn manageBranchQueue(const std::queue<std::shared_ptr<PipeBranch>>& branchQueue);

private:
    // Map of 'name' of branch and branch itself
    std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;
    std::string uuid;
    PadInfo padinfo;
    
    // Uri decode bin
    GstElement* source;
    GstElement* pipeline;
    GstState currentState;
};