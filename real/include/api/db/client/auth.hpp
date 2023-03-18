#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include <types.hpp>
#include <api/db/dto/auth.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class AuthClient : public oatpp::orm::DbClient {
public:
    AuthClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(authorize,
    "INSERT INTO auth(user, auth_token, refresh_token, time)"
    " VALUES(:auth.user, :auth.auth_token, :auth.refresh_token, :auth.end_time);",
    PARAM(oatpp::Object<AuthDto>, auth))

    QUERY(deauthorize,
    "DELETE FROM auth"
    " WHERE auth_token=:auth_token;",
    PARAM(oatpp::String, auth_token))

    QUERY(refreshToken,
    "UPDATE auth"
    " SET auth_token=:auth.auth_token, refresh_token=:auth.refresh_token, time=:auth.end_time"
    " WHERE refresh_token=:old_refresh_token;",
    PARAM(oatpp::Object<AuthDto>, auth),
    PARAM(oatpp::String, old_refresh_token))

    QUERY(getUserByAuth,
    "SELECT * FROM auth"
    " WHERE auth_token=:auth.auth_token;",
    PARAM(oatpp::Object<AuthDto>, auth))

    QUERY(createTableIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.auth"
    "("
        " id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( CYCLE INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        " \"user\" integer NOT NULL,"
        " auth_token text COLLATE pg_catalog.\"default\" NOT NULL,"
        " refresh_token text COLLATE pg_catalog.\"default\" NOT NULL,"
        " \"time\" timestamp without time zone NOT NULL,"
        " CONSTRAINT auth_pkey PRIMARY KEY (id),"
        " CONSTRAINT f_user FOREIGN KEY (\"user\")"
            " REFERENCES public.users (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE CASCADE"
            " NOT VALID"
    ")")
};

#include OATPP_CODEGEN_END(ApiController) 