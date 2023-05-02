#include <video/videoserver/videoserver.hpp>

#include <oatpp/network/Server.hpp>

#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <regex>

using boost::str;
using boost::format;

Videoserver::Accelerator Videoserver::accelerator = Videoserver::Accelerator::CPU;

Videoserver::Videoserver(Accelerator accelerator) 
: 
    // GStreamer params
    mainLoop(g_main_loop_new(nullptr, false))
{
    this->accelerator = accelerator;
}

void Videoserver::runMainLoop() {
    g_main_loop_run(this->mainLoop);
}

void Videoserver::stopMainLoop() {
    g_main_loop_quit(this->mainLoop);
}

GMainLoop *Videoserver::getMainLoop() const {
    return mainLoop;
}

Videoserver::~Videoserver() {
    g_main_loop_quit(mainLoop);
    g_main_loop_unref(mainLoop);
}

std::shared_ptr<Source> Videoserver::openSource(std::shared_ptr<SourceDto> config) {
    if (aliveSources.contains(config->source_url)) return aliveSources[config->source_url];

    auto newURI = authURI(config->source_url.getValue(""), config->login.getValue(""), config->password.getValue(""));
    OATPP_LOGD("Videoserver", "created new source %s", newURI.c_str());
    auto newSource = std::make_shared<Source>(newURI);
    aliveSources[newURI] = newSource;
    auto removeBranch = new RemoveBranchData {
        newSource,
        &aliveSources
    };
    
    newSource->setRemoveBranchCallback(onBranchRemoved, removeBranch);
    newSource->addBusCallback("eos", Source::BusCallbackData{
        G_CALLBACK(onSourceStop),
        removeBranch
    });
    newSource->addBusCallback("error", Source::BusCallbackData{
        G_CALLBACK(onSourceStop),
        removeBranch
    });
    newSource->setState();
        
    return newSource;
}

void Videoserver::removeBranchFromSource(const std::string &source, const std::string &branch) {
    OATPP_LOGD("Videoserver", "branch remove call");
    if (!aliveSources.contains(source)) return;

    auto targetSrc = aliveSources.at(source);
    targetSrc->removeBranch(branch);
}

void Videoserver::removeSource(const std::string &source) {
    if (aliveSources.contains(source)) {
        auto obj = aliveSources.at(source);
        aliveSources.erase(source);
    }
}

void Videoserver::onBranchRemoved(void* data) {
    RemoveBranchData* removeData = (RemoveBranchData*)data;
    OATPP_LOGD("Videoserver", "onBranchRemoved callback fired!");

    if (removeData->targetSource->isEmpty()) {
        removeData->allSources->erase(removeData->targetSource->getSource());
        delete removeData;
    }
}

void Videoserver::onSourceStop(GstBus *bus, GstMessage *message, gpointer data) {
    auto removeData = (RemoveBranchData*)data;
    OATPP_LOGD("Videoserver", "Removed source %s because something happend to stream", removeData->targetSource->getUUID().c_str());
    
    auto source = removeData->allSources->at(removeData->targetSource->getSource());
    auto pipetree = source->getPipeTree_UNSAFE();
    
    // Force remove all branches. They are in paused state (on error or eos) so ok!
    for (auto &branch : pipetree->getBranches())
        pipetree->removeBranch_UNSAFE(branch.first);

    // remove source itself
    removeData->allSources->erase(removeData->targetSource->getSource());
    int matched = g_signal_handlers_disconnect_by_func(bus, (gpointer)onSourceStop, data);
    OATPP_LOGD("Videoserver", "Disconnected %d handlers!", matched);
    delete removeData;
}

std::string Videoserver::authURI(const std::string &uri, const std::string &login, const std::string &password) {
    if (login.empty() && password.empty())
        return uri;

    std::regex rgx("(.+)://(.+)");
    std::smatch match;

    OATPP_ASSERT_HTTP(
        std::regex_match(uri, match, rgx), 
        VSTypes::OatStatus::CODE_422, 
        "Source URI is incorrect!"
    );
    
    auto protocol = match[1].str();
    auto tail = match[2].str();

    auto authURI = str(format("%s://%s:%s@%s") % protocol % login % password % tail);
    return authURI;
}