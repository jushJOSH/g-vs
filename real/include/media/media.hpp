#pragma once

#include <vector>
#include <source/source.hpp>
#include <nlohmann/json.hpp>
#include <gst/gst.h>

class MediaUnit {
public:
    MediaUnit();
    MediaUnit(int defaultSource, const std::initializer_list<Source> &sources);

    /// @brief Deserialize object (nlohmann)
    /// @param object JSON object
    void from_json(const nlohmann::json& object);

    /// @brief Serialize object (nlohmann)
    /// @param object JSON object
    void to_json(nlohmann::json& object) const;\

    /// @brief Adds new source to media. If it is first source in media unit, sets to default
    /// @param newSource new source
    /// @return true on success. Else false
    bool addSource(const Source& newSource);

    /// @brief Removes source from media.
    /// @param index index to remove
    /// @return True on success
    bool removeSource(int index);

    /// @brief Changes default source
    /// @param index index of source in array
    /// @return true on success
    bool changeDefault(int index);

    /// @brief Indexer for sources
    /// @param index index
    /// @return rvalue Source
    Source& operator[](int index);

    // ----- Operations with default source

    /// @brief Creates snapshot from source. Use indexer + makeSnap to specify source
    /// @return PATH to snapshot
    std::string makeSnap() const;

    /// @brief Creates live stream from source. Use indexer + makeLive to specify source
    /// @return GstAppSink* element
    GstElement* makeLive() const;

    

private:
    std::vector<Source> mediaSources;
    Source *defaultSource = nullptr;
};