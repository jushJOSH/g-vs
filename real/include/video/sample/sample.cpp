#include <video/sample/sample.hpp>

#include <utility>

Sample::Sample(GstSample* sample) 
:   sample(std::move(sample))
{}

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
    this->sample = std::move(sample);
}