#include <video/live/live.hpp>

#include <video/videoserver/videoserver.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <oatpp/web/protocol/http/outgoing/StreamingBody.hpp>
#include <oatpp/core/data/stream/FileStream.hpp>

#include <oatpp/web/protocol/http/outgoing/ResponseFactory.hpp>

using boost::format;
using boost::str;

LiveHandler::LiveHandler(int sourceid, std::shared_ptr<SourceConfigDto> config)
:   source_uri(config->source_url)
{   
    auto o_source = videoserver->openSource(config);
    auto hlsPath = str(format("%s/%d") % appconfig->hlsPath->c_str() % sourceid);
    auto o_branch = o_source->runStream(hlsPath);

    this->source_uuid = o_source->getUUID();
    this->branch_uuid = o_branch->getUUID();

    this->hlsconfig = o_source->makeConfig(hlsPath);
    this->fileManager = std::make_shared<StaticFilesManager>(this->hlsconfig->playlist_folder);

    bundle.issuer = this;

    GFile* directory = g_file_new_for_path(this->hlsconfig->playlist_folder.c_str());
    this->hlswatchdog = g_file_monitor_directory(directory, G_FILE_MONITOR_WATCH_MOVES, NULL, NULL);
    if (this->hlswatchdog != NULL)
        g_signal_connect(this->hlswatchdog, "changed", G_CALLBACK(folderChanged), &bundle);    
    
    g_object_unref(directory);
}

LiveHandler::~LiveHandler()
{
    OATPP_LOGD("LiveHandler", "Deleted %s", source_uuid.c_str());
    videoserver->removeBranchFromSource(source_uuid, branch_uuid);
    g_object_unref(hlswatchdog);
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

void LiveHandler::folderChanged(GFileMonitor *m, GFile* file, GFile *other, GFileMonitorEvent event, gpointer data) {
    auto bundle = (ReadyNotificationBundle*)data;
    switch (event) {
        case GFileMonitorEvent::G_FILE_MONITOR_EVENT_CREATED:
        {
            auto filename = std::string(g_file_get_basename(file));
            if (filename.ends_with(".m3u8")) {
                OATPP_LOGD("LiveHandler", "Created new playlist %s", filename.c_str());
                return;
            } else if (filename.ends_with(".ts")) {
                OATPP_LOGD("LiveHandler", "Created new playlist segment %s", filename.c_str());
                
                if (bundle->segmentCount >= 2) {
                    OATPP_LOGI("LiveHandler", "Stream is ready");
                    auto lock = std::lock_guard<std::mutex>(bundle->commonMutex);
                    bundle->issuer->lastSegmentRequest = std::chrono::system_clock::now();
                    bundle->ready = true;
                    bundle->cv.notify_all();
                    
                    g_file_monitor_cancel(m);
                } else bundle->segmentCount++;
            }
        }
        break;
        default:
        OATPP_LOGD("LiveHandler", "Received non-create event");
        return;
    }
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

GFileMonitor* LiveHandler::getWatchdog() const {
    return hlswatchdog;
}