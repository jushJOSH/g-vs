#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class SourceDto : public oatpp::DTO {
    DTO_INIT(SourceDto, DTO)

    DTO_FIELD(String, name);
    DTO_FIELD(String, host);
    DTO_FIELD(String, config);
};

#include OATPP_CODEGEN_END(DTO)