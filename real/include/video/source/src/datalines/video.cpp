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
}

VideoLine::VideoLine(
    GstBin* bin,
    const std::string &encoder,
    Resolution resolution, 
    int fps, 
    int bitrate)
:   VideoLine(encoder, resolution, fps, bitrate)
{
    loadBin(bin);

    g_print("%s - linking elements together!\n", uuid.c_str());
    bool isLinkedOk = 
        gst_element_link(this->queue, this->videoconverter) &&
        gst_element_link(this->videoconverter, this->videoscale) &&
        gst_element_link(this->videoscale, this->videorate) &&
        gst_element_link(this->videorate, this->videoencoder) &&
        gst_element_link(this->videoencoder, this->tee);

    if (!isLinkedOk) 
        throw std::runtime_error("VideoLine: Failed creation of videoline for some reason\n");
}

void VideoLine::loadBin(GstBin* bin) {
    g_print("%s - loading BIN\n", uuid.c_str());
    if (this->bin != nullptr) return;
    
    this->bin = bin;
    gst_bin_add_many(this->bin, 
                     this->queue,
                     this->videoconverter, 
                     this->videoscale, 
                     this->videorate, 
                     this->videoencoder, 
                     this->tee, NULL);
}

void VideoLine::unloadBin() {
    gst_element_send_event(this->queue, gst_event_new_eos());
    gst_bin_remove_many(this->bin, 
                        this->queue,
                        this->videoconverter, 
                        this->videoscale,
                        this->videorate,
                        this->videoencoder,
                        this->tee, NULL);
}

GstElement* VideoLine::getEncoder() const {
    return this->videoencoder;
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

GstElement* VideoLine::getRate() const {
    return this->videorate;
}

GstElement* VideoLine::getScale() const {
    return this->videoscale;
}

bool VideoLine::attachToPipeline(GstElement* before) {
    return gst_element_link(before, this->queue);
}

GstPadLinkReturn VideoLine::attachToPipeline(GstPad* before) {
    g_print("VideoLine: Attaching to pipeline\n");

    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return gst_pad_link(before, queueSink);
}

void VideoLine::detachFromPipeline(GstElement* before) {
    gst_element_unlink(before, this->queue);
}

bool VideoLine::detachFromPipeline(GstPad* before) {
    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return gst_pad_unlink(before, queueSink);
}

GstElement* VideoLine::getFirstElement() const {
    return this->queue;
}