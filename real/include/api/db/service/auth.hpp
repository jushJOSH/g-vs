#pragma once

#include <api/db/client/auth.hpp>
#include <api/db/dto/auth.hpp>
#include <types.hpp>

#include <oatpp/web/protocol/http/Http.hpp>
#include <oatpp/core/macro/component.hpp>

/*  
    QUERY(authorize,
    QUERY(deauthorize,
    QUERY(refreshToken,
    QUERY(createTableIfNotExists,
*/

class AuthService {
// Auth client handlers
public:
    void authorize(const oatpp::Object<AuthDto>& dto);
    void deauthorize(const oatpp::String& token);
    void refreshToken(const oatpp::String& newAuthToken, const oatpp::Int32& userid);
    oatpp::Int32 getUserByAuth(const oatpp::String& token);

    void initAuthTable();

private:
    // Injecting database component
    OATPP_COMPONENT(std::shared_ptr<AuthClient>, auth_database);
};