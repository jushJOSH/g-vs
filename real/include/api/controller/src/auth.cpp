#include <api/controller/auth.hpp>
#include <api/db/dto/user.hpp>
#include <oatpp/core/utils/ConversionUtils.hpp>
#include <openssl/sha.h>

// TODO make digest not password
VSTypes::OatResponse AuthController::makeToken(const oatpp::String &login, const oatpp::String &password) {
    using namespace oatpp::utils::conversion;

    // Making user dto with query params
    auto userDto = UserDto::createShared();
    userDto->username = login; userDto->digest = password;

    userDto = userService.getUserByCreds(userDto);
    if (userDto->id == -1) {
        OATPP_LOGI("AuthRouter", "Bad auth attempt");
        return createResponse(VSTypes::OatStatus::CODE_401);
    }
    auto authDto = AuthDto::createShared();
    auto payload = std::make_shared<JwtPayload>();
    payload->userId = int32ToStr(userDto->id);
    payload->username = userDto->username;
    
    authDto->user = userDto->id;
    authDto->auth_token = jwt->createToken(payload);
    authService.authorize(authDto);

    return createDtoResponse(VSTypes::OatStatus::CODE_200, authDto);
}

VSTypes::OatResponse AuthController::refreshToken(const std::shared_ptr<JwtPayload> &payload) {
    using namespace oatpp::utils::conversion;
    
    bool success;
    oatpp::Int32 userId = strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, VSTypes::OatStatus::CODE_401, "Malformed user token");
    
    auto newToken = jwt->createToken(payload);
    authService.refreshToken(newToken, userId);
    
    auto authDto = AuthDto::createShared();
    authDto->id = userId;
    authDto->auth_token = newToken;

    return createDtoResponse(VSTypes::OatStatus::CODE_200, authDto);
}

VSTypes::OatResponse AuthController::removeToken(const std::shared_ptr<JwtPayload> &payload) {
    authService.deauthorize(payload->token);
    return createResponse(VSTypes::OatStatus::CODE_200);
}