#pragma once

#include <gst/gst.h>
#include <unordered_map>
#include <video/source/source.hpp>
#include <nlohmann/json.hpp>

#include <api/controller/vsapi.hpp>
#include <api/controller/auth.hpp>
#include <api/controller/users.hpp>

class Videoserver {
public:
// Hardware accelerator
enum class Accelerator {
    AMD, NVIDIA, CPU
};

public:
/// @brief Videoserver HTTP constructor
/// @param host Hostname to bind
/// @param port Port to bind
/// @param startLoop Defines whenever main loop will be started from the creation of videoserver instance or later.
Videoserver(const std::string &host, int port, bool HTTPnonblocking = false, bool HTTPmultithreaded = true, bool GSTstartLoop = false);
~Videoserver();

/// @brief Start glib main loop
void runMainLoop();

/// @brief Stops glib main loop
void stopMainLoop();

std::shared_ptr<Source> openSource(const std::string& source);

/// @brief Allows to get current GST main loop
/// @return  GMainLoop* GST Main loop
GMainLoop *getGSTMainLoop() const;


// ----- JSON manipulation -----

/// @brief Deserialize object (nlohmann)
/// @param object JSON object
void from_json(const nlohmann::json& object);

/// @brief Serialize object (nlohmann)
/// @param object JSON object
void to_json(nlohmann::json& object) const;

static Accelerator accelerator;

private:
// --- GStreamer params ---
GMainLoop *mainLoop;
std::unordered_map<std::string, std::shared_ptr<Source>> aliveSources;

// --- HTTP server params
std::string host;
int port;
};