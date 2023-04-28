#include <api/controller/vsapi.hpp>

#include <video/live/live.hpp>
#include <video/videoserver/videoserver.hpp>

#include <chrono>

VSTypes::OatResponse VsapiController::getLive(const oatpp::Int32 &source, const std::shared_ptr<JwtPayload> &payload) {
    using namespace std;

    auto source_obj = vsapiService.getSourceById(source);

    if (!liveStreams.contains(source_obj->source_url))
    {   
        auto sourceid_str = oatpp::utils::conversion::int32ToStdStr(source);
        auto newHandler = std::make_shared<LiveHandler>(source, source_obj.getPtr());
        auto removebundle = new HandlerRemoveBundle{
            .target = newHandler,
            .liveStreams = &liveStreams,
            .liveStreams_UUID = &liveStreams_UUID
            //.maxAttempt = YOUR VALUE
        };

        OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
        removebundle->timer = g_timeout_add_seconds(10, G_SOURCE_FUNC(timeoutCheckUsage), removebundle);
        liveStreams[source_obj->source_url] = newHandler;
        liveStreams_UUID[sourceid_str] = newHandler;
    }
    
    auto lock = std::unique_lock<std::mutex>(liveStreams[source_obj->source_url]->getMutex());
    // false - timeout
    bool waitResult = liveStreams[source_obj->source_url]->getCv().wait_for(
        lock,
        1min,
        [live = liveStreams[source_obj->source_url]] {
            return live->isReady();
        }
    );

    return waitResult
           ? createResponse(Status::CODE_200, liveStreams[source_obj->source_url]->getPlaylist())
           : createResponse(Status::CODE_408);
}

VSTypes::OatResponse VsapiController::getStatic(const VSTypes::OatRequest &request, const oatpp::Int32 &source, const std::shared_ptr<JwtPayload> &payload) {
    using namespace std;

    auto filepath = request->getPathTail();
    auto sourceid_str = oatpp::utils::conversion::int32ToStdStr(source);

    if (!liveStreams_UUID.contains(sourceid_str))
        return createResponse(Status::CODE_404, "Stream not found");

    auto handler = liveStreams_UUID[sourceid_str];
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