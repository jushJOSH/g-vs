#include <video/videoserver/videoserver.hpp>

Videoserver::Videoserver(const std::string &host, int port, bool nonblocking, bool multithreaded, bool startLoop) 
: 
    // HTTP Params
    host(host), port(port),

    // GStreamer params
    mainLoop(g_main_loop_new(nullptr, startLoop))
{}

void Videoserver::runMainLoop() {
    g_main_loop_run(this->mainLoop);
}

void Videoserver::stopMainLoop() {
    g_main_loop_quit(this->mainLoop);
}

GMainLoop *Videoserver::getGSTMainLoop() const {
    return mainLoop;
}

Videoserver::~Videoserver() {
    g_main_loop_quit(mainLoop);
    g_main_loop_unref(mainLoop);
}