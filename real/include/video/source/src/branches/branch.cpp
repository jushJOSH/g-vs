#include <video/source/branches/branch.hpp>

#include <video/source/datalines/audioline.hpp>
#include <video/source/datalines/videoline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <algorithm/string.hpp>

using boost::format;
using boost::str;

PipeBranch::PipeBranch(const SourceConfigDto &config) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    config(config),
    queue(gst_element_factory_make("queue", str(format("%1%_queue") % uuid).c_str())),
    decodebin(gst_element_factory_make("decodebin", str(format("%1%_decodebin") % uuid).c_str()))
{
    g_print("Branch created %s\n", uuid);

    if (!gst_element_link(queue, decodebin))
        throw std::runtime_error("PipeBranch: failed to link queue and decodebin");

    datalines->bin = this->bin;
    datalines->dtoConfig = this->config;

    // Decode bin will send signals like 'New video pad' or 'New audio pad'
    g_signal_connect(decodebin, "pad-added", G_CALLBACK (onNewPad), datalines.get());
}

void PipeBranch::onNewPad(GstElement* element, GstPad* newPad, gpointer userData) {
    g_print ("Received new pad '%s'\n", GST_PAD_NAME (newPad));
    auto linedata = (DataLines*)userData;

    // New data line for video or audio
    std::shared_ptr<DataLine> newLine = nullptr;

    // Check new pad type
    GstCaps *pad_caps = gst_pad_get_current_caps (newPad);
    GstStructure *pad_struct = gst_caps_get_structure (pad_caps, 0);
    const gchar *pad_type = gst_structure_get_name (pad_struct);
    if (g_str_has_prefix (pad_type, "audio/x-raw")) {
        newLine = std::make_shared<AudioLine>(
            linedata->bin,
            linedata->dtoConfig.audioencoding,
            linedata->dtoConfig.quality,
            linedata->dtoConfig.volume);
        std::reinterpret_pointer_cast<AudioLine>(newLine)->mute(linedata->dtoConfig.mute);
    }
    else if (g_str_has_prefix(pad_type, "video/x-raw")) {
        newLine = std::make_shared<VideoLine>(
            linedata->bin,
            linedata->dtoConfig.videoencoding,
            VideoLine::strToResolution(linedata->dtoConfig.resolution),
            linedata->dtoConfig.fps,
            linedata->dtoConfig.bitrate
        );
    }
    else return;

    // Linking pad
    if (newLine->attachToPipeline(newPad)) {
        g_print ("Type is '%s' but link failed.\n", pad_type);
        return;
    } else {
        g_print ("Link succeeded (type '%s').\n", pad_type);
    }

    linedata->datalines.push_back(newLine);
    // Unreference the new pad's caps, if we got them
    if (pad_caps != NULL)
        gst_caps_unref (pad_caps);
}

GstElement* PipeBranch::getQueue() const {
    return this->queue;
}