#include <api/controller/vsapi.hpp>

#include <video/live/live.hpp>
#include <video/videoserver/videoserver.hpp>

void VsapiController::onBranchStop(VsapiController::HandlerRemoveBundle* handlerToDelete) {
    std::string sourceUri = handlerToDelete->target->getSourceUri();
    OATPP_LOGW("VsapiController", "Branch %s stopped on timeout", sourceUri.c_str());

    if (!handlerToDelete->liveStreams->contains(sourceUri)) return;

    auto sourceid_str = oatpp::utils::conversion::int32ToStdStr(handlerToDelete->target->getSourceId());    
    handlerToDelete->liveStreams->erase(sourceUri);
    handlerToDelete->liveStreams_UUID->erase(sourceid_str);
    
    // Stop timeout timer   
    if (handlerToDelete->timer != -1)
        g_source_remove(handlerToDelete->timer);

    delete handlerToDelete;
    OATPP_LOGD("VsapiController", "Gotcha");
}

bool VsapiController::timeoutCheckUsage(gpointer data) {
    OATPP_LOGI("VsapiController", "About to check if timeout");
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
        OATPP_LOGI("VsapiController", "Really timed out. Remove this funny");
        onBranchStop(removeBundle);
        return false;
    } else if (!removeBundle->target->isReady())
        removeBundle->unreadyAttempt++;

    if (removeBundle->unreadyAttempt > removeBundle->maxAttempt)
    {
        OATPP_LOGW("VsapiController", "Source aint ready in %d attempts. Removing...", removeBundle->unreadyAttempt);
        onBranchStop(removeBundle);
        return false;
    }

    return true;
}