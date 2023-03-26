#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <api/multipart/mpstreamer.hpp>
#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

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

// Endpoint callbacks
protected:
//    VSTypes::OatResponse makeToken();
//    VSTypes::OatResponse refreshToken();

// Endpoints
public:
    ENDPOINT("GET", "/live", getLive);
};

#include OATPP_CODEGEN_END(ApiController) 