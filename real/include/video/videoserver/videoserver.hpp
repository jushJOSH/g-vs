#pragma once

#include <gst/gst.h>
#include <vector>
#include <video/pipeline/pipeline.hpp>
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

// /// @brief Run http server sync mode.
// void httpSync();

// /// @brief Run http server async mode.
// /// @return std::future of running instance
// std::future<void> httpAsync();

// // ----- GET methods ------

// /// @brief Allows to get current HTTP Crow app instance
// /// @return Crow app instance
// crow::App<AuthMiddleware> &getHTTPApp();

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

private:
// --- GStreamer params ---
GMainLoop *mainLoop;

// --- HTTP server params
std::string host;
int port;
};