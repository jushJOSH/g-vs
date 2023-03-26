#pragma once

#include <gst/app/app.h>

class Sample {
public:
    Sample(GstSample* sample);
    Sample() = default;
    
    void setSample(GstSample* sample);

    ~Sample();
    operator GstSample*();
    operator GstSample*() const;

private:
    GstSample *sample;
};