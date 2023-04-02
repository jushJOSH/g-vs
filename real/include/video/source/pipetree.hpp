#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <unordered_map>

#include <video/source/branches/branch.hpp>
#include <video/source/branches/archive.hpp>
#include <video/source/branches/stream.hpp>
#include <video/source/branches/screenshot.hpp>

class PipeBranch;

class PipeTree {
public:
    PipeTree();
    PipeTree(const std::string &source);
    ~PipeTree();

    bool setSource(const std::string& source);
    GstPadLinkReturn addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch);
    void removeBranch(const std::string &name);
    GstElement* getSink(const std::string &name);
    
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 

private:
    static GstPadProbeReturn padProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);
    static GstPadProbeReturn eventProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data);

    std::string uuid;

    GstElement* pipeline;
    GstElement* source;
    GstElement* tee;

    // Map of 'name' of branch and GstPad of tee for it
    std::unordered_map<std::string, GstPad*> teePads;

    // Map of 'name' of branch and branch itself
    std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;
};