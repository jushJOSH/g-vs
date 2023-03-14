#include <videoserver.hpp>

Videoserver::Videoserver(const std::string &host, int port, bool startLoop) 
: 
    // HTTP Params
    host(host), port(port),

    // GStreamer params
    mainLoop(g_main_loop_new(nullptr, startLoop))
{
    // Loading blueprints
    auto blueprints = {
        registerAuthBlueprint(),
        registerUserBlueprint(),
        registerVsapiBlueprint()
    };
    for (auto blueprint : blueprints)
        app.register_blueprint(blueprint);
        
    // Running multithreaded async HTTP server
    //this->instance = app.bindaddr(host).port(port).multithreaded().run_async();
    app.bindaddr(host).port(port).multithreaded().run();
}

void Videoserver::runMainLoop() {
    g_main_loop_run(this->mainLoop);
}

void Videoserver::stopMainLoop() {
    g_main_loop_quit(this->mainLoop);
}

Videoserver::~Videoserver() {
    g_main_loop_quit(mainLoop);
    g_main_loop_unref(mainLoop);
}