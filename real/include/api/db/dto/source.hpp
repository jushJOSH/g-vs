#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SourceDto : public oatpp::DTO {
    DTO_INIT(SourceDto, DTO)

    DTO_FIELD(Int32, id);
    
    // Source settings
    // TODO make available
    // DTO_FIELD(String, cache_mode) = "none"; TEMP UNAVAILABLE
    DTO_FIELD(String, source_url);

    // Videoline settings
    DTO_FIELD(Int32, fps) = 15;
    DTO_FIELD(String, videoencoding) = "264";
    DTO_FIELD(Int32, bitrate) = 2000;

    // Audioline settings
    DTO_FIELD(Boolean, mute) = false;
    DTO_FIELD(Float32, volume) = 0.8;
};

#include OATPP_CODEGEN_END(DTO)