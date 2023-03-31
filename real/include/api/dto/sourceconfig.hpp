#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SourceConfigDto : public oatpp::DTO {
    DTO_INIT(SourceConfigDto, DTO)

    DTO_FIELD(Int32, fps) = 15;
    DTO_FIELD(String, resolution) = "1280x720";
    DTO_FIELD(String, encoding) = "h264";
    DTO_FIELD(Int32, bitrate) = 2000;
    DTO_FIELD(String, cache_mode) = "stream";
    DTO_FIELD(String, archive_path);
};

#include OATPP_CODEGEN_END(DTO)