#pragma once

#include <gst/gst.h>

// Dummies
class GPipeline;

class GBus {
public:
    /// @brief Bus constructor. Uses GPipeline object to make one
    /// @param pipeline Pipeline itself
    GBus(GPipeline &pipeline);

    /// @brief Bus constructor. Uses passed reference as bus
    /// @param bus Created bus itself
    GBus(GstBus* bus);
    ~GBus();
    
    // ----- Get ------
    operator GstBus*() const {
        return self;
    }

private:
    GstBus* self;
};