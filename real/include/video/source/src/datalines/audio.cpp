#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::str;

AudioLine::AudioLine(
    const std::string& encoder,
    double quality,
    double volume)

:   DataLine(DataLine::LineType::Audio, encoder),
    audioconverter(gst_element_factory_make("audioconvert", str(format("%1%_audioconvert") % uuid).c_str())),
    volume(gst_element_factory_make("volume", str(format("%1%_volume") % uuid).c_str())),
    audioencoder(gst_element_factory_make("vorbisenc", str(format("%1%_vorbisenc") % uuid).c_str()))
{
    g_print("Created audioline %s\n", uuid.c_str());

    if (!loadBin()) 
        throw std::runtime_error("AudioLine: Failed creation of audioline for some reason\n");
}

GstElement* AudioLine::createEncoder() {
    auto encodertype = str(format("%1%enc") % encoder_s);
    auto encodername = str(format("%1%_%2%") % uuid % encodertype);

    return gst_element_factory_make(encodername.c_str(), encodername.c_str());
}

bool AudioLine::loadBin() {
    gst_bin_add_many(this->bin, 
                     this->queue,
                     this->audioconverter, 
                     this->audioconverter, 
                     this->volume, 
                     this->audioencoder, NULL);

    bool isLinkedOk = 
        gst_element_link(this->queue, this->audioconverter) &&
        gst_element_link(this->audioconverter, this->volume) &&
        gst_element_link(this->volume, this->audioencoder);

    return isLinkedOk;
}

void AudioLine::unloadBin() {
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(this->bin,
                        this->queue, 
                        this->audioconverter, 
                        this->volume, 
                        this->audioencoder, NULL);
}

GstElement* AudioLine::getEncoder() const {
    return this->audioencoder;
}

GstElement* AudioLine::getVolume() const {
    return volume;
}

bool AudioLine::attachToPipeline(GstElement* before) {
    return 
        gst_element_link(before, this->queue);
}

GstPadLinkReturn AudioLine::attachToPipeline(GstPad* before) {
    g_print("AudioLine: Attaching to pipeline\n");

    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return gst_pad_link(before, queueSink);
}

void AudioLine::detachFromPipeline(GstElement* before) {
    gst_element_unlink(before, this->queue);
}

bool AudioLine::detachFromPipeline(GstPad* before) {
    auto queueSink = gst_element_get_static_pad(this->queue, "sink");
    return 
        gst_pad_unlink(before, queueSink);
}

GstElement* AudioLine::getFirstElement() const {
    return this->queue;
}

GstElement *AudioLine::getLastElement() const {
    return this->audioencoder;
}

GstPad* AudioLine::generateSrcPad() {
    auto staticPad = gst_element_get_static_pad(audioencoder, "src");
    auto ghostPad = gst_ghost_pad_new("src", staticPad);
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(staticPad);

    return gst_element_get_static_pad(GST_ELEMENT(bin), "src");
}

AudioLine::~AudioLine() {
    gst_element_send_event(GST_ELEMENT(this->bin), gst_event_new_eos());
    unloadBin();
}