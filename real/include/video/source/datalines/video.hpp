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
    VideoLine(const std::string &encoder, int fps, int bitrate);
    ~VideoLine();

    void updateBitrate(int bitrate);
    void updateFramerate(int fps);
    
    static Resolution strToResolution(const std::string &resolution_s, char separator = 'x');

private:
    void generateSrcPad() const;
    GstElement* createEncoder();

private:
    GstElement *videoconverter;
    GstElement *videoscale;
    GstElement *videorate;
};