#pragma once

#include <gst/gst.h>
#include <vector>
#include <pipeline.hpp>
#include <nlohmann/json.hpp>
#include <crow.h>

class Videoserver {
// Struct definition
private:
struct AuthMiddleware {
        struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        std::cout << "\nMiddleware test\n";
        // TODO Auth check
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {}
};

public:

/// @brief Videoserver HTTP constructor
/// @param host Hostname to bind
/// @param port Port to bind
/// @param startLoop Defines whenever main loop will be started from the creation of videoserver instance or later.
Videoserver(const std::string &host, int port, bool startLoop = false);
~Videoserver();

/// @brief Start glib main loop
void runMainLoop();

/// @brief Stops glib main loop
void stopMainLoop();

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
std::vector<GPipeline> pipelines;

// --- HTTP server params
std::string host;
int port;

crow::App<AuthMiddleware> app;
std::future<void> instance;

// --- Blueprints functions
crow::Blueprint registerUserBlueprint();
crow::Blueprint registerVsapiBlueprint();
crow::Blueprint registerAuthBlueprint();
};