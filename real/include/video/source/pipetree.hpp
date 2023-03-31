#pragma once

#include <gst/gst.h>
#include <gst/app/app.h>

#include <video/source/pipebranch.hpp>

class PipeTree {
public:
    PipeTree();
    ~PipeTree();

    bool setSource(const std::string& source);
    bool addBranch(const std::string &name, PipeBranch& branch);
    void removeBranch(const std::string &name);
    GstElement* getSink(const std::string &name);
    
    void setPlay();

private:
    std::string uuid;

    GstElement* pipeline;
    GstElement* source;
    GstElement* decoder;
    GstElement* converter;
    GstElement* tee;
    std::unordered_map<std::string, std::shared_ptr<PipeBranch>> branches;
};