#include <video/source/branches/branch.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <string>

using boost::format;
using boost::str;

void PipeBranch::updateRate(int fps) {
        GstCaps* frame_caps = gst_caps_new_simple("video/x-raw",
        "framerate", G_TYPE_INT, fps, 1,
        NULL
    );
    g_object_set(videorate, "caps", frame_caps, NULL);
}

void PipeBranch::updateResolution(int w, int h) {
        GstCaps* scale_caps = gst_caps_new_simple("video/x-raw",
        "width", G_TYPE_INT, w,
        "height", G_TYPE_INT, h,
        NULL
    );
    g_object_set(videoscale, "caps", scale_caps, NULL);
}

PipeBranch::PipeBranch(const SourceConfigDto &config) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    dtoConfig(config),

    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    videorate(gst_element_factory_make("videorate", str(format("%1%_videorate") % uuid).c_str())),
    videoscale(gst_element_factory_make("videoscale", str(format("%1%_videoscale") % uuid).c_str()))
{
    std::vector<std::string> resolution;
    boost::split(resolution, dtoConfig.resolution->c_str(), boost::is_any_of("x"));

    updateRate(dtoConfig.fps);
    updateResolution(std::stoi(resolution[0]), std::stoi(resolution[1]));
}

PipeBranch::~PipeBranch() {
    unloadBin();
}

bool PipeBranch::loadBin(GstBin* bin) {
    this->bin = bin;

    gst_bin_add_many(bin, queue, videoscale, videorate, videoencoder, sink, NULL);
    return gst_element_link_many(queue, videoscale, videorate, videoencoder, sink, NULL);
}

void PipeBranch::unloadBin() {
    gst_bin_remove_many(bin, queue, videoscale, videorate, videoencoder, sink, NULL);
}

void PipeBranch::setCallback(GCallback callback, gpointer* callbackArg) {
    g_object_set(G_OBJECT(sink), "emit-signals", TRUE, NULL);
    g_signal_connect(sink, "new-sample", callback, callbackArg);
}

GstElement* PipeBranch::getQueue() {
    return queue;
}

GstElement* PipeBranch::getSink() {
    return sink;
}