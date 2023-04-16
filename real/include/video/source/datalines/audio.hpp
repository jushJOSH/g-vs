#pragma once

#include <video/source/datalines/data.hpp>

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
    ~AudioLine();
    
    void updateVolume(double volume);
    void updateQuality(double quality);
    void mute(bool mute);

private:
    void generateSrcPad() const; 
    GstElement* createEncoder();

private:
    GstElement *audioconverter;
    GstElement *volume;
};