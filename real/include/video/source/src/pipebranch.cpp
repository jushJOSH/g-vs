#include <video/source/pipebranch.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::str;

GstElement* PipeBranch::createEncoder(Videoserver::Accelerator accelerator) {
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
        str(boost::format("%1%%2%enc") % platform % dtoConfig.encoding).c_str(),
        str(format("%1%_encoder") % uuid).c_str()
    );
}

PipeBranch::PipeBranch(const std::string &sink, const SourceConfigDto &config) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    dtoConfig(config),

    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_sink") % uuid).c_str())),
    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    videorate(gst_element_factory_make("videorate", str(format("%1%_videorate") % uuid).c_str())),
    videoscale(gst_element_factory_make("videoscale", str(format("%1%_videoscale") % uuid).c_str())),
    videoencoder(createEncoder(Videoserver::accelerator))
{}

bool PipeBranch::loadBin(GstBin* bin) {
    gst_bin_add_many(bin, queue, videoscale, videorate, videoencoder, sink, NULL);
    return gst_element_link_many(queue, videoscale, videorate, videoencoder, sink, NULL);
}