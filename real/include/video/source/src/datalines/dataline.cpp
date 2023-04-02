#include <video/source/datalines/dataline.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <algorithm/string.hpp>

using boost::format;
using boost::str;

DataLine::DataLine(LineType type, const std::string &encoder)
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    encoder_s(encoder),
    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    type(type)
{
    g_print("Created dataline %s\n", uuid.c_str());
}

DataLine::DataLine(
    GstBin* bin,
    LineType type, 
    const std::string &encoder)
:   DataLine(type, encoder)
{
    loadBin(bin);
}

bool DataLine::attachToPipeline(GstElement* before) {
    return 
        gst_element_link(before, this->queue);
}

bool DataLine::attachToPipeline(GstPad* before) {
    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return 
        gst_pad_link(before, queueSink);
}

void DataLine::detachFromPipeline(GstElement* before) {
    gst_element_unlink(before, this->queue);
}

bool DataLine::detachFromPipeline(GstPad* before) {
    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return 
        gst_pad_unlink(before, queueSink);
}

GstElement* DataLine::getQueue() const {
    return this->queue;
}

std::string DataLine::getUUID() const {
    return this->uuid;
}