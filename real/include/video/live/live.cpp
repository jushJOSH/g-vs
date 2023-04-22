#include <video/live/live.hpp>

#include <video/videoserver/videoserver.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <oatpp/web/protocol/http/outgoing/StreamingBody.hpp>
#include <oatpp/core/data/stream/FileStream.hpp>

#include <oatpp/web/protocol/http/outgoing/ResponseFactory.hpp>

using boost::format;
using boost::str;

LiveHandler::LiveHandler(const std::string &source)
:   source_uri(source)
{   
    auto o_source = videoserver->openSource(source);
    auto o_branch = o_source->runStream(config->hlsPath);

    this->source_uuid = o_source->getUUID();
    this->branch_uuid = o_branch->getUUID();

    this->hlsconfig = o_branch->getConfig();
    this->fileManager = std::make_shared<StaticFilesManager>(this->hlsconfig->playlist_folder);

    bundle.issuer = this;
    auto id = o_source->addBusCallback("element", Source::BusCallbackData{
        G_CALLBACK(onElementMessage),
        &bundle
    });
    bundle.callback = id;
}

LiveHandler::~LiveHandler()
{
    OATPP_LOGD("LiveHandler", "Deleted %s", source_uuid);
    videoserver->removeBranchFromSource(source_uuid, branch_uuid);
}

oatpp::String LiveHandler::getPlaylist() {
    return fileManager->getFile("playlist.m3u8");
}

VSTypes::OatResponse LiveHandler::getSegment(const oatpp::String &requestedSegment) {
    auto file = str(format("%s/%s") % fileManager->getBasePath()->c_str() % requestedSegment->c_str());

    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(
        std::make_shared<oatpp::data::stream::FileInputStream>(file.c_str())
    );

    this->lastSegmentRequest = std::chrono::system_clock::now();
    return oatpp::web::protocol::http::outgoing::Response::createShared(VSTypes::OatStatus::CODE_200, body);
}

oatpp::String LiveHandler::guessMime(const oatpp::String &filename) const {
    return fileManager->guessMimeType(filename);
}

bool LiveHandler::isReady() const {
    return bundle.ready;
} 

std::mutex &LiveHandler::getMutex() {
    return bundle.commonMutex;
} 

std::condition_variable &LiveHandler::getCv() {
    return bundle.cv;
} 

bool LiveHandler::onElementMessage(GstBus *bus, GstMessage *message, gpointer data) {
    auto bundle = (ReadyNotificationBundle*)data;

    auto messageStruct = gst_message_get_structure(message);
    if (!messageStruct) {
        OATPP_LOGD("LiveHandler", "Received message has no structure in it");
        return true;
    }

    if (!gst_structure_has_name(messageStruct, "splitmuxsink-fragment-closed")) {
        OATPP_LOGD("LiveHandler", "Received message is not fragment closed or whatever");
        return true;
    }
    
    // If already created 3 segments. Up to 3 segments is stable ???
    // TODO fix unstable first segment
    bundle->packetCount++;
    if (bundle->packetCount >= 3) {
        OATPP_LOGD("LiveHandler", "Now we think what stream is ready to watch!");
        auto lock = std::lock_guard<std::mutex>(bundle->commonMutex);
        bundle->issuer->lastSegmentRequest = std::chrono::system_clock::now();
        bundle->ready = true;
        bundle->cv.notify_all();
        g_signal_handler_disconnect(bus, bundle->callback);
    }

    return false;
}

std::string LiveHandler::getSourceUUID() const {
    return this->source_uuid;
}

std::string LiveHandler::getSourceUri() const {
    return this->source_uri;
}

std::string LiveHandler::getBranchUUID() const {
    return this->branch_uuid;
}

timestamp LiveHandler::getLastTimestamp() const {
    return lastSegmentRequest;
}

int LiveHandler::getBias() const {
    return hlsconfig->bias;
}

int LiveHandler::getSegmentDuration() const {
    return hlsconfig->target_duration;
}