#include <api/controller/vsapi.hpp>
#include <video/videoserver/videoserver.hpp>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

VSTypes::OatResponse VsapiController::getLive() {
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    auto source = videoserver->openSource("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream");

    auto multipart = std::make_shared<MPStreamer>(source->runStream(), source);
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
        multipart,
        "application/octet-stream",
        true
    );
    
    return OutgoingResponse::createShared(Status::CODE_200, body);
}