#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <memory>
#include <unordered_map>

#include <gst/gst.h>

#include <types.hpp>

class LiveHandler;

#include OATPP_CODEGEN_BEGIN(ApiController)

class VsapiController : public oatpp::web::server::api::ApiController {
// Default constructor
public:
    VsapiController(const std::shared_ptr<ObjectMapper>& objectMapper)
    :oatpp::web::server::api::ApiController(objectMapper)
    {}

// Static
public: 
    static std::shared_ptr<VsapiController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<VsapiController>(objectMapper);
    }

    static bool onSourceStop(GstBus *bus, GstMessage *message, gpointer data);
    static bool timeoutCheckUsage(gpointer data);

// Endpoints
public:
    ENDPOINT("GET", "/vsapi/live", getLive, QUERY(oatpp::String, source, "source"));
    ENDPOINT("GET", "/vsapi/static/{uuid}/*",  getStatic,
             REQUEST(std::shared_ptr<IncomingRequest>, request),
             PATH(oatpp::String, uuid));

private:
    struct HandlerRemoveBundle {
        std::shared_ptr<LiveHandler> target;
        guint timer;

        std::unordered_map<std::string, std::shared_ptr<LiveHandler>> *liveStreams;
        std::unordered_map<std::string, std::shared_ptr<LiveHandler>> *liveStreams_UUID;
    };

    // Map of SOURCE(URI) -> LiveHandler
    std::unordered_map<std::string, std::shared_ptr<LiveHandler>> liveStreams;

    // Map of SOURCE(UUID) -> LiveHandler
    std::unordered_map<std::string, std::shared_ptr<LiveHandler>> liveStreams_UUID;
};

#include OATPP_CODEGEN_END(ApiController) 