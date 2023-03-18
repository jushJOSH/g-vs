#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include <api/db/dto/source.hpp>

#include OATPP_CODEGEN_BEGIN(DTO)

class MediaDto : public oatpp::DTO {
    DTO_INIT(MediaDto, DTO)

    DTO_FIELD(Int32, id);
    DTO_FIELD(String, title);
    DTO_FIELD(Int32, default_source) = -1;
    DTO_FIELD(Vector<oatpp::Object<SourceDto>>, sources);
    DTO_FIELD(String, digest);
    DTO_FIELD(String, login);
    DTO_FIELD(String, password);
};

#include OATPP_CODEGEN_END(DTO)