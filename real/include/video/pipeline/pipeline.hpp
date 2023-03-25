#pragma once

#include <string>
#include <gst/gst.h>

#include <video/bus/bus.hpp>
#include <video/brick/brick.hpp>

class GPipeline {
public:
    /// @brief Pipeline constructor from str
    /// @param pipeName Pipe name
    GPipeline(const std::string&);

    /// @brief Pipeline constructor from GstElement*
    /// @param  
    GPipeline(GstElement*);
    ~GPipeline();

    // ------ GET ------

    /// @brief Auto conversion from GPipeline to GstElement AKA pipeline
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
    
    /// @brief Get bus object
    /// @return bus
    GBus &getBus();

    // ------ Update ------

    /// @brief Create bus for this pipeline
    /// @return Created bus
    GBus reloadBus();

    /// @brief Update self state
    /// @param State to set this state
    /// @return Result of operation. 
    GstStateChangeReturn updateState(GstState);

    /// @brief Adds element to pipeline
    /// @param Element to add
    /// @return true if element was successfully added
    bool addToPipeline(GstElement*);

    /// @brief Adds multiple elements to pipeline
    /// @param Elements
    /// @return Failed element to add. nullptr if success.
    GstElement* addToPipelineMany(const std::initializer_list<GstElement*>&);

    /// @brief Adds multiple elements to pipeline
    /// @param Elements
    /// @return Failed element to add. nullptr if success.
    GstElement* addToPipelineMany(const std::initializer_list<GBrick>&);


    /// @brief Removes element from pipeline
    /// @param  Element to remove
    /// @return true on success, else false
    bool removeFromPipeline(GstElement*);

    /// @brief Removes multiple elements
    /// @param  Elements to remove
    /// @return failed element. nullptr on success
    GstElement* removeFromPipelineMany(const std::initializer_list<GstElement*>&);

    /// @brief Set pipeline clock
    /// @param  Clock to set
    void setPipelineClock(GstClock*);

    /// @brief Restores default pipeline clock
    void restorePipelineClock();

private:
    GstElement* self;
    GBus pipelineBus;
};