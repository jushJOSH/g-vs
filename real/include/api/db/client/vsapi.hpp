#pragma once

#include <oatpp/orm/SchemaMigration.hpp>
#include <oatpp/orm/DbClient.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <api/db/dto/media.hpp>

#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(DbClient) 

class VsapiClient : public oatpp::orm::DbClient {
public:
    VsapiClient(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
    {}

    QUERY(getMediaByUser,
    "SELECT media.id, title, default_source, owner, media.digest, login, password"
    " FROM media" 
    " LEFT JOIN users ON media.owner = users.id" 
    " LEFT JOIN auth ON users.id = auth.user" 
    " WHERE auth.auth_token=:auth_token;",
    PARAM(oatpp::String, auth_token))

    QUERY(getSourceByMedia,
    "SELECT id, host, config"
    " FROM source" 
    " LEFT JOIN media_source ON id = source_id" 
    " WHERE media_id = :media_id;",
    PARAM(oatpp::Int32, media_id))

    QUERY(changeDefaultSource,
    "UPDATE media" 
    " SET default_source=:source_id"
    " WHERE id=:media_id;",
    PARAM(oatpp::Int32, source_id),
    PARAM(oatpp::Int32, media_id))

    QUERY(addMedia,
    "INSERT INTO media(title, default_source, owner, digest, login, password)"
    " VALUES(:media.title, :media.default_source, :media.owner, :media.digest, :media.login, :media.password)"
    " RETURNING id;",
    PARAM(oatpp::Object<MediaDto>, media))

    QUERY(removeMedia,
    "DELETE FROM media"
    " WHERE id=:id;",
    PARAM(oatpp::Int32, id))

    QUERY(addSource,
    "INSERT INTO source(host, config)"
    " VALUES(:source.host, :source.config)"
    " RETURNING id;",
    PARAM(oatpp::Object<SourceDto>, source))

    QUERY(linkMediaSource,
    "INSERT INTO media_source(media_id, source_id)"
    " VALUES(:media_id, :source_id);",
    PARAM(oatpp::Int32, media_id),
    PARAM(oatpp::Int32, source_id))

    QUERY(removeSource,
    "DELETE FROM source"
    " WHERE id=:id;",
    PARAM(oatpp::Int32, id))

    QUERY(createMediaIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.media"
    "("
        " id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( CYCLE INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        " title text COLLATE pg_catalog.\"default\","
        " default_source integer,"
        " owner integer NOT NULL,"
        " digest text COLLATE pg_catalog.\"default\","
        " login text COLLATE pg_catalog.\"default\","
        " password text COLLATE pg_catalog.\"default\","
        " CONSTRAINT media_pkey PRIMARY KEY (id),"
        " CONSTRAINT media_default_source UNIQUE (default_source),"
        " CONSTRAINT media_digest UNIQUE (digest),"
        " CONSTRAINT f_owner FOREIGN KEY (owner)"
            " REFERENCES public.users (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE CASCADE"
            " NOT VALID,"
        " CONSTRAINT f_source FOREIGN KEY (default_source)"
            " REFERENCES public.source (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE SET NULL"
            " NOT VALID"
    ")")

    QUERY(createSourceIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.source"
    "("
        " id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( CYCLE INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        " host text COLLATE pg_catalog.\"default\" NOT NULL,"
        " config text COLLATE pg_catalog.\"default\" NOT NULL,"
        " CONSTRAINT \"Source_pkey\" PRIMARY KEY (id)"
    ")")

    QUERY(createMedia_SourceIfNotExists,
    "CREATE TABLE IF NOT EXISTS public.media_source"
    "("
        " id integer NOT NULL GENERATED ALWAYS AS IDENTITY ( CYCLE INCREMENT 1 START 0 MINVALUE 0 MAXVALUE 2147483647 CACHE 1 ),"
        " media_id integer NOT NULL,"
        " source_id integer NOT NULL,"
        " CONSTRAINT media_source_pkey PRIMARY KEY (id),"
        " CONSTRAINT u_source_id UNIQUE (source_id),"
        " CONSTRAINT f_media FOREIGN KEY (media_id)"
            " REFERENCES public.media (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE CASCADE"
            " NOT VALID,"
        " CONSTRAINT f_source FOREIGN KEY (source_id)"
            " REFERENCES public.source (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE CASCADE"
            " NOT VALID"
    ")")

    QUERY(modifyMedia,
    "UPDATE media"
    " SET title=:media.title, default_source=:media.default_source, owner=:media.owner, digest=:media.digest, login=:media.login, password=:media.password"
    " WHERE id=:media.id;",
    PARAM(oatpp::Object<MediaDto>, media));

    QUERY(modifySource,
    "UPDATE source"
    " SET host=:source.host, config=:source.config"
    " WHERE id=:source.id;",
    PARAM(oatpp::Object<SourceDto>, source))
};

#include OATPP_CODEGEN_END(ApiController) 