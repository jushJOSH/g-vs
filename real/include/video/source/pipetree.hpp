#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <unordered_map>
#include <queue>
#include <vector>

#include <video/source/branches/branch.hpp>
#include <video/source/datalines/data.hpp>
#include <video/source/branches/archive.hpp>
#include <video/source/branches/screenshot.hpp>
#include <video/source/branches/stream.hpp>

class PipeBranch;

class PipeTree {
private:
    struct PadInfo {
        GstBin* bin;
        GstState currentState;

        SourceConfigDto config;
        
        std::queue<std::shared_ptr<PipeBranch>> branchQueue;
        std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;
        std::vector<std::pair<std::string, GstElement*>> createdPads;
        bool noMorePads = false;
    };

    struct RemoveBranch {
        int currIdx;
        std::shared_ptr<PipeBranch> branch;
        std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;     
        GstBin* bin;
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
    std::vector<std::shared_ptr<DataLine>> &getDatalines();

private:
    static GstPadProbeReturn padProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);
    static GstPadProbeReturn eventProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);
    static void onNewPad(GstElement *src, GstPad *newPad, PadInfo* data);
    static void onErrorCallback(GstBus *bus, GstMessage *msg, gpointer data);
    static int manageBranchQueue(PadInfo& data);
    static void onNoMorePads(GstElement* src, PadInfo* data);
    static std::shared_ptr<DataLine> createDataline(const std::pair<std::string, GstPad*> &pad, const PadInfo &userData);

private:
    // Map of 'name' of branch and branch itself
    std::string uuid;
    PadInfo padinfo;
    
    // Uri decode bin
    GstElement* source;
    GstElement* tee;
    GstElement* pipeline;
};