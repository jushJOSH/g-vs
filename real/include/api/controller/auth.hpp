#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <api/handler/auth.hpp>
#include <api/db/service/users.hpp>
#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class AuthController : public oatpp::web::server::api::ApiController {
// Default constructor
public:
    AuthController(const std::shared_ptr<ObjectMapper>& objectMapper)
    :   oatpp::web::server::api::ApiController(objectMapper),
        jsonMapper(oatpp::parser::json::mapping::ObjectMapper::createShared())
    {
        setDefaultAuthorizationHandler(std::make_shared<AuthHandler>(jwt));
    }

// Static
public: 
    static std::shared_ptr<AuthController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<AuthController>(objectMapper);
    }

private:
    AuthService authService;
    UserService userService;
    OATPP_COMPONENT(std::shared_ptr<JWT>, jwt);
    std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> jsonMapper;

// Endpoints
public:
    ENDPOINT("POST", "/auth", makeToken, QUERY(oatpp::String, login, "login"), QUERY(oatpp::String, password, "password"));
    ENDPOINT("DELETE", "/auth", removeToken, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload));
    ENDPOINT("PUT", "/auth", refreshToken, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload));
};

#include OATPP_CODEGEN_END(ApiController) 