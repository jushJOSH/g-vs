#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class AuthController : public oatpp::web::server::api::ApiController {
// Default constructor
public:
    AuthController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
    {}

// Static
public: 
    static std::shared_ptr<AuthController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<AuthController>(objectMapper);
    }

// Endpoints
public:
    ENDPOINT("POST", "/auth/new", makeToken);
    ENDPOINT("POST", "/auth/refresh", refreshToken);
};

#include OATPP_CODEGEN_END(ApiController) 