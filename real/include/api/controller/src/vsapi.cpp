#include <api/controller/vsapi.hpp>
#include <video/source/source.hpp>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

VSTypes::OatResponse VsapiController::getLive() {
    auto newSource = std::make_shared<Source>();
    auto stateResult = newSource->setState();
    g_print("Status %d\n", stateResult);

    auto multipart = std::make_shared<MPStreamer>(newSource);
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
        multipart,
        "application/octet-stream",
        true
    );

    return OutgoingResponse::createShared(Status::CODE_200, body);
}