#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ConfigDto : public oatpp::DTO {
  
  DTO_INIT(ConfigDto, DTO)

  DTO_FIELD(String, host);
  DTO_FIELD(UInt16, port);
  DTO_FIELD(String, swaggerHost);
  DTO_FIELD(String, dbConnectionString);
  DTO_FIELD(String, secretPath) = "/home/egor/g-vs/real/secret.pem";
  DTO_FIELD(String, issuer) = "vs:jwt-issuer";

};

#include OATPP_CODEGEN_END(DTO)