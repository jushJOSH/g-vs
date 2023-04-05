// #include <video/source/source.hpp>

// int main(int argc, char *argv[]) {
//     gst_init(&argc, &argv);
//     auto loop = g_main_loop_new(NULL, false);   

//     SourceConfigDto config;
//     config.archive_path = "/home/egor/";

//     Source testSource("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream");
//     auto screenshot = std::make_shared<ScreenshotBranch>("/home/egor/test.png");
//     auto archive = std::make_shared<ArchiveBranch>("/home/egor/test.mp4");
//     auto stream = std::make_shared<StreamBranch>();

//     auto changeStateResult = testSource.setState();
//     testSource.addBranch("screenshot", screenshot);
//     testSource.addBranch("archive", archive);
//     testSource.addBranch("stream", stream);

//     if (changeStateResult == GST_STATE_CHANGE_FAILURE) {
//         g_printerr ("Unable to set the pipeline to the playing state.\n");
//         return -1;
//     }
// }


#include <api/controller/auth.hpp>
#include <api/controller/users.hpp>
#include <api/controller/vsapi.hpp>
#include <api/handler/error.hpp>
#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

#include <oatpp/network/Server.hpp>

#include <thread>

void run(const oatpp::base::CommandLineArguments& args) {
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

int main(int argc, char **argv) {
    // TEMP
    /* Создание и запуск главного цикла GStreamer */
    /* Initialize GStreamer */
    gst_init(&argc, &argv);
    //g_main_loop_run(loop);

    oatpp::base::Environment::init();
    run(oatpp::base::CommandLineArguments(argc, (const char**)argv));
    auto loop = g_main_loop_new(NULL, false);
    g_main_loop_run(loop);
    oatpp::base::Environment::destroy();

    return 0;
}