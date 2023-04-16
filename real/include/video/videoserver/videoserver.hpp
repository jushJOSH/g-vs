#pragma once

#include <gst/gst.h>
#include <unordered_map>
#include <video/source/source.hpp>
#include <thread>

#include <api/controller/vsapi.hpp>
#include <api/controller/auth.hpp>
#include <api/controller/users.hpp>

#include <oatpp/core/base/CommandLineArguments.hpp>

class Videoserver {
public:
// Hardware accelerator
enum class Accelerator {
    AMD, NVIDIA, CPU
};

public:
/// @brief Videoserver HTTP constructor
/// @param accelerator Which hardware accelerator to use
Videoserver(Accelerator accelerator = Accelerator::CPU);
~Videoserver();

/// @brief Start glib main loop
void runMainLoop();

/// @brief Stops glib main loop
void stopMainLoop();

// Creates source if not exist
// Gets created source if exists
std::shared_ptr<Source> openSource(const std::string& source);

// Removes branch from source
void removeBranchFromSource(const std::string &source, const std::string &branch);

/// @brief Allows to get current GST main loop
/// @return  GMainLoop* GST Main loop
GMainLoop *getMainLoop() const;

static Accelerator accelerator;

private:
// --- GStreamer params ---
GMainLoop *mainLoop;
std::unordered_map<std::string, std::shared_ptr<Source>> aliveSources;
std::thread loopThread;
};