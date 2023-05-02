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
    "SELECT media.*"
    " FROM media" 
    " LEFT JOIN auth ON media.owner = auth.user" 
    " WHERE auth.auth_token=:auth_token;",
    PARAM(oatpp::String, auth_token))

    QUERY(isMediaBelongsToUser,
    "SELECT media.id"
    " FROM media"
    " WHERE media.owner = :owner_id",
    PARAM(oatpp::Int32, owner_id))

    QUERY(isSourceBelongsToUser,
    "SELECT source.id"
    " FROM source"
    " LEFT JOIN media ON media.id = source.media"
    " WHERE media.owner = :owner_id",
    PARAM(oatpp::Int32, owner_id))

    QUERY(isMediaHasSource,
    "SELECT source.*"
    " FROM source"
    " WHERE source.media = :media_id AND source.id = :source_id;",
    PARAM(oatpp::Int32, media_id),
    PARAM(oatpp::Int32, source_id))

    QUERY(getSourceByMedia,
    "SELECT source.*"
    " FROM source" 
    " WHERE source.media = :media_id;",
    PARAM(oatpp::Int32, media_id))

    QUERY(getSourceById,
    "SELECT source.*"
    " FROM source"
    " WHERE id = :source_id;",
    PARAM(oatpp::Int32, source_id))

    QUERY(getMediaById,
    "SELECT media.*"
    " FROM media"
    " WHERE id = :media_id",
    PARAM(oatpp::Int32, media_id))

    QUERY(changeDefaultSource,
    "UPDATE media" 
    " SET default_source=:source_id"
    " WHERE id=:media_id;",
    PARAM(oatpp::Int32, source_id),
    PARAM(oatpp::Int32, media_id))

    QUERY(addMedia,
    "INSERT INTO media(title, default_source, owner)"
    " VALUES(:media.title, :media.default_source, :media.owner)"
    " RETURNING id;",
    PARAM(oatpp::Object<MediaDto>, media))

    QUERY(removeMedia,
    "DELETE FROM media"
    " WHERE id=:id;",
    PARAM(oatpp::Int32, id))

    QUERY(addSource,
    "INSERT INTO source(source_url, login, password, fps, videoencoding, bitrate, mute, volume, media)"
    " VALUES(:source.source_url, :source.login, :source.password, :source.fps, :source.videoencoding, :source.bitrate, :source.mute, :source.volume, :source.media)"
    " RETURNING id;",
    PARAM(oatpp::Object<SourceDto>, source))

    QUERY(linkMediaSource,
    "UPDATE source"
    " SET media=:media_id"
    " WHERE id=:source_id;",
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
        " CONSTRAINT media_pkey PRIMARY KEY (id),"
        " CONSTRAINT media_default_source UNIQUE (default_source),"
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
        " source_url text COLLATE pg_catalog.\"default\" NOT NULL,"
        " fps integer NOT NULL,"
        " videoencoding text COLLATE pg_catalog.\"default\" NOT NULL,"
        " bitrate integer NOT NULL,"
        " mute boolean NOT NULL,"
        " volume double precision NOT NULL,"
        " login text COLLATE pg_catalog.\"default\","
        " password text COLLATE pg_catalog.\"default\","
        " media integer NOT NULL,"
        " CONSTRAINT \"Source_pkey\" PRIMARY KEY (id),"
        " CONSTRAINT source_media_fkey FOREIGN KEY (media)"
            " REFERENCES public.media (id) MATCH SIMPLE"
            " ON UPDATE NO ACTION"
            " ON DELETE CASCADE"
            " NOT VALID"
    ")")

    QUERY(modifyMedia,
    "UPDATE media"
    " SET title=:media.title, default_source=:media.default_source"
    " WHERE id=:media.id;",
    PARAM(oatpp::Object<MediaDto>, media));

    QUERY(modifySource,
    "UPDATE source"
    " SET source_url=:source.source_url, login=:source.login, password=:source.password, fps=:source.fps, videoencoding=:source.videoencoding, bitrate=:source.bitrate, mute=:source.mute, volume=:source.volume"
    " WHERE id=:source.id;",
    PARAM(oatpp::Object<SourceDto>, source))
};

#include OATPP_CODEGEN_END(ApiController) 