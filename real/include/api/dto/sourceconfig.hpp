#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SourceConfigDto : public oatpp::DTO {
    DTO_INIT(SourceConfigDto, DTO)

    // Source settings
    DTO_FIELD(String, cache_mode) = "stream";
    DTO_FIELD(String, archive_path);
    DTO_FIELD(String, source_url);

    // Videoline settings
    DTO_FIELD(Int32, fps) = 15;
    DTO_FIELD(String, resolution) = "1280x720";
    DTO_FIELD(String, videoencoding) = "264";
    DTO_FIELD(Int32, bitrate) = 2000;

    // Audioline settings
    DTO_FIELD(Boolean, mute) = false;
    DTO_FIELD(Float32, quality) = 0.3;
    DTO_FIELD(Float32, volume) = 0.8;
    DTO_FIELD(String, audioencoding) = "vorbis";
};

#include OATPP_CODEGEN_END(DTO)