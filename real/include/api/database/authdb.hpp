#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class AuthClient : public oatpp::orm::DbClient {
public:
    AuthClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(authorize,
    "")

    QUERY(deauthorize,
    "")

    QUERY(refreshToken,
    "")

    QUERY(createTableIfNotExists,
    "")
};

#include OATPP_CODEGEN_END(ApiController) 