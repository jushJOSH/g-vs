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
    if (!aliveSources.contains(source)) {
        g_print("Videoserver: created new source %s\n", source.c_str());
        aliveSources[source] = std::make_shared<Source>(source);
        aliveSources[source]->setState();
    }
        
    return aliveSources.at(source);
}

void Videoserver::removeBranchFromSource(const std::string &source, const std::string &branch) {
    if (!aliveSources.contains(source)) return;

    auto targetSrc = aliveSources.at(source);
    targetSrc->removeBranch(branch);
    if (targetSrc->isEmpty())
        aliveSources.erase(source);
}