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
    AudioLine(GstBin *bin, const std::string& encoder, double quality, double volume);

    bool attachToPipeline(GstElement *before);
    GstPadLinkReturn attachToPipeline(GstPad *before);
    void detachFromPipeline(GstElement *before);
    bool detachFromPipeline(GstPad *before);

    void loadBin(GstBin *bin);
    void unloadBin();

    GstPad* generateSrcPad(); 
    GstPad* generateSinkPad();

    GstElement *getFirstElement() const;
    GstElement *getLastElement() const;
    GstElement *getEncoder() const;
    GstElement *getVolume() const;

private:
    GstElement* createEncoder();

private:
    GstElement *audioconverter;
    GstElement *volume;
    GstElement *audioencoder;
};