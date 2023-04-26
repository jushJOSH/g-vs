#include <api/controller/vsapi.hpp>

#include <video/live/live.hpp>
#include <video/videoserver/videoserver.hpp>

#include <chrono>

VSTypes::OatResponse VsapiController::getLive(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &source) {
    using namespace std;

    bool success;
    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    OATPP_ASSERT_HTTP(vsapiService.isSourceBelongsToUser(source, userid), 
                      Status::CODE_404, "No source found");
    auto source_obj = vsapiService.getSourceById(source);

    if (!liveStreams.contains(source_obj->host))
    {   
        auto config = jsonMapper->readFromString<oatpp::Object<SourceConfigDto>>(source_obj->config);
        auto newHandler = std::make_shared<LiveHandler>(source_obj->host, config);
        auto removebundle = new HandlerRemoveBundle{
            newHandler,
            0, // No timer
            0, // current attempts
            3, // max attempts
            &liveStreams,
            &liveStreams_UUID
        };

        OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
        removebundle->timer = g_timeout_add_seconds(10, G_SOURCE_FUNC(timeoutCheckUsage), removebundle);
        liveStreams[source_obj->host] = newHandler;
        liveStreams_UUID[newHandler->getSourceUUID()] = newHandler;   
    }
    
    auto lock = std::unique_lock<std::mutex>(liveStreams[source]->getMutex());
    // false - timeout
    bool waitResult = liveStreams[source]->getCv().wait_for(
        lock,
        1min,
        [live = liveStreams[source]] {
            return live->isReady();
        }
    );

    return waitResult
           ? createResponse(Status::CODE_200, liveStreams[source]->getPlaylist())
           : createResponse(Status::CODE_408);
}

VSTypes::OatResponse VsapiController::getStatic(const VSTypes::OatRequest &request, const oatpp::String &uuid, const std::shared_ptr<JwtPayload> &payload) {
    using namespace std;

    auto filepath = request->getPathTail();
    auto range = request->getHeader(Header::RANGE);
    
    if (!liveStreams_UUID.contains(uuid))
        return createResponse(Status::CODE_404, "Stream not found");

    auto handler = liveStreams_UUID[uuid];
    auto lock = std::unique_lock<std::mutex>(handler->getMutex());
    // false - timeout
    bool waitResult = handler->getCv().wait_for(
        lock,
        10s,
        [handler] {
            return handler->isReady();
        }
    );

    return handler->getSegment(filepath);
}

VSTypes::OatResponse VsapiController::getFrame(const std::shared_ptr<JwtPayload> &payload, const oatpp::web::server::api::ApiController::QueryParams &QueryParams) {

}

VSTypes::OatResponse VsapiController::modifyArchive(const std::shared_ptr<JwtPayload> &payload, const oatpp::String &mediainfo) {

}