#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AuthDto : public oatpp::DTO {
    DTO_INIT(AuthDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD(Int32, user);
    DTO_FIELD(String, auth_token);
    DTO_FIELD(String, refresh_token);
    DTO_FIELD(String, end_time);
};

#include OATPP_CODEGEN_END(DTO)