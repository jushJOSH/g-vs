#include <api/controller/auth.hpp>
#include <api/controller/users.hpp>
#include <api/controller/vsapi.hpp>

#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

#include <oatpp/network/Server.hpp>

#include <api/handler/error.hpp>

#include <iostream>

void run(const oatpp::base::CommandLineArguments& args) {

  AppComponent appComponent(args);
  ServiceComponent serviceComponent;
  DatabaseComponent databaseComponent;

  /* create ApiControllers and add endpoints to router */

  auto router = serviceComponent.httpRouter.getObject();
  router->addController(AuthController::createShared());
  //router->addController(UserController::createShared());
  //router->addController(VsapiController::createShared());

  /* create server */

  oatpp::network::Server server(serviceComponent.serverConnectionProvider.getObject(),
                                serviceComponent.serverConnectionHandler.getObject());

  OATPP_LOGD("Server", "Running on port %s...", serviceComponent.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

  server.run();

}

int main(int argc, const char * argv[]) {

  oatpp::base::Environment::init();

  run(oatpp::base::CommandLineArguments(argc, argv));

  oatpp::base::Environment::destroy();

  return 0;
}