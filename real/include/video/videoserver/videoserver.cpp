#include <video/videoserver/videoserver.hpp>

#include <oatpp/network/Server.hpp>

#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

#include <boost/regex.hpp>

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

    OATPP_LOGD("Videoserver", "created new source %s", config->source_url->c_str());
    auto newSource = std::make_shared<Source>(config->source_url);
    newSource->setRemoveBranchCallback(onBranchRemoved, new RemoveBranchData {
        newSource.get(),
        &aliveSources
    });
    newSource->addBusCallback("eos", Source::BusCallbackData{
        G_CALLBACK(onSourceStop),
        new RemoveBranchData{
            newSource.get(),
            &aliveSources
        }
    });
    newSource->addBusCallback("error", Source::BusCallbackData{
        G_CALLBACK(onSourceStop),
        new RemoveBranchData{
            newSource.get(),
            &aliveSources
        }
    });
    newSource->setState();
    aliveSources[config->source_url] = newSource;
        
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

bool Videoserver::onSourceStop(GstBus *bus, GstMessage *message, gpointer data) {
    auto removeData = (RemoveBranchData*)data;
    OATPP_LOGD("Videoserver", "Removed source %s because something happend to stream", removeData->targetSource->getUUID().c_str());
    
    auto source = removeData->allSources->at(removeData->targetSource->getSource());
    auto pipetree = source->getPipeTree_UNSAFE();
    
    // Force remove all branches. They are in paused state (on error or eos) so ok!
    for (auto &branch : pipetree->getBranches())
        pipetree->removeBranch_UNSAFE(branch.first);

    // remove source itself
    removeData->allSources->erase(removeData->targetSource->getSource());
    delete removeData;

    return false;
}