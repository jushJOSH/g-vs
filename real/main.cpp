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
        std::shared_ptr<oatpp::network::Server> server;
        if (serviceComponent.secureConnectionProvider.getObject() == nullptr) {
            server = std::make_shared<oatpp::network::Server>(
                serviceComponent.serverConnectionProvider.getObject(),
                serviceComponent.serverConnectionHandler.getObject()
            );
            OATPP_LOGI("API ROOT", "Server running insecure!");
        } else {
            server = std::make_shared<oatpp::network::Server>(
                serviceComponent.secureConnectionProvider.getObject(),
                serviceComponent.serverConnectionHandler.getObject()
            );
            OATPP_LOGI("API ROOT", "Server running secure!");
        }

        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);
        OATPP_LOGI("API ROOT", "API is running on %s:%d...", 
            config->host->c_str(),
            *(config->port)
        );
        server->run();
    });

    serverThread.detach();
}

void runVideo() {
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    OATPP_LOGD("VIDEO API", "VIDEO is running...");
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
