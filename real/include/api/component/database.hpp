#pragma once

#include <oatpp-postgresql/orm.hpp>
#include <oatpp/core/macro/component.hpp>
#include <tuple>

#include <api/db/client/auth.hpp>
#include <api/db/client/vsapi.hpp>
#include <api/db/client/users.hpp>
#include <api/dto/config.hpp>

/// @brief Database component to communicate with postgresql
class DatabaseComponent {
public:
    // Make connection provider
    OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connection)([](){
        // Get config component
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

        // Create database-specific ConnectionProvider
        auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(config->dbConnectionString);

        // Create database-specific ConnectionPool
        return oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                            10 /* max-connections */,
                                                                            std::chrono::seconds(5) /* connection TTL */);
    }());

    // Make auth client
    OATPP_CREATE_COMPONENT(std::shared_ptr<AuthClient>, auth_database)([](){
        // Get connection pool component
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connection);

        // Create database-specific Executor
        auto executor = std::make_shared<oatpp::postgresql::Executor>(connection);

        // Making AuthClient
        return std::make_shared<AuthClient>(executor);
    }());

    // Make vsapi client
    OATPP_CREATE_COMPONENT(std::shared_ptr<VsapiClient>, vsapi_database)([](){
        // Get connection pool component
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connection);

        // Create database-specific Executor
        auto executor = std::make_shared<oatpp::postgresql::Executor>(connection);

        // Making VsapiClient
        return std::make_shared<VsapiClient>(executor);
    }());

    // Make user client
    OATPP_CREATE_COMPONENT(std::shared_ptr<UserClient>, user_database)([](){
        // Get connection pool component
        OATPP_COMPONENT(std::shared_ptr<oatpp::postgresql::ConnectionPool>, connection);

        // Create database-specific Executor
        auto executor = std::make_shared<oatpp::postgresql::Executor>(connection);

        // Making UserClient
        return std::make_shared<UserClient>(executor);
    }());

};