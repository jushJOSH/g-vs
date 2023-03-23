#include <api/db/service/auth.hpp>
#include <types.hpp>
#include <misc.hpp>

void AuthService::authorize(const oatpp::Object<AuthDto>& dto) {
    auto result = auth_database->authorize(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void AuthService::deauthorize(const oatpp::String& token) {
    auto result = auth_database->deauthorize(token);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void AuthService::refreshToken(const oatpp::String& newAuthToken, const oatpp::Int32& userid) {
    auto result = auth_database->refreshToken(newAuthToken, userid);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

oatpp::Int32 AuthService::getUserByAuth(const oatpp::String& dto) {
    auto result = auth_database->getUserByAuth(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    auto receivedData = result->fetch<oatpp::Vector<oatpp::Object<AuthDto>>>();
    if (receivedData->empty()) return -1;
    else return receivedData[0]->user;
}

void AuthService::initAuthTable() {
    auto result = auth_database->createTableIfNotExists();
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}