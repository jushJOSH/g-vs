#pragma once

#include <api/token/jwt.hpp>
#include <api/db/service/auth.hpp>

#include <oatpp/core/macro/component.hpp>

class AuthHandler : public oatpp::web::server::handler::BearerAuthorizationHandler {
public:
    AuthHandler(const std::shared_ptr<JWT>& jwt);
    std::shared_ptr<AuthorizationObject> authorize(const oatpp::String& token) override;

private:
    std::shared_ptr<JWT> jwt;

    AuthService authService;
};