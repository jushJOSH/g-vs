#pragma once

#include <api/dto/config.hpp>
#include <api/handler/error.hpp>

#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp/web/server/HttpRouter.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>
#include "oatpp/web/server/interceptor/AllowCorsGlobal.hpp"
#include "api/interceptor/request.hpp"

#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/component.hpp>

#include <oatpp-openssl/server/ConnectionProvider.hpp>
#include <oatpp-openssl/configurer/TemporaryDhParamsFile.hpp>
#include <oatpp-openssl/Config.hpp>

class ServiceComponent {
public:
    // TODO add to config usage of extended client info
    // Create connection provider on specific host:port
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider)([] {
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config); // Get config component
        return oatpp::network::tcp::server::ConnectionProvider::createShared({config->host, config->port, oatpp::network::Address::IP_4}, true);
    }());
    
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::openssl::server::ConnectionProvider>, secureConnectionProvider)([]()->std::shared_ptr<oatpp::openssl::server::ConnectionProvider> {
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config); // Get config component
        if (config->apiCert.getValue("") == "" || config->apiKey.getValue("") == "")
            return nullptr;
        auto connectionConfig = oatpp::openssl::Config::createDefaultServerConfigShared(config->apiCert, config->apiKey);
        if (config->apiDiffieHellman.getValue("") != "")
            connectionConfig->addContextConfigurer(
                std::make_shared<oatpp::openssl::configurer::TemporaryDhParamsFile>(config->apiDiffieHellman)
            );

        OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, serverConnectionProvider);
        return oatpp::openssl::server::ConnectionProvider::createShared(connectionConfig, serverConnectionProvider);
    }());

    // Create HTTP Router component
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([] {
        return oatpp::web::server::HttpRouter::createShared();
    }());
    
    // TODO add to config usage of interceptors
    // Create ConnectionHandler component which uses Router component to route requests
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, serverConnectionHandler)([] {
        OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router); // get Router component
        OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, objectMapper); // get ObjectMapper component

        auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
        connectionHandler->setErrorHandler(std::make_shared<ErrorHandler>(objectMapper));
        connectionHandler->addRequestInterceptor(std::make_shared<RequestInterceptor>());
        return connectionHandler;
    }());

};