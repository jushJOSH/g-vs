#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include <api/db/dto/user.hpp>
#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class UserClient : public oatpp::orm::DbClient {
public:
    UserClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(createUser,
    "INSERT INTO users (username, digest)" 
    " VALUES (:user.username, :user.digest)"
    " RETURNING id;",
    PARAM(oatpp::Object<UserDto>, user))

    QUERY(deleteUser,
    "DELETE FROM users" 
    " WHERE username=:username;",
    PARAM(oatpp::String, username))

    QUERY(modifyUser,
    "UPDATE users" 
    " SET username=:user.username, digest=:user.digest"
    " WHERE username=:oldusername;",
    PARAM(oatpp::Object<UserDto>, user),
    PARAM(oatpp::String, oldusername))

    QUERY(getUserByCreds,
    "SELECT * FROM users"
    " WHERE username=:user.username AND digest=:user.digest;",
    PARAM(oatpp::Object<UserDto>, user))

    QUERY(getUserById,
    "SELECT login FROM users"
    " WHERE id=:id",
    PARAM(oatpp::Int32, id))

    QUERY(makeAdminIfNotExists,
    "INSERT INTO users (username, digest)" 
    " VALUES ('admin', 'admin')" 
    " ON CONFLICT DO NOTHING;")

    QUERY(createTableIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.users"
    "("
        " id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( CYCLE INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        " username text COLLATE pg_catalog.\"default\" NOT NULL,"
        " digest text COLLATE pg_catalog.\"default\" NOT NULL,"
        " CONSTRAINT users_pkey PRIMARY KEY (id),"
        " CONSTRAINT u_username UNIQUE (username)"
    ")")

    QUERY(getCurrentUser,
    "SELECT id, username" 
    " FROM users" 
    " LEFT JOIN auth ON users.id = auth.user" 
    " WHERE auth_token=:auth_token;",
    PARAM(oatpp::String, auth_token))
};

#include OATPP_CODEGEN_END(ApiController) 