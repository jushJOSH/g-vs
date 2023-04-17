#include <api/controller/vsapi.hpp>
#include <video/videoserver/videoserver.hpp>

#include <chrono>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

VSTypes::OatResponse VsapiController::getLive() {
    // For chrono!
    using namespace std;

    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    auto source = videoserver->openSource("https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm");

    auto firedBranch = source->runStream(&untilBranchReady, &commonBranchMutex);
    std::unique_lock lock(commonBranchMutex);
    if (untilBranchReady.wait_for(
        lock,
        10s,
        [firedBranch]{
            return firedBranch->isReady();
        }
    ))
    {
        OATPP_LOGI("MPStreamer", "Ain't timed out");
        auto multipart = std::make_shared<MPStreamer>(firedBranch, source);
        auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
            multipart,
            "application/octet-stream",
            true
        );

        return OutgoingResponse::createShared(Status::CODE_200, body);
    }
    else 
    {   
        OATPP_LOGI("MPStreamer", "Connection timeout for branch");
        videoserver->removeBranchFromSource(source->getSource(), firedBranch->getUUID());
        return createResponse(Status::CODE_408);
    }
}