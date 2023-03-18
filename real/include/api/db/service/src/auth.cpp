#include <api/db/service/auth.hpp>
#include <types.hpp>

void AuthService::authorize(const oatpp::Object<AuthDto>& dto) {
    auto result = m_database->authorize(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void AuthService::deauthorize(const oatpp::Object<AuthDto>& dto) {
    auto result = m_database->deauthorize(dto->auth_token);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void AuthService::refreshToken(const oatpp::String& oldRefreshToken, const oatpp::Object<AuthDto>& newDto) {
    auto result = m_database->refreshToken(newDto, oldRefreshToken);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

oatpp::Int32 AuthService::getUserByAuth(const oatpp::Object<AuthDto>& dto) {
    auto result = m_database->getUserByAuth(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void AuthService::initAuthTable() {
    auto result = m_database->createTableIfNotExists();
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}