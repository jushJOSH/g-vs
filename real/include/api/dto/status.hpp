#pragma once

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class StatusDto : public oatpp::DTO {

  DTO_INIT(StatusDto, DTO)

  DTO_FIELD(String, status);
  DTO_FIELD(Int32, code);
  DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)