#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <unordered_map>

#include <video/source/pipebranch.hpp>

class PipeBranch;

class PipeTree {
public:
    PipeTree();
    PipeTree(const std::string &source);
    ~PipeTree();

    bool setSource(const std::string& source);
    bool addBranch(const std::string &name, std::shared_ptr<PipeBranch> branch);
    void removeBranch(const std::string &name);
    GstElement* getSink(const std::string &name);
    
    GstStateChangeReturn setState(GstState state = GST_STATE_PLAYING); 

private:
    std::string uuid;

    GstElement* pipeline;
    GstElement* source;
    GstElement* decoder;
    GstElement* converter;
    GstElement* tee;
    std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;
};