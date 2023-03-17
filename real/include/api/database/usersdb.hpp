#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include <oatpp-postgresql/orm.hpp>

#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class UserClient : public oatpp::orm::DbClient {
public:
    UserClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(createUser,
    "INSERT INTO users (username, digest) VALUES (:username, :digest);",
    PARAM(oatpp::String, username),
    PARAM(oatpp::String, digest))

    QUERY(deleteUser,
    "DELETE FROM users WHERE id=:username;",
    PARAM(oatpp::String, username))

    QUERY(modifyUser,
    "UPDATE users SET username=:username, digest=:digest;",
    PARAM(oatpp::String, username),
    PARAM(oatpp::String, digest))

    QUERY(makeAdminIfNotExists,
    "INSERT INTO users (username, digest) VALUES ('admin', 'admin') ON CONFLICT DO NOTHING;")

    QUERY(createTableIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.users"
    "("
        "id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        "username text COLLATE pg_catalog.\"default\" NOT NULL,"
        "digest text COLLATE pg_catalog.\"default\" NOT NULL,"
        "CONSTRAINT users_pkey PRIMARY KEY (id),"
        "CONSTRAINT u_username UNIQUE (username)"
    ")")

    QUERY(getCurrentUser,
    "SELECT username FROM users LEFT JOIN auth ON users.id = auth.user WHERE auth_token=:auth_token;",
    PARAM(oatpp::String, auth_token))
};

#include OATPP_CODEGEN_END(ApiController) 