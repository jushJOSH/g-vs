#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <api/db/service/vsapi.hpp>
#include <api/handler/auth.hpp>
#include <api/token/jwt.hpp>

#include <memory>
#include <unordered_map>

#include <gst/gst.h>

#include <types.hpp>

class LiveHandler;

#include OATPP_CODEGEN_BEGIN(ApiController)

class VsapiController : public oatpp::web::server::api::ApiController {
private:
    struct HandlerRemoveBundle {
        std::shared_ptr<LiveHandler> target;
        guint timer = -1;
        int unreadyAttempt = 0;
        int maxAttempt = 3;

        std::unordered_map<std::string, std::shared_ptr<LiveHandler>> *liveStreams;
        std::unordered_map<std::string, std::shared_ptr<LiveHandler>> *liveStreams_UUID;
    };

// Default constructor
public:
    VsapiController(const std::shared_ptr<ObjectMapper>& objectMapper)
    :   oatpp::web::server::api::ApiController(objectMapper),
        jsonMapper(oatpp::parser::json::mapping::ObjectMapper::createShared())
    {
        setDefaultAuthorizationHandler(std::make_shared<AuthHandler>(jwt));
    }

// Static
public: 
    static std::shared_ptr<VsapiController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<VsapiController>(objectMapper);
    }

    static void onBranchStop(HandlerRemoveBundle* handlerToDelete);
    static bool timeoutCheckUsage(gpointer data);

// Endpoints
public:
    // GET ALL USERS MEDIAS WITH SOURCES
    ENDPOINT("GET", "/vsapi/media", getMedia, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload));
    // GET SELECTED MEDIA FOR USER
    ENDPOINT("GET", "/vsapi/media/{media}", getSpecialMedia, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
                                                             PATH(oatpp::Int32, media));
    // CREATE MEDIA ENDPOINT (allowed with sources)
    ENDPOINT("POST", "/vsapi/media", createMedia, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
                                                  BODY_STRING(oatpp::String, mediainfo));
    // MODIFY MEDIA ENDPOINT (allowed with sources)
    ENDPOINT("PUT", "/vsapi/media/{media}", modifyMedia, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
                                                         PATH(oatpp::Int32, media),
                                                         BODY_STRING(oatpp::String, mediainfo));
    // DELETE MEDIA ENDPOINT (with sources!)
    ENDPOINT("DELETE", "/vsapi/media/{media}", deleteMedia, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
                                                            PATH(oatpp::Int32, media));
    // ENDPOINT("PUT", "/vsapi/media/{media}/source/{source}", modifySource, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
    //                                                                       PATH(oatpp::Int32, source),
    //                                                                       PATH(oatpp::Int32, media),
    //                                                                       BODY_STRING(oatpp::String, sourceinfo));
    // // DELETE SOURCE ENDPOINT (single source)
    // ENDPOINT("DELETE", "/vsapi/media/{media}/source/{source}", deleteSource, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
    //                                                                          PATH(oatpp::Int32, source),
    //                                                                          PATH(oatpp::Int32, media));
    // // GET FRAME FOR REQUESTED SOURCE OR MEDIA
    // ENDPOINT("GET", "/vsapi/frame", getFrame, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload),
    //                                           QUERIES(QueryParams, QueryParams));
    // GET LIVE FOR SELECTED SOURCE OR MEDIA
    ADD_CORS(getLive)
    ENDPOINT("GET", "/vsapi/live", getLive, QUERY(oatpp::Int32, source));

    ADD_CORS(getStatic)
    ENDPOINT("GET", "/vsapi/static/{source}/*",  getStatic,
             REQUEST(std::shared_ptr<IncomingRequest>, request),
             PATH(oatpp::Int32, source));

// Api stuff
private:
    std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> jsonMapper;
    OATPP_COMPONENT(std::shared_ptr<JWT>, jwt);
    VsapiService vsapiService;

// Video stuff
private:
    // Map of SOURCE(URI) -> LiveHandler
    std::unordered_map<std::string, std::shared_ptr<LiveHandler>> liveStreams;

    // Map of SOURCE(UUID) -> LiveHandler
    std::unordered_map<std::string, std::shared_ptr<LiveHandler>> liveStreams_UUID;

    // Running archives (where int - sourceid, pair.first = source.uri, pair.second = branch.uuid)
    std::unordered_map<int, std::pair<std::string, std::string>> runningArchives;
};

#include OATPP_CODEGEN_END(ApiController) 