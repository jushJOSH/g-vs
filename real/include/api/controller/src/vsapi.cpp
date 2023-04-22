#include <api/controller/vsapi.hpp>

#include <video/live/live.hpp>
#include <video/videoserver/videoserver.hpp>

#include <chrono>

VSTypes::OatResponse VsapiController::getLive(const oatpp::String &source) {
    using namespace std;
    
    if (!liveStreams.contains(source))
    {
        auto newHandler = std::make_shared<LiveHandler>(source);
        auto removebundle = new HandlerRemoveBundle{
            newHandler,
            &liveStreams,
            &liveStreams_UUID
        };
        // newHandler->getSource()->addBusCallback("eos", Source::BusCallbackData{
        //     G_CALLBACK(onSourceStop),
        //     removebundle
        // });
        // newHandler->getSource()->addBusCallback("error", Source::BusCallbackData{
        //     G_CALLBACK(onSourceStop),
        //     removebundle
        // });
        g_timeout_add_seconds(10, G_SOURCE_FUNC(timeoutCheckUsage), removebundle);
        liveStreams[source] = newHandler;
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

VSTypes::OatResponse VsapiController::getStatic(const VSTypes::OatRequest &request, const oatpp::String &uuid) {
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

bool VsapiController::onSourceStop(GstBus *bus, GstMessage *message, gpointer data) {
    auto handlerToDelete = (HandlerRemoveBundle*)data;
    std::string sourceUri = handlerToDelete->target->getSourceUri();
    OATPP_LOGD("VsapiController", "Source %s end stopped for some reason", sourceUri.c_str());

    if (!handlerToDelete->liveStreams->contains(sourceUri)) return false;

    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    videoserver->removeBranchFromSource(sourceUri, handlerToDelete->target->getBranchUUID());
    handlerToDelete->liveStreams->erase(sourceUri);
    handlerToDelete->liveStreams_UUID->erase(handlerToDelete->target->getSourceUUID());

    OATPP_LOGD("VsapiController", "Gotcha");
    return false;
}

bool VsapiController::timeoutCheckUsage(gpointer data) {
    OATPP_LOGD("VsapiController", "About to check if timeout");
    auto removeBundle = (HandlerRemoveBundle*)data;

    auto currentTime = std::chrono::system_clock::now();
    auto previousTime = removeBundle->target->getLastTimestamp();
    int bias = removeBundle->target->getBias();
    int segmentLenght = removeBundle->target->getSegmentDuration();
    
    OATPP_LOGD("VsapiController", "Lets see:\nPrev: %ld\nCurr: %ld", 
                (previousTime + std::chrono::seconds(segmentLenght) + std::chrono::seconds(bias)).time_since_epoch().count(),
                currentTime.time_since_epoch().count());
    if ((previousTime + std::chrono::seconds(segmentLenght) + std::chrono::seconds(bias)) < currentTime
        && removeBundle->target->isReady())
    {
        OATPP_LOGD("VsapiController", "Really timed out. Remove this funny");
        onSourceStop(NULL, NULL, data);
        return false;
    }

    return true;
}