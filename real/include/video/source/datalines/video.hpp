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
    VideoLine(GstBin *bin, const std::string &encoder, Resolution resolution, int fps, int bitrate);
    //~VideoLine();

    void loadBin(GstBin *bin);
    void unloadBin();

    GstElement *getEncoder() const;
    GstElement *getScale() const;
    GstElement *getRate() const;

    bool makeScreenshot(std::shared_ptr<ScreenshotBranch> &branch);

    static Resolution strToResolution(const std::string &resolution_s, char separator = 'x');

private:
    GstElement* createEncoder();

private:
    GstElement *videoconverter;
    GstElement *videoscale;
    GstElement *videorate;

    GstElement *enctee;
    GstElement *videoencoder;
    GstElement *picencoder;
};