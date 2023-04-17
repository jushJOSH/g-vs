#include <video/source/source.hpp>
#include <video/source/datalines/video.hpp>

#include <atomic>

Source::Source() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()()))
{   
    g_print("Created source %s\n", uuid.c_str());
}

Source::Source(const std::string &source)
:   Source()
{   
    sourceElements = std::make_unique<PipeTree>(source);
    this->source = source;
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
    g_print("Deleted source %s\n", uuid.c_str());
}

void Source::setConfig(SourceConfigDto& config) {
    sourceElements->setConfig(config);
}

std::string Source::getUUID() const {
    return uuid;
}

std::shared_ptr<StreamBranch> Source::runStream() {
    auto streamBranch = std::make_shared<StreamBranch>();
    sourceElements->addBranch(streamBranch);

    return streamBranch;
}

std::shared_ptr<StreamBranch> Source::runStream(std::condition_variable* untilBranchReady, std::mutex *commonBranchMutex) {
    auto streamBranch = std::make_shared<StreamBranch>(untilBranchReady, commonBranchMutex);
    sourceElements->addBranch(streamBranch);

    return streamBranch;
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
