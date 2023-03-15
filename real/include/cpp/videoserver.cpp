#include <videoserver.hpp>

Videoserver::Videoserver(const std::string &host, int port, bool nonblocking, bool multithreaded, bool startLoop) 
: 
    // HTTP Params
    host(host), port(port),

    // Router params
    // Set just default paths
    auth("auth"), vsapi(this->pipelines, "vsapi"), user("user"),

    // GStreamer params
    mainLoop(g_main_loop_new(nullptr, startLoop))
{
    // Register all routers
    auth.registerRouter(app);
    vsapi.registerRouter(app);
    user.registerRouter(app);

    // Building HTTP server instance
    app.bindaddr(host).port(port);

    if (multithreaded) app.multithreaded();
}

std::future<void> Videoserver::httpAsync() {
    return app.run_async();
}

void Videoserver::httpSync() {
    app.run();
}

void Videoserver::runMainLoop() {
    g_main_loop_run(this->mainLoop);
}

void Videoserver::stopMainLoop() {
    g_main_loop_quit(this->mainLoop);
}

crow::App<Videoserver::AuthMiddleware> &Videoserver::getHTTPApp() {
    return app;
}

GMainLoop *Videoserver::getGSTMainLoop() const {
    return mainLoop;
}

std::vector<std::shared_ptr<GPipeline>> Videoserver::getGSTPipelines() const {
    return pipelines;
}

Videoserver::~Videoserver() {
    g_main_loop_quit(mainLoop);
    g_main_loop_unref(mainLoop);

    delete httpInstance;
}