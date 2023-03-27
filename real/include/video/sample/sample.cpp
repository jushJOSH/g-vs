#include <video/sample/sample.hpp>

#include <utility>

Sample::Sample(GstSample* sample) 
{
    setSample(sample);
}

Sample::~Sample() {
    gst_sample_unref(sample);
}

Sample::operator GstSample*() {
    return sample;
}

Sample::operator GstSample*() const {
    return sample;
}

void Sample::setSample(GstSample* sample) {
    this->sample = gst_sample_copy(sample);
    gst_sample_unref(sample);
}