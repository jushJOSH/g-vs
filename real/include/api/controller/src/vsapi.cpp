#include <api/controller/vsapi.hpp>
#include <video/live/live.hpp>

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
        newHandler->getSource()->addBusCallback("eos", Source::BusCallbackData{
            G_CALLBACK(onSourceStop),
            removebundle
        });
        newHandler->getSource()->addBusCallback("error", Source::BusCallbackData{
            G_CALLBACK(onSourceStop),
            removebundle
        });
        liveStreams[source] = newHandler;
        liveStreams_UUID[newHandler->getUUID()] = newHandler;
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

    return getStaticFileResponse(handler, filepath, range);
}

VSTypes::OatResponse VsapiController::getStaticFileResponse(std::shared_ptr<LiveHandler> handler,
                                                            const oatpp::String& filepath,
                                                            const oatpp::String& rangeHeader) const
{
    auto file = handler->getSegment(filepath);
    return file;
}

bool VsapiController::onSourceStop(GstBus *bus, GstMessage *message, gpointer data) {
    auto handlerToDelete = (HandlerRemoveBundle*)data;
    std::string sourceUri = handlerToDelete->target->getSourceUri();
    OATPP_LOGD("VsapiController", "Source %s end stopped for some reason", sourceUri);

    if (handlerToDelete->liveStreams->contains(sourceUri)) return false;

    handlerToDelete->liveStreams->erase(sourceUri);
    handlerToDelete->liveStreams_UUID->erase(handlerToDelete->target->getUUID());
    OATPP_LOGD("VsapiController", "Gotcha");
    return false;
}
