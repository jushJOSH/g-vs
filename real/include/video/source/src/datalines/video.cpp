#include <video/source/datalines/video.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

// TODO pngenc добавить в конфиг
VideoLine::VideoLine(
    const std::string &encoder,
    Resolution resolution, 
    int fps, 
    int bitrate)
:   DataLine(DataLine::LineType::Video, encoder),
    videoconverter(gst_element_factory_make("videoconvert", str(format("%1%_videoconvert") % uuid).c_str())),
    videoscale(gst_element_factory_make("videoscale", str(format("%1%_videoscale") % uuid).c_str())),
    videorate(gst_element_factory_make("videorate", str(format("%1%_videorate") % uuid).c_str())),
    videoencoder(createEncoder())
{
    g_print("Created videoline %s\n", uuid.c_str());

    gst_bin_add_many(this->bin, 
                     this->videoconverter, 
                     this->videoscale, 
                     this->videorate, 
                     this->videoencoder, NULL);

    bool isLinkedOk = 
        gst_element_link(this->queue, this->videoconverter) &&
        gst_element_link(this->videoconverter, this->videoscale) &&
        gst_element_link(this->videoscale, this->videorate) &&
        gst_element_link(this->videorate, this->videoencoder);

    if (!isLinkedOk)
        throw std::runtime_error("Failed to create videoline");

    generateSrcPad();
}

GstElement* VideoLine::createEncoder() {
    std::string platform = "x";
    switch (Videoserver::accelerator) {
        case Videoserver::Accelerator::AMD:
            platform = "amfh";
        break;
        case Videoserver::Accelerator::NVIDIA:
            platform = "nvcudah";          
        break;
    }

    g_print("Desired encoder: %s\n", str(format("%1%%2%enc") % platform % encoder_s).c_str());
    
    return gst_element_factory_make(
        str(format("%1%%2%enc") % platform % encoder_s).c_str(),
        str(format("%1%_encoder") % uuid).c_str()
    );
}

VideoLine::Resolution VideoLine::strToResolution(const std::string &resolution_s, char separator) {
    VideoLine::Resolution resolution;
    std::vector<std::string> resolution_v;
    boost::split(resolution_v, resolution_s, boost::is_any_of(std::string(1, separator)));
    
    resolution.width = std::stoi(resolution_v[0]);
    resolution.height = std::stoi(resolution_v[1]);

    return resolution;
}

void VideoLine::generateSrcPad() const {
    auto staticPad = gst_element_get_static_pad(videoencoder, "src");
    auto ghostPad = gst_ghost_pad_new("src", staticPad);

    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(staticPad);
}

VideoLine::~VideoLine() {
    gst_element_send_event(GST_ELEMENT(this->bin), gst_event_new_eos());
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(this->bin, 
                        this->queue,
                        this->videoconverter, 
                        this->videoscale,
                        this->videorate,
                        this->videoencoder, NULL);
}

void VideoLine::updateBitrate(int bitrate) {
    g_object_set(videoencoder, "bitrate", bitrate, NULL);
}

void VideoLine::updateResolution(const std::string &resolution) {
    auto targetResolution = VideoLine::strToResolution(resolution);
    g_object_set(videoscale, "width", targetResolution.width, NULL);
    g_object_set(videoscale, "height", targetResolution.height, NULL);
}

void VideoLine::updateFramerate(int fps) {
    g_object_set(videorate, "max-rate", fps, NULL);
}
