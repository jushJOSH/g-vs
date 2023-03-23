#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include <api/db/dto/media.hpp>
#include <api/db/dto/auth.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

class UserDto : public oatpp::DTO {
    DTO_INIT(UserDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD(String, username);
    DTO_FIELD(String, digest);
    DTO_FIELD(Vector<oatpp::Object<MediaDto>>, user_media);
};

#include OATPP_CODEGEN_END(DTO)