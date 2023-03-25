#include <api/token/jwt.hpp>
#include <experimental/optional>

JWT::JWT(const oatpp::String& secret, const oatpp::String& issuer)
:   issuer(issuer), secret(secret),
    verifier(jwt::verify()
            .with_issuer(this->issuer)
            .allow_algorithm(jwt::algorithm::hs512{secret}))
{}

// TODO add token lifespan in config dto
oatpp::String JWT::createToken(const std::shared_ptr<JwtPayload>& payload) {
    auto token = jwt::create()
                    .set_issuer(this->issuer)
                    .set_type("vs:JWT")
                    .set_payload_claim("userid", jwt::claim(payload->userId))
                    .set_payload_claim("username", jwt::claim(payload->username))
                    .set_issued_at(std::chrono::system_clock::now())
                    .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{4})
                    .sign(jwt::algorithm::hs512{secret});

    return token;
}

std::shared_ptr<JwtPayload> JWT::verifyToken(const oatpp::String& token) {
    using std::experimental::optional;
    using std::experimental::in_place;

    // Validating received token
    optional<jwt::decoded_jwt<jwt::traits::kazuho_picojson>> decodedToken;
    try {
        decodedToken = optional<jwt::decoded_jwt<jwt::traits::kazuho_picojson>>(in_place, jwt::decode(token));
        verifier.verify(*decodedToken);
    } catch(const std::exception& e) {
        OATPP_LOGI("JWT Verifier", "Failed to verify token.");
        OATPP_LOGD("JWT Verifier", e.what());

        return nullptr;
    }

    // Getting info of this token;
    std::shared_ptr payload = std::make_shared<JwtPayload>();
    try {
        payload->userId = decodedToken->get_payload_claim("userid").as_string();
        payload->username = decodedToken->get_payload_claim("username").as_string();
    } catch (const std::exception& e) {
        OATPP_LOGE("JWT Verifier", "Failed to parse payload for data");
        OATPP_LOGD("JWT Verifier", e.what());

        return nullptr;
    }

    return payload;
}