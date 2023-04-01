#include <video/source/branches/stream.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void StreamBranch::updateEncoder(int bitrate) {
    GstCaps* encoder_caps = gst_caps_new_simple(str(format("video/x-%1%") % dtoConfig.encoding->c_str()).c_str(),
        "bitrate", G_TYPE_INT, bitrate,
        NULL
    );
    g_object_set(videoencoder, "caps", encoder_caps, NULL);
}

GstElement* StreamBranch::createEncoder(int accelerator) {
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

StreamBranch::StreamBranch(const SourceConfigDto& config) 
:   PipeBranch(config)
{
    this->videoencoder = createEncoder((int)Videoserver::accelerator);
    this->sink = gst_element_factory_make("appsink", str(format("%1%_sink") % uuid).c_str());
} 

StreamBranch::~StreamBranch() {
    // Sending eos so it can properly save videofile
    gst_element_send_event(
        queue,
        gst_event_new_eos()
    );
}