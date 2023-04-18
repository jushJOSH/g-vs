#include <api/controller/vsapi.hpp>
#include <video/videoserver/videoserver.hpp>

#include <chrono>
#include <thread>

VSTypes::OatResponse VsapiController::getLive() {
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    // auto source = videoserver->openSource("file:///home/egor/test.webm");
    auto source = videoserver->openSource("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream");
    auto firedBranch = source->runStream();
    
    using namespace std;
    this_thread::sleep_for(10+s);

    auto response = createResponse(Status::CODE_200, oatpp::String::loadFromFile("/home/egor/hls_test/playlist.m3u8"));
    response->putHeader("Accept-Ranges", "bytes");
    response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
    response->putHeader(Header::CONTENT_TYPE, "application/x-mpegURL");
    return response;
}