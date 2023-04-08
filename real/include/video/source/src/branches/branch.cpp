#include <video/source/branches/branch.hpp>

#include <video/source/datalines/audio.hpp>
#include <video/source/datalines/video.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;


PipeBranch::PipeBranch(const std::string &sink, const std::string &muxer) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    muxer(gst_element_factory_make(muxer.c_str(), str(format("%1%_%2%") % uuid % muxer).c_str())),
    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_%2%") % uuid % sink).c_str()))
{   
    g_print("Branch created %s\n", uuid.c_str());
}

PipeBranch::PipeBranch(GstBin* bin, const std::string &sink, const std::string &muxer) 
:   PipeBranch(sink, muxer)
{
    if (!loadBin(bin))
        throw std::runtime_error("PipeBranch: failed to link queue and decodebin");
}

GstElement* PipeBranch::getSink() const {
    return this->sink;
}

void PipeBranch::setMuxer(GstElement* muxer) {
    this->muxer = muxer;
}

void PipeBranch::setSink(GstElement* sink) {
    this->sink = sink;
}

bool PipeBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(bin, sink, NULL);
    if (this->muxer) gst_bin_add(bin, muxer);
    
    return 
        this->muxer ? gst_element_link(muxer, sink) : true;
}

void PipeBranch::unloadBin() {
    gst_bin_remove_many(bin, sink, NULL);
    if (this->muxer) gst_bin_remove(bin, muxer);
    bin = nullptr;
}

GstBin*  PipeBranch::getBin() const {
    return bin;
}

GstStateChangeReturn PipeBranch::setState(GstState state) {
    return gst_element_set_state(muxer, state);
}

std::string PipeBranch::getUUID() const {
    return uuid;
}