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
:   source(videoserver->openSource(source))
{
    this->streamBranch = this->source->runStream(config->hlsPath);

    auto currentSourceFolder = str(format("%s/%s") % config->hlsPath->c_str()
                                                   % this->source->getUUID());

    this->fileManager = std::make_shared<StaticFilesManager>(currentSourceFolder);
}

LiveHandler::~LiveHandler()
{
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
    //return fileManager->getFileCached(requestedSegment);
}

std::string LiveHandler::getUUID() const {
    return source->getUUID();
}

oatpp::String LiveHandler::guessMime(const oatpp::String &filename) const {
    return fileManager->guessMimeType(filename);
}