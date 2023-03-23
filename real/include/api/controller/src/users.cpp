#include <api/controller/users.hpp>
#include <api/dto/status.hpp>

// Create new user
VSTypes::OatResponse UserController::makeUser(const oatpp::String &userinfo) {
    try {
        auto userObject = jsonMapper->readFromString<oatpp::Object<UserDto>>(userinfo);
        userService.createUser(userObject);
        OATPP_LOGI("UserRouter", "Create new user %s", userObject->username->c_str());
        return createDtoResponse(VSTypes::OatStatus::CODE_200, userObject);
    } catch (const std::exception& e) {
        OATPP_LOGI("UserRouter", "Error creating user");
        OATPP_LOGD("UserRouter", e.what());

        return createResponse(VSTypes::OatStatus::CODE_400, "Error creating selected user");
    }
}

// Modify user
VSTypes::OatResponse UserController::editUser(const oatpp::String &userinfo, const std::shared_ptr<JwtPayload> &payload) {
    try {
        auto userObject = jsonMapper->readFromString<oatpp::Object<UserDto>>(userinfo);
        userService.modifyUser(userObject, payload->username);
        OATPP_LOGI("UserRouter", "Modify user %s -> %s", payload->username->c_str(), userObject->username->c_str());
        
        return createResponse(VSTypes::OatStatus::CODE_200);
    } catch (const std::exception& e) {
        OATPP_LOGI("UserRouter", "Error modifying user");
        OATPP_LOGD("UserRouter", e.what());

        return createResponse(VSTypes::OatStatus::CODE_400, "Error modifying user");
    }
}

// Delete user
VSTypes::OatResponse UserController::deleteUser(const std::shared_ptr<JwtPayload> &payload) {
    try {
        userService.deleteUser(payload->username);
        OATPP_LOGI("UserRouter", "Delete user %s", payload->username->c_str());
        return createResponse(VSTypes::OatStatus::CODE_200);
    } catch (const std::exception& e) {
        OATPP_LOGI("UserRouter", "Error deleting user");
        OATPP_LOGD("UserRouter", e.what());

        return createResponse(VSTypes::OatStatus::CODE_400, "Error deleting user");
    }
}