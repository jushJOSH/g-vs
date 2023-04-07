#include <api/controller/vsapi.hpp>
#include <video/source/source.hpp>

#include <oatpp/web/protocol/http/outgoing/MultipartBody.hpp>

// VSTypes::OatResponse VsapiController::getLive() {
//     auto newSource = std::make_shared<Source>();
//     auto stateResult = newSource->setState();
//     g_print("Status %d\n", stateResult);

//     auto multipart = std::make_shared<MPStreamer>(newSource);
//     auto body = std::make_shared<oatpp::web::protocol::http::outgoing::MultipartBody>(
//         multipart,
//         "application/octet-stream",
//         true
//     );

//     return OutgoingResponse::createShared(Status::CODE_200, body);
// }

VSTypes::OatResponse VsapiController::getLive() {
    SourceConfigDto config;
    Source *testSource = new Source("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream", config);

    auto screenshot = std::make_shared<ScreenshotBranch>("/home/egor/test.png");
    auto archive = new ArchiveBranch("/home/egor/test.mp4");
    auto stream = std::make_shared<StreamBranch>();

    auto changeStateResult = testSource->setState();
    //testSource->addBranch(std::make_shared<ArchiveBranch>(*archive));
    //testSource.addBranch(stream);

    if (changeStateResult == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        return createResponse(Status::CODE_500);
    }

    return createResponse(Status::CODE_200);
}