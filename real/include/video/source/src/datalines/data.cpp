#include <video/source/datalines/data.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

DataLine::DataLine(LineType type, const std::string &encoder)
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    encoder_s(encoder),
    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    type(type),
    bin(GST_BIN(gst_bin_new(str(format("%1%_bin") % uuid).c_str())))
{
    g_print("Created dataline %s\n", uuid.c_str());
}

std::string DataLine::getUUID() const {
    return this->uuid;
}

DataLine::LineType DataLine::getType() const {
    return type;
}

GstStateChangeReturn DataLine::setState(GstState state) {
    return gst_element_set_state(queue, state);
}

GstPad* DataLine::generateSinkPad() {
    auto staticPad = gst_element_get_static_pad(queue, "sink");
    auto ghostPad = gst_ghost_pad_new("sink", staticPad);
    
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(staticPad);

    return gst_element_get_static_pad(GST_ELEMENT(bin), "sink");
}

void DataLine::setParent(GstElement* parent) {
    this->parent = parent;
}

GstElement* DataLine::getParent() const {
    return this->parent;
}

GstBin* DataLine::getBin() const {
    return bin;
}

GstPad* DataLine::getNewPad() const {
    GstPad* newPad = gst_element_get_static_pad(queue, "sink");
    if (gst_pad_is_linked(newPad)) return nullptr;

    auto ghostPad = gst_ghost_pad_new(
            str(format("%1%_ghost_filter") % uuid).c_str(),
            newPad
    );
    gst_element_add_pad(queue, ghostPad);

    return ghostPad;
}