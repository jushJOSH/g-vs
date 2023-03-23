#include <api/handler/auth.hpp>

AuthHandler::AuthHandler(const std::shared_ptr<JWT>& jwt)
:   oatpp::web::server::handler::BearerAuthorizationHandler("vs-api"),
    jwt(jwt)
{}

std::shared_ptr<AuthHandler::AuthorizationObject> AuthHandler::authorize(const oatpp::String& token) {
    // verify token. If corrupted, return nullptr
    auto payload = jwt->verifyToken(token);
    if (payload != nullptr) payload->token = token;

    // If user authorized with received token return payload of token
    auto userid = authService.getUserByAuth(token);
    
    // If token is present but not verified remove from db
    if (userid != -1 && payload == nullptr)
        authService.deauthorize(token);

    return (userid != -1 ? payload : nullptr);
}