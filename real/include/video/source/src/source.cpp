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

std::shared_ptr<StreamBranch> Source::runStream() {
    auto streamBranch = std::make_shared<StreamBranch>();
    sourceElements->addBranch(streamBranch);

    return streamBranch;
}

std::shared_ptr<ArchiveBranch> Source::runArchive(const std::string &path) {
    auto archiveBranch = std::make_shared<ArchiveBranch>(path);
    sourceElements->addBranch(archiveBranch);

    return archiveBranch;
}

std::shared_ptr<ScreenshotBranch> Source::makeScreenshot(const std::string &path) {
    auto screenshotBranch = std::make_shared<ScreenshotBranch>(path);
    auto &datalines = sourceElements->getDatalines();
    auto found = std::find_if(datalines.begin(), datalines.end(), [](std::shared_ptr<DataLine>& elem){
        return elem->getType() == DataLine::LineType::Video;
    });
    if (found == datalines.end()) return nullptr;

    auto videoline = std::reinterpret_pointer_cast<VideoLine>(*found);
    return screenshotBranch;   
    return videoline->makeScreenshot(screenshotBranch) ? screenshotBranch : nullptr;
}