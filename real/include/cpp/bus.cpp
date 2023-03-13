#include "pipeline.hpp"
#include "bus.hpp"

GBus::GBus(GPipeline &pipe)
: self(gst_element_get_bus(pipe))
{}

GBus::GBus(GstBus* bus)
: self(bus)
{}

GBus::~GBus() {
    gst_object_unref(self);
    g_print("Bus deleted\n");
}