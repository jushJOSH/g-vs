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
    sourceElements = std::make_shared<PipeTree>(source);
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

void Source::runStream(std::shared_ptr<StreamBranch> streamBranch) {
    sourceElements->addBranch(streamBranch);
}

std::string Source::runArchive(std::shared_ptr<ArchiveBranch> archiveBranch) {
    sourceElements->addBranch(archiveBranch);

    return archiveBranch->getPath();
}

std::string Source::makeScreenshot(std::shared_ptr<ScreenshotBranch> branch) {
    auto &datalines = sourceElements->getDatalines();
    auto found = std::find_if(datalines.begin(), datalines.end(), [](std::shared_ptr<DataLine>& elem){
        return elem->getType() == DataLine::LineType::Video;
    });
    if (found == datalines.end()) return "";

    auto videoline = std::reinterpret_pointer_cast<VideoLine>(*found);    
    return videoline->makeScreenshot(branch) ? branch->getPath() : "";
}