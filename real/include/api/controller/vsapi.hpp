#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <memory>
#include <unordered_map>

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

private:
    VSTypes::OatResponse getStaticFileResponse(std::shared_ptr<LiveHandler> handler, const oatpp::String& filepath, const oatpp::String& rangeHeader) const;
    VSTypes::OatResponse getFullFileResponse(const oatpp::String& file) const;
    VSTypes::OatResponse getRangeResponse(const oatpp::String& rangeStr, const oatpp::String& file) const;

// Endpoints
public:
    ENDPOINT("GET", "/vsapi/live", getLive, QUERY(oatpp::String, source, "source"));
    ENDPOINT("GET", "/vsapi/static/{uuid}/*",  getStatic,
             REQUEST(std::shared_ptr<IncomingRequest>, request),
             PATH(oatpp::String, uuid));

private:
    // Map of SOURCE(URI) -> LiveHandler
    std::unordered_map<std::string, std::shared_ptr<LiveHandler>> liveStreams;
};

#include OATPP_CODEGEN_END(ApiController) 