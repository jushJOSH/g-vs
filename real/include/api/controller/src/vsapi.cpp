#include <api/controller/vsapi.hpp>

#include <gst/gst.h>
#include <gst/app/app.h>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

VSTypes::OatResponse VsapiController::getLive(const VSTypes::OatRequest &request) {
    auto multipart = std::make_shared<MPStreamer>();
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
        multipart,
        "multipart/x-mixed-replace",
        true
    );

    return OutgoingResponse::createShared(Status::CODE_200, body);
}