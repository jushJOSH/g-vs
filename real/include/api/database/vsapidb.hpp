#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class VsapiClient : public oatpp::orm::DbClient {
public:
    VsapiClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(getMediaByUser,
    "SELECT * FROM media LEFT JOIN user ON media.owner = user.id LEFT JOIN auth ON user.id = auth.user WHERE auth.auth_token=:auth_token;")

    QUERY(getSourceByMedia,
    "")

    QUERY(changeDefaultSource,
    "")

    QUERY(addMedia,
    "")

    QUERY(removeMedia,
    "")

    QUERY(addSource,
    "")

    QUERY(removeSource,
    "")

    QUERY(createMediaIfNotExists,
    "")

    QUERY(createSourceIfNotExists,
    "")

    QUERY(createMedia_SourceIfNotExists,
    "")

    QUERY(modifyMedia,
    "")

    QUERY(modifySource,
    "")
};

#include OATPP_CODEGEN_END(ApiController) 