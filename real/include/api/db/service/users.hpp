#pragma once

#include <api/db/client/auth.hpp>
#include <api/db/client/users.hpp>
#include <api/db/client/vsapi.hpp>
#include <api/db/dto/user.hpp>
#include <types.hpp>

#include <oatpp/web/protocol/http/Http.hpp>
#include <oatpp/core/macro/component.hpp>

/*  
    QUERY(createUser,
    QUERY(deleteUser,
    QUERY(modifyUser,
    QUERY(makeAdminIfNotExists,
    QUERY(createTableIfNotExists,
    QUERY(getCurrentUser,
*/

class UserService {
// User client handlers
public:
    void createUser(const oatpp::Object<UserDto>& dto);
    void deleteUser(const oatpp::String& username);
    void modifyUser(const oatpp::Object<UserDto>& dto, const oatpp::String& olduser);
    
    oatpp::Object<UserDto> getUserByCreds(const oatpp::Object<UserDto>& dto);
    oatpp::Object<UserDto> getUserById(const oatpp::Int32& id);
    oatpp::Object<UserDto> getCurrentUser(const oatpp::Object<AuthDto>& dto);
    
    void initUserTable();

private:
    void createTableIfNotExists();
    void makeAdminIfNotExists();

    // Injecting database component
    OATPP_COMPONENT(std::shared_ptr<UserClient>, user_database);
    OATPP_COMPONENT(std::shared_ptr<AuthClient>, auth_database);
    OATPP_COMPONENT(std::shared_ptr<VsapiClient>, vsapi_database);
};