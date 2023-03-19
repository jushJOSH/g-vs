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
    /// @brief Struct to hold all Database interfaces
    struct PostgresqlClients {
        std::shared_ptr<AuthClient> IAuth;
        std::shared_ptr<VsapiClient> IVsapi;
        std::shared_ptr<UserClient> IUsers;
    };

    /// @brief Create database interfaces
    OATPP_CREATE_COMPONENT(PostgresqlClients, postgreComponents)([](){
        // Get config component
        OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

        // Create database-specific ConnectionProvider
        auto connectionProvider = std::make_shared<oatpp::postgresql::ConnectionProvider>(config->dbConnectionString);

        // Create database-specific ConnectionPool
        auto connectionPool = oatpp::postgresql::ConnectionPool::createShared(connectionProvider,
                                                                            10 /* max-connections */,
                                                                            std::chrono::seconds(5) /* connection TTL */);

        // Create database-specific Executor
        auto executor = std::make_shared<oatpp::postgresql::Executor>(connectionPool);

        // Making structure object with all table interfaces
        return PostgresqlClients {
            std::make_shared<AuthClient>(executor),
            std::make_shared<VsapiClient>(executor),
            std::make_shared<UserClient>(executor)
        };
    }());
};