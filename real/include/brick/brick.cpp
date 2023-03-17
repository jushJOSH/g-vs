#include <brick/brick.hpp>

GBrick::GBrick(const std::string &type, const std::string& name)
: self(gst_element_factory_make(type.c_str(), name.c_str()))
{}

GBrick::GBrick(GstElement* element)
: self(element)
{}