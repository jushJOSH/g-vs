#include <api/controller/auth.hpp>

VSTypes::OatResponse AuthController::makeToken() {
    return createResponse(VSTypes::OatStatus::CODE_200, "makeToken()");
}

VSTypes::OatResponse AuthController::refreshToken() {
    return createResponse(VSTypes::OatStatus::CODE_200, "refreshToken()");
}