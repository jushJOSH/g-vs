#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SourceConfigDto : public oatpp::DTO {
    DTO_INIT(SourceConfigDto, DTO)

    DTO_FIELD(Int32, fps);
    DTO_FIELD(String, resolution);
    DTO_FIELD(String, encoding);
    DTO_FIELD(Int32, bitrate);
    DTO_FIELD(String, cache_mode);
    DTO_FIELD(String, archive_path);
};

#include OATPP_CODEGEN_END(DTO)