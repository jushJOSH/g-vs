#include <api/db/service/users.hpp>
#include <misc.hpp>

void UserService::createUser(const oatpp::Object<UserDto>& dto) {
    auto result = user_database->createUser(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    dto->id = VSMisc::fetchId<UserDto>(result);
}

void UserService::deleteUser(const oatpp::String& username) {
    auto result = user_database->deleteUser(username);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void UserService::modifyUser(const oatpp::Object<UserDto>& dto, const oatpp::String &oldusername) {
    auto result = user_database->modifyUser(dto, oldusername);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

oatpp::Object<UserDto> UserService::getUserByCreds(const oatpp::Object<UserDto>& dto) {
    auto result = user_database->getUserByCreds(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    dto->id = VSMisc::fetchId<UserDto>(result);
    return dto;
}

oatpp::Object<UserDto> UserService::getUserById(const oatpp::Int32& id) {
    auto result = user_database->getUserById(id);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    auto receivedData = result->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    OATPP_ASSERT_HTTP(receivedData->size() == 1, VSTypes::OatStatus::CODE_500, "Unable to find user"); 
    return receivedData[0];
}

oatpp::Object<UserDto> UserService::getCurrentUser(const oatpp::Object<AuthDto>& dto) {
    // Searching user id by received auth token
    auto authResult = auth_database->getUserByAuth(dto->auth_token);
    OATPP_ASSERT_HTTP(authResult->isSuccess(), VSTypes::OatStatus::CODE_500, authResult->getErrorMessage());
    
    // Saving user_id for use
    auto receivedUserId = authResult->fetch<oatpp::Vector<oatpp::Object<AuthDto>>>();
    OATPP_ASSERT_HTTP(receivedUserId->size() == 1, VSTypes::OatStatus::CODE_500, "Unable to find user"); 
    auto user_id = receivedUserId[0]->user;

    // Searching user by its id (login only, no digest)
    auto userResult = user_database->getUserById(user_id);
    OATPP_ASSERT_HTTP(userResult->isSuccess(), VSTypes::OatStatus::CODE_500, userResult->getErrorMessage());

    // Retreiving user data
    auto receivedUserData = userResult->fetch<oatpp::Vector<oatpp::Object<UserDto>>>();
    OATPP_ASSERT_HTTP(receivedUserData->size() == 1, VSTypes::OatStatus::CODE_500, "Unable to find user"); 
    
    // This object to return
    auto retv_user = receivedUserData[0];

    // Searching media of this user
    auto mediaResult = vsapi_database->getMediaByUser(dto->auth_token);
    OATPP_ASSERT_HTTP(mediaResult->isSuccess(), VSTypes::OatStatus::CODE_500, mediaResult->getErrorMessage());

    // Fetching media array
    auto receivedMediaData = userResult->fetch<oatpp::Vector<oatpp::Object<MediaDto>>>();
    OATPP_ASSERT_HTTP(receivedMediaData->size() == 1, VSTypes::OatStatus::CODE_500, "Unable to find media"); 
    retv_user->user_media = receivedMediaData;

    // Fetching sources for all media
    for (auto &media : *retv_user->user_media) {
        auto sourceResult = vsapi_database->getSourceByMedia(media->id);
        OATPP_ASSERT_HTTP(sourceResult->isSuccess(), VSTypes::OatStatus::CODE_500, sourceResult->getErrorMessage());
    
        media->sources = userResult->fetch<oatpp::Vector<oatpp::Object<SourceDto>>>();
    }

    return retv_user;
}

void UserService::initUserTable() {
    void createTableIfNotExists();  // Creates users table
    void makeAdminIfNotExists();    // Create user 'admin'
}

void UserService::createTableIfNotExists() {
    auto result = user_database->createTableIfNotExists();
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void UserService::makeAdminIfNotExists() {
    auto result = user_database->makeAdminIfNotExists();
}