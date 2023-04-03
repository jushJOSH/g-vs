#pragma once

#include <video/source/datalines/dataline.hpp>

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
    struct Resolution
    {
        int width;
        int height;
    };

public:
    VideoLine(const std::string &encoder, Resolution resolution, int fps, int bitrate);
    VideoLine(GstBin *bin, const std::string &encoder, Resolution resolution, int fps, int bitrate);
    //~VideoLine();

    void loadBin(GstBin *bin);
    void unloadBin();

    GstElement *getEncoder() const;
    GstElement *getScale() const;
    GstElement *getRate() const;

    static Resolution strToResolution(const std::string &resolution_s, char separator = 'x');

private:
    GstElement* createEncoder();

private:
    std::string uuid;
    std::string encoder_s;

    GstBin *bin;
    GstElement *queue;
    GstElement *videoconverter;
    GstElement *videoscale;
    GstElement *videorate;
    GstElement *videoencoder;
};