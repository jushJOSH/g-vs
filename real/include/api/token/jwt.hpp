#pragma once

#include <oatpp/web/server/handler/AuthorizationHandler.hpp>
#include <oatpp/core/Types.hpp>

#include <jwt-cpp/jwt.h>

class JwtPayload : public oatpp::web::server::handler::AuthorizationObject {
public:
    oatpp::String userId;
    oatpp::String username; 
    oatpp::String token;
};

class JWT {
public:
    JWT(const oatpp::String& secret, const oatpp::String& issuer);

    oatpp::String createToken(const std::shared_ptr<JwtPayload>& payload);
    std::shared_ptr<JwtPayload> verifyToken(const oatpp::String& token);

private:
    oatpp::String secret;
    oatpp::String issuer;

    jwt::verifier<jwt::default_clock, jwt::traits::kazuho_picojson> verifier;
};