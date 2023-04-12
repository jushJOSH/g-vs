#include <api/controller/auth.hpp>
#include <api/controller/users.hpp>
#include <api/controller/vsapi.hpp>
#include <api/handler/error.hpp>
#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

#include <oatpp/network/Server.hpp>

#include <thread>

void runApi() {
    std::thread serverThread([]{
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

void runVideo() {
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    OATPP_LOGD("Video", "Video loop is running");
    videoserver->runMainLoop();
    videoserver->stopMainLoop();
}

int main(int argc, char **argv) {
    gst_init(&argc, &argv);

    oatpp::base::Environment::init();

    // Init app component
    auto parsedArgs = oatpp::base::CommandLineArguments(argc, (const char**)argv);
    AppComponent appComponent(parsedArgs);

    // Run api and video parts
    runApi();
    runVideo();

    oatpp::base::Environment::destroy();

    return 0;
}