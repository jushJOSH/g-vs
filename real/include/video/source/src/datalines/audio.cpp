#include <video/source/datalines/audio.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

#include <oatpp/core/base/Environment.hpp>

using boost::format;
using boost::str;

AudioLine::AudioLine(
    const std::string& encoder,
    double quality,
    double volume)

:   DataLine(DataLine::LineType::Audio, encoder),
    audioconverter(gst_element_factory_make("audioconvert", str(format("%1%_audioconvert") % uuid).c_str())),
    volume(gst_element_factory_make("volume", str(format("%1%_volume") % uuid).c_str()))
{
    this->encoder = gst_element_factory_make("avenc_aac", str(format("%1%_avenc_aac") % uuid).c_str());
    OATPP_LOGD("AudioLine", "Created audioline %s", uuid.c_str());

    gst_bin_add_many(this->bin, 
                     this->audioconverter, 
                     this->audioconverter, 
                     this->volume, 
                     this->encoder, NULL);

    bool isLinkedOk = 
        gst_element_link(this->queue, this->audioconverter) &&
        gst_element_link(this->audioconverter, this->volume) &&
        gst_element_link(this->volume, this->encoder);

    if (!isLinkedOk)
        throw std::runtime_error("Failed to create audioline");

    generateSrcPad();
}

GstElement* AudioLine::createEncoder() {
    auto encodertype = str(format("%1%enc") % encoder_s);
    auto encodername = str(format("%1%_%2%") % uuid % encodertype);

    return gst_element_factory_make(encodername.c_str(), encodername.c_str());
}

void AudioLine::generateSrcPad() const {
    auto staticPad = gst_element_get_static_pad(encoder, "src");
    auto ghostPad = gst_ghost_pad_new("src", staticPad);
    gst_element_add_pad(GST_ELEMENT(bin), ghostPad);
    g_object_unref(staticPad);
}

AudioLine::~AudioLine() {
    OATPP_LOGD("AudioLine", "Destroyed one");

    gst_element_send_event(GST_ELEMENT(this->bin), gst_event_new_eos());
    gst_element_set_state(GST_ELEMENT(bin), GST_STATE_NULL);
    gst_bin_remove_many(this->bin,
                        this->queue, 
                        this->audioconverter, 
                        this->volume, 
                        this->encoder, NULL);

    auto padParent = gst_pad_get_parent_element(previousPad);
    gst_element_release_request_pad(padParent, previousPad);
}

void AudioLine::updateVolume(double volume) {
    if (volume > 1.0) volume = 1.0;
    else if (volume < 0.0) volume = 0.0;

    g_object_set(this->volume, "volume", volume, NULL);
}

void AudioLine::updateQuality(double quality) {
    if (quality > 1.0) quality = 1.0;
    else if (quality < 0.0) quality = 0.0;

    g_object_set(encoder, "quality", quality, NULL);
}

void AudioLine::mute(bool mute) {
    g_object_set(volume, "mute", mute, NULL);
}
