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
    tee(gst_element_factory_make("tee", str(format("%1%_tee") % uuid).c_str())),
    type(type)
{
    g_print("Created dataline %s\n", uuid.c_str());
}

std::string DataLine::getUUID() const {
    return this->uuid;
}

DataLine::LineType DataLine::getType() const {
    return type;
}

GstPad* DataLine::generateNewPad() {
    auto newPad = gst_element_get_request_pad(tee, "src_%u");
    return newPad;
}

GstElement* DataLine::getTee() const {
    return this->tee;
}