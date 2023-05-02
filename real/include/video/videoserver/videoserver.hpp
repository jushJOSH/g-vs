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

struct RemoveBranchData {
    std::shared_ptr<Source> targetSource;
    std::unordered_map<std::string, std::shared_ptr<Source>> *allSources;
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
std::shared_ptr<Source> openSource(std::shared_ptr<SourceDto> config);

// Removes branch from source
void removeBranchFromSource(const std::string &source, const std::string &branch);
void removeSource(const std::string &source);

/// @brief Allows to get current GST main loop
/// @return  GMainLoop* GST Main loop
GMainLoop *getMainLoop() const;

static Accelerator accelerator;

private:
    static void onBranchRemoved(void* data);
    static void onSourceStop(GstBus *bus, GstMessage *message, gpointer data);
    static std::string authURI(const std::string &uri, const std::string &login, const std::string &password);

private:
// --- GStreamer params ---
GMainLoop *mainLoop;

// Map of SOURCE(URI) -> Source obj
std::unordered_map<std::string, std::shared_ptr<Source>> aliveSources;
std::thread loopThread;
};