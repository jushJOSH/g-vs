#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/parser/json/mapping/ObjectMapper.hpp>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>

#include <api/db/service/users.hpp>
#include <api/handler/auth.hpp>
#include <types.hpp>

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class UserController : public oatpp::web::server::api::ApiController {
// Default constructor
public:
    UserController(const std::shared_ptr<ObjectMapper>& objectMapper)
    :oatpp::web::server::api::ApiController(objectMapper)
    {
        jsonMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
        setDefaultAuthorizationHandler(std::make_shared<AuthHandler>(jwt));
    }

// Static
public: 
    static std::shared_ptr<UserController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
        return std::make_shared<UserController>(objectMapper);
    }

// TODO : make authorization
// Endpoints
public:
    ENDPOINT("POST", "/user/new", makeUser, BODY_STRING(oatpp::String, userinfo));
    ENDPOINT("PUT", "/user/edit", editUser, BODY_STRING(oatpp::String, userinfo), AUTHORIZATION(std::shared_ptr<JwtPayload>, payload));
    ENDPOINT("DELETE", "/user/delete", deleteUser, AUTHORIZATION(std::shared_ptr<JwtPayload>, payload));

// Users Client
private:
    UserService userService;
    std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper> jsonMapper;
    OATPP_COMPONENT(std::shared_ptr<JWT>, jwt);
};

#include OATPP_CODEGEN_END(ApiController) 