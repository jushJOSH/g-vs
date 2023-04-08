#include <video/videoserver/videoserver.hpp>

Videoserver::Accelerator Videoserver::accelerator = Videoserver::Accelerator::CPU;

Videoserver::Videoserver(const std::string &host, int port) 
: 
    // HTTP Params
    host(host), port(port),

    // GStreamer params
    mainLoop(g_main_loop_new(nullptr, false))
{}

void Videoserver::runMainLoop() {
    loopThread = std::thread(g_main_loop_run, this->mainLoop);
    loopThread.detach();
}

void Videoserver::stopMainLoop() {
    g_main_loop_quit(this->mainLoop);
}

GMainLoop *Videoserver::getMainLoop() const {
    return mainLoop;
}

Videoserver::~Videoserver() {
    g_main_loop_quit(mainLoop);
    g_main_loop_unref(mainLoop);
}

std::shared_ptr<Source> Videoserver::openSource(const std::string& source) {
    if (!aliveSources.contains(source)) 
        aliveSources[source] = std::make_shared<Source>(source);
        
    return aliveSources.at(source);
}

void Videoserver::runServer() {
    std::thread serverThread([args]{
        AppComponent appComponent(args);
        ServiceComponent serviceComponent;
        DatabaseComponent databaseComponent;

        /* create ApiControllers and add endpoints to router */
        auto router = serviceComponent.httpRouter.getObject();
        router->addController(AuthController::createShared());
        router->addController(UserController::createShared());
        router->addController(VsapiController::createShared());

        /* create server */

        oatpp::network::Server server(serviceComponent.serverConnectionProvider.getObject(),
                                        serviceComponent.serverConnectionHandler.getObject());

        OATPP_LOGD("Server", "Running on port %s...", serviceComponent.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

        server.run();
    });

    serverThread.detach();
}

void Videoserver::run() {
    runServer();
    runMainLoop();
}