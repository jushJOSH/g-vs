#include <video/videoserver/videoserver.hpp>

#include <oatpp/network/Server.hpp>

#include <api/component/app.hpp>
#include <api/component/database.hpp>
#include <api/component/service.hpp>

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

std::shared_ptr<Source> Videoserver::openSource(const std::string& source) {
    if (aliveSources.contains(source)) return aliveSources[source];

    g_print("Videoserver: created new source %s\n", source.c_str());
    auto newSource = std::make_shared<Source>(source);
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
    newSource->setState();
    aliveSources[source] = newSource;
        
    return aliveSources.at(source);
}

void Videoserver::removeBranchFromSource(const std::string &source, const std::string &branch) {
    if (!aliveSources.contains(source)) return;

    auto targetSrc = aliveSources.at(source);
    targetSrc->removeBranch(branch);
}

void Videoserver::removeSource(const std::string &source) {
    if (aliveSources.contains(source))
        aliveSources.erase(source);
}

void Videoserver::onBranchRemoved(void* data) {
    RemoveBranchData* removeData = (RemoveBranchData*)data;
    g_print("onBranchRemoved callback fired!\n");

    if (removeData->targetSource->isEmpty()) {
        removeData->allSources->erase(removeData->targetSource->getSource());
        delete removeData;
    }
}

bool Videoserver::onSourceStop(GstBus *bus, GstMessage *message, gpointer data) {
    auto removeData = (RemoveBranchData*)data;
    g_print("Removed source %s because something happend to stream\n", removeData->targetSource->getUUID().c_str());
    
    removeData->allSources->erase(removeData->targetSource->getUUID());
    delete removeData;

    return false;
}