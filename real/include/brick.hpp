#pragma once

#include <string>
#include <gst/gst.h>

class GBrick {
public:
    /// @brief Creates brick element for pipeline. Like sink or src or filter etc...
    /// @param type Type of element
    /// @param name Name for element
    GBrick(const std::string &type, const std::string &name);

    /// @brief Creates brick element for pipeline. Uses passed reference
    /// @param element Reference element
    GBrick(GstElement* element);

    operator GstElement*() const {
        return self;
    }

    /// @brief Logical not operator
    /// @return NOT pipe
    bool operator!() const {
        return !self;
    }

    /// @brief Logical operator
    /// @return pipe
    operator bool() const {
        return self;
    }

private:
    GstElement* self;
};