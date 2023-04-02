#include <video/source/datalines/audioline.hpp>

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
}

AudioLine::AudioLine(
    GstBin* bin,
    const std::string& encoder, 
    double quality,
    double volume)
:   AudioLine(encoder, quality, volume)
{
    loadBin(bin);

    bool isLinkedOk = 
        gst_element_link(this->queue, this->audioconverter) &&
        gst_element_link(this->audioconverter, this->volume) &&
        gst_element_link(this->volume, this->audioencoder);

    if (!isLinkedOk) 
        throw std::runtime_error("AudioLine: Failed creation of audioline for some reason\n");
}

GstElement* AudioLine::createEncoder() {
    auto encodertype = str(format("%1%enc") % encoder_s);
    auto encodername = str(format("%1%_%2%") % uuid % encodertype);

    return gst_element_factory_make(encodername.c_str(), encodername.c_str());
}

void AudioLine::loadBin(GstBin* bin) {
    if (this->bin != nullptr) return;
    
    this->bin = bin;
    gst_bin_add_many(this->bin, this->queue, this->audioconverter, this->volume, this->audioencoder, NULL);
}

void AudioLine::unloadBin() {
    gst_element_send_event(this->queue, gst_event_new_eos());
    gst_bin_remove_many(this->bin, this->queue, this->audioconverter, this->volume, this->audioencoder, NULL);
}

GstElement* AudioLine::getEncoder() const {
    return this->audioencoder;
}

void AudioLine::updateVolume(double volume) {
    g_object_set(this->volume, "volume", volume, NULL);
}

void AudioLine::updateQuality(double quality) {
    g_object_set(audioconverter, "quality", quality, NULL);
}

void AudioLine::mute(bool state) {
    g_object_set(this->volume, "mute", state, NULL);
}