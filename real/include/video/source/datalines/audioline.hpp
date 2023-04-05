#pragma once

#include <video/source/datalines/dataline.hpp>

#include <gst/gst.h>
#include <gst/app/app.h>
#include <string>

// Setting to change
// - Quality
// - Volume
class AudioLine : public DataLine
{
public:
    AudioLine(const std::string& encoder, double quality, double volume);
    AudioLine(GstBin *bin, const std::string& encoder, double quality, double volume);

    void loadBin(GstBin *bin);
    void unloadBin();

    GstElement *getEncoder() const;
    GstElement *getVolume() const;

private:
    GstElement* createEncoder();

private:
    GstElement *audioconverter;
    GstElement *volume;
    GstElement *audioencoder;
};