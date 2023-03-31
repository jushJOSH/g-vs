#include <video/source/RTSP/rtsp.hpp>

#include <boost/format.hpp>
#include <sys/types.h>
#include <sys/stat.h>

GstElement* RTSPSource::createEncoder(Videoserver::Accelerator accelerator) {
    std::string platform = "";
    switch (accelerator) {
        case Videoserver::Accelerator::AMD:
            platform = "amf";
        break;
        case Videoserver::Accelerator::NVIDIA:
            platform = "nvcuda";          
        break;
        default: 
            platform = "";
    }

    return gst_element_factory_make(
        boost::str(boost::format("%1%%2%enc") % platform % dtoConfig.encoding).c_str(),
        "encoder"
    );
}

GstElement* RTSPSource::createSrc(const std::string &host) {
    GstElement* source = gst_element_factory_make("rtspsrc", "source");
    g_object_set(G_OBJECT(source), "location", host.c_str(), NULL);

    return source;
}

GstElement* RTSPSource::createAppsink() {
    GstElement* appsink = gst_element_factory_make("appsink", "stream");

    g_object_set(G_OBJECT(sourceElements.appsink), "emit-signals", TRUE, NULL);
    g_signal_connect(sourceElements.appsink, "new-sample", G_CALLBACK(on_new_sample), (gpointer*)arg.get());    

    return appsink;
}

GstElement* RTSPSource::createFilesink() {
    GstElement* filesink = gst_element_factory_make("filesink", "archive");
    g_object_set(G_OBJECT(filesink), "location", dtoConfig.archive_path->c_str(), NULL);
}

RTSPSource::RTSPSource(
    const std::string &name,
    const std::string &host,
    const Videoserver::Accelerator accelerator,
    const SourceConfigDto& dto
)
:   Source(name),
    host(host),
    dtoConfig(dto)
{
    this->sourceElements.encoder = createEncoder(accelerator);
    this->sourceElements.source = createSrc(host);
    this->sourceElements.appsink = createAppsink();

    struct stat info;
    if (dtoConfig.cache_mode->contains("cont") &&            // Check if continous mode
        stat(dtoConfig.archive_path->c_str(), &info) != 0 && // Check if can access requested path
        info.st_mode & S_IFDIR)                              // Check if requested path is directory
    {
        this->sourceElements.tee = gst_element_factory_make("tee", "sourceSplitter");
        this->sourceElements.filesink = gst_element_factory_make("filesink", "achive");
    }
}
