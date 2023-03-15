#pragma once

#include <gst/gst.h>
#include <vector>
#include <pipeline.hpp>
#include <nlohmann/json.hpp>
#include <crow.h>

#include <vsapirouter.hpp>
#include <authrouter.hpp>
#include <usersrouter.hpp>

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
Videoserver(const std::string &host, int port, bool HTTPnonblocking = false, bool HTTPmultithreaded = true, bool GSTstartLoop = false);
~Videoserver();

/// @brief Start glib main loop
void runMainLoop();

/// @brief Stops glib main loop
void stopMainLoop();

/// @brief Run http server sync mode.
void httpSync();

/// @brief Run http server async mode.
/// @return std::future of running instance
std::future<void> httpAsync();

// ----- GET methods ------

/// @brief Allows to get current HTTP Crow app instance
/// @return Crow app instance
crow::App<AuthMiddleware> &getHTTPApp();

/// @brief Allows to get current GST main loop
/// @return  GMainLoop* GST Main loop
GMainLoop *getGSTMainLoop() const;

/// @brief Allows to get all pipelines
/// @return std::vector Pipelines
std::vector<std::shared_ptr<GPipeline>> getGSTPipelines() const;

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
std::vector<std::shared_ptr<GPipeline>> pipelines;

// --- HTTP server params
std::string host;
int port;
crow::Crow<Videoserver::AuthMiddleware> *httpInstance;

crow::App<AuthMiddleware> app;
std::future<void> instance;

// --- Videoserver routers
AuthRouter<AuthMiddleware> auth; 
VsapiRouter<AuthMiddleware> vsapi; 
UserRouter<AuthMiddleware> user;
};