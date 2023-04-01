#include <video/source/branches/archive.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void ArchiveBranch::updateEncoder(int bitrate) {
    GstCaps* encoder_caps = gst_caps_new_simple(str(format("video/x-%1%") % dtoConfig.encoding->c_str()).c_str(),
        "bitrate", G_TYPE_INT, bitrate,
        NULL
    );
    g_object_set(videoencoder, "caps", encoder_caps, NULL);
}

GstElement* ArchiveBranch::createEncoder(int accelerator) {
    std::string platform = "";
    Videoserver::Accelerator v_accelerator = (Videoserver::Accelerator)accelerator;
    switch (v_accelerator) {
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
        str(format("%1%%2%enc") % platform % dtoConfig.encoding->c_str()).c_str(),
        str(format("%1%_encoder") % uuid).c_str()
    );
}

ArchiveBranch::ArchiveBranch(const SourceConfigDto& config) 
:   PipeBranch(config)
{
    this->sink = gst_element_factory_make("filesink", str(format("%1%_sink") % uuid).c_str());
    this->muxer = gst_element_factory_make("mp4mux", str(format("%1%_muxer") % uuid).c_str());
    this->videoencoder = createEncoder((int)Videoserver::accelerator);

    g_object_set(sink, "location", dtoConfig.archive_path->c_str() ,NULL);
}

bool ArchiveBranch::loadBin(GstBin* bin) {
    this->bin = bin;

    gst_bin_add_many(bin, queue, videoscale, videorate, videoencoder, muxer, sink, NULL);
    return gst_element_link_many(queue, videoscale, videorate, videoencoder, muxer, sink, NULL);
}

void ArchiveBranch::unloadBin() {
    gst_bin_remove_many(bin, queue, videoscale, videorate, videoencoder, muxer, sink, NULL);
}

ArchiveBranch::~ArchiveBranch() {
    // Sending eos so it can properly save videofile
    gst_element_send_event(
        queue,
        gst_event_new_eos()
    );

    gst_bin_remove(bin, muxer);
}