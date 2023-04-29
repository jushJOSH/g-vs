#pragma once

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ConfigDto : public oatpp::DTO {
    DTO_INIT(ConfigDto, DTO)

    // Default config
    DTO_FIELD(String, host) = "0.0.0.0";
    DTO_FIELD(UInt16, port) = 8080;
    DTO_FIELD(String, dbConnectionString);
    
    // Video config
    DTO_FIELD(Int32, hlsBias) = 6;
    DTO_FIELD(Int32, hlsPlaylistLenght) = 4;
    DTO_FIELD(Int32, hlsSegmentDuration) = 4;
    DTO_FIELD(String, hlsPath) = "/home/egor/hls_test";
    
    // Security config
    // Token
    DTO_FIELD(String, tokenSalt);
    DTO_FIELD(String, issuer) = "vs:jwt-issuer";
    
    // OpenSSL
    DTO_FIELD(String, apiCert);
    DTO_FIELD(String, apiKey);
    DTO_FIELD(String, apiDiffieHellman);
};

#include OATPP_CODEGEN_END(DTO)