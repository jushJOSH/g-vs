#include <video/source/source.hpp>
#include <video/source/datalines/video.hpp>

#include <oatpp/core/base/Environment.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

Source::Source() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()()))
{   
    OATPP_LOGD("Source", "Created source %s", uuid.c_str());
}

Source::Source(const std::string &source)
:   Source()
{   
    sourceElements = std::make_unique<PipeTree>(source);
    this->source = source;
    this->bus = this->sourceElements->getBus();
    gst_bus_add_signal_watch(this->bus);
}

Source::Source(const std::string &source, SourceConfigDto& config)
:   Source(source)
{
    sourceElements->setConfig(config);
}

GstStateChangeReturn Source::setState(GstState state) {
    return sourceElements->setState(state);
}

Source::~Source() {
    OATPP_LOGD("Source", "Deleted source %s", uuid.c_str());
    g_object_unref(bus);
}

void Source::setConfig(SourceConfigDto& config) {
    sourceElements->setConfig(config);
}

std::string Source::getUUID() const {
    return uuid;
}

std::shared_ptr<StreamBranch> Source::runStream(const std::string &hlsFolder) {
    auto config = makeConfig(hlsFolder);
    auto streamBranch = std::make_shared<StreamBranch>(config);
    sourceElements->addBranch(streamBranch);

    return streamBranch;
}

std::shared_ptr<HLSConfig> Source::makeConfig(const std::string &hlsFolder, int playlistLenght, int bias) {
    return std::make_shared<HLSConfig>(hlsFolder, this->uuid, playlistLenght, bias);
}

std::shared_ptr<ArchiveBranch> Source::runArchive(const std::string &path) {
    auto archiveBranch = std::make_shared<ArchiveBranch>(path);
    sourceElements->addBranch(archiveBranch);

    return archiveBranch;
}

void Source::removeBranch(const std::string &branchid) {
    sourceElements->removeBranch(branchid);
}

bool Source::isEmpty() const {
    return sourceElements->noMoreBranches();
}

std::string Source::getSource() const {
    return source;
}

void Source::setRemoveBranchCallback(const std::function<void(void*)> callback, void* data) {
    sourceElements->setOnBranchDeleted(callback, data);
}

gulong Source::addBusCallback(const std::string &message, BusCallbackData data) {
    auto signal = str(format("message::%s") % message);
    return g_signal_connect(this->bus, signal.c_str(), G_CALLBACK(data.callback), data.data);
}

PipeTree* Source::getPipeTree_UNSAFE() const {
    return sourceElements.get();
}