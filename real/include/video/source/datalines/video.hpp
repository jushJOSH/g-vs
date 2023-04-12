#pragma once

#include <video/source/datalines/data.hpp>
#include <video/source/branches/screenshot.hpp>

#include <gst/gst.h>
#include <gst/app/app.h>
#include <string>

// Setting to change
// - Resolution
// - FPS
// - Bitrate
class VideoLine : public DataLine
{
public:
    VideoLine(const std::string &encoder, Resolution resolution, int fps, int bitrate);
    ~VideoLine();

    bool loadBin();
    void unloadBin();

    bool attachToPipeline(GstElement *before);
    GstPadLinkReturn attachToPipeline(GstPad *before);
    void detachFromPipeline(GstElement *before);
    bool detachFromPipeline(GstPad *before);
    
    GstPad* generateSrcPad();
    GstPad* generateSinkPad();

    GstElement *getFirstElement() const;
    GstElement *getLastElement() const;
    GstElement *getEncoder() const;
    GstElement *getScale() const;
    GstElement *getRate() const;
    
    static Resolution strToResolution(const std::string &resolution_s, char separator = 'x');

private:
    GstElement* createEncoder();

private:
    GstElement *videoconverter;
    GstElement *videoscale;
    GstElement *videorate;
    GstElement *videoencoder;
};