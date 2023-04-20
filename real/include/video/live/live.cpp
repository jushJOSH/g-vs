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
:   source(videoserver->openSource(source)),
    source_uri(source)
{   
    this->source->addBusCallback("element", Source::BusCallbackData{
        G_CALLBACK(onElementMessage),
        &bundle
    });
    this->streamBranch = this->source->runStream(config->hlsPath);

    auto currentSourceFolder = str(format("%s/%s") % config->hlsPath->c_str()
                                                   % this->source->getUUID());

    this->fileManager = std::make_shared<StaticFilesManager>(currentSourceFolder);
}

LiveHandler::~LiveHandler()
{
    OATPP_LOGD("LiveHandler", "Deleted %s", source->getUUID());
    videoserver->removeBranchFromSource(source->getUUID(), streamBranch->getUUID());
}

oatpp::String LiveHandler::getPlaylist() {
    return fileManager->getFile("playlist.m3u8");
}

VSTypes::OatResponse LiveHandler::getSegment(const oatpp::String &requestedSegment) {
    auto file = str(format("%s/%s") % fileManager->getBasePath()->c_str() % requestedSegment->c_str());

    auto body = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(
        std::make_shared<oatpp::data::stream::FileInputStream>(file.c_str())
    );

    return oatpp::web::protocol::http::outgoing::Response::createShared(VSTypes::OatStatus::CODE_200, body);
}

std::string LiveHandler::getUUID() const {
    return source->getUUID();
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
    g_print("%s\n", gst_structure_to_string(messageStruct));

    if (!gst_structure_has_name(messageStruct, "splitmuxsink-fragment-closed")) {
        OATPP_LOGD("LiveHandler", "Received message is not fragment closed or whatever");
        return true;
    }
    OATPP_LOGD("LiveHandler", "Created HLS segment. Now we think what stream is ready to watch!");
    
    // If already created 3 segments. Up to 3 segments is stable ???
    // TODO fix unstable first segment
    bundle->packetCount++;
    if (bundle->packetCount >= 3) {
        auto lock = std::lock_guard<std::mutex>(bundle->commonMutex);
        bundle->ready = true;
        bundle->cv.notify_all();
    }

    // By returning false we disconnect handler from signal
    return false;
}

std::shared_ptr<Source> LiveHandler::getSource() const {
    return this->source;
}

std::string LiveHandler::getSourceUri() const {
    return this->source_uri;
}
