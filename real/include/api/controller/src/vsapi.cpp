#include <api/controller/vsapi.hpp>
#include <video/videoserver/videoserver.hpp>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

VSTypes::OatResponse VsapiController::getLive() {
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    auto source = videoserver->openSource("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream");
    auto stateResult = source->setState();
    g_print("Status %d\n", stateResult);

    auto multipart = std::make_shared<MPStreamer>(source->runStream());
    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
        multipart,
        "application/octet-stream",
        true
    );

    return OutgoingResponse::createShared(Status::CODE_200, body);
}

// VSTypes::OatResponse VsapiController::getLive() {
//     SourceConfigDto config;
//     Source *testSource = new Source("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream", config);

//     auto archive = new ArchiveBranch("/home/egor/test.mp4");
//     auto screenshot = new ScreenshotBranch("/home/egor/test.png");
//     auto stream = new StreamBranch();

//     stream->setCallback(G_CALLBACK(StreamBranch::onNewSample));

//     auto changeStateResult = testSource->setState();
//     testSource->runArchive(std::make_shared<ArchiveBranch>(*archive));
//     testSource->runStream(std::make_shared<StreamBranch>(*stream));
//     testSource->makeScreenshot(std::make_shared<ScreenshotBranch>(*screenshot));

//     if (changeStateResult == GST_STATE_CHANGE_FAILURE) {
//         g_printerr ("Unable to set the pipeline to the playing state.\n");
//         return createResponse(Status::CODE_500);
//     }

//     return createResponse(Status::CODE_200);
// }