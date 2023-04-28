#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ConfigDto : public oatpp::DTO {
    DTO_INIT(ConfigDto, DTO)

    DTO_FIELD(String, host) = "0.0.0.0";
    DTO_FIELD(UInt16, port) = 8080;
    DTO_FIELD(String, swaggerHost);
    DTO_FIELD(String, dbConnectionString);
    DTO_FIELD(Int32, bias) = 6;
    DTO_FIELD(Int32, hlsPlaylistLenght) = 4;
    DTO_FIELD(Int32, hlsSegmentDuration) = 4;
    DTO_FIELD(String, tokenSalt) = "/home/egor/g-vs/real/secret.pem";
    DTO_FIELD(String, apiCert);
    DTO_FIELD(String, apiKey);
    DTO_FIELD(String, hlsPath) = "/home/egor/hls_test";
    DTO_FIELD(String, archivePath) = "/home/egor/archive_test";
    DTO_FIELD(String, issuer) = "vs:jwt-issuer";
    DTO_FIELD(String, hardware_acceleration) = "cpu";
};

#include OATPP_CODEGEN_END(DTO)