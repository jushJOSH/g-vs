#include <video/source/pipetree.hpp>

#include <video/source/datalines/audioline.hpp>
#include <video/source/datalines/videoline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>

using boost::format;
using boost::str;

// GstPadProbeReturn PipeTree::eventProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
//     if (GST_EVENT_TYPE (GST_PAD_PROBE_INFO_DATA (info)) != GST_EVENT_EOS)
//         return GST_PAD_PROBE_OK;
    
//     g_print("Entered eos probe call\n");

//     PipeBranch* userBranch = (PipeBranch*)user_data;

//     gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
//     userBranch->unloadBin();

//     return GST_PAD_PROBE_DROP;
// }

// GstPadProbeReturn PipeTree::padProbeCallback(GstPad* pad, GstPadProbeInfo *info, gpointer user_data) {
//     g_print("Entered pad probe call\n");
    
//     PipeBranch* userBranch = (PipeBranch*)user_data;
//     GstPad *src, *sink;

//     // Removing probe
//     gst_pad_remove_probe(pad, GST_PAD_PROBE_INFO_ID(info));
    
//     // Setting up new probe for EOS event
//     src = gst_element_get_static_pad (userBranch->getQueue(), "src");
//     gst_pad_add_probe (src, 
//         (GstPadProbeType)(GST_PAD_PROBE_TYPE_BLOCK | GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM),
//         eventProbeCallback, user_data, NULL);
//     gst_object_unref (src);

//     sink = gst_element_get_static_pad (userBranch->getQueue(), "sink");
//     gst_pad_send_event (sink, gst_event_new_eos ());
//     gst_object_unref (sink);

//     return GST_PAD_PROBE_OK;
// }


void PipeTree::onErrorCallback(GstBus *bus, GstMessage *msg, gpointer data) {
    GError *err;
    gchar *debug_info;

    gst_message_parse_error (msg, &err, &debug_info);
    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error (&err);
    g_free (debug_info);
}

int PipeTree::manageBranchQueue(PadInfo& data) {
    g_print("PipeTree: Started queue management\n");

    int successfulCount = 0;
    while (!data.branchQueue.empty()) {
        auto branch = data.branchQueue.front();
        data.branchQueue.pop();

        bool loadResult = branch->loadBin(data.bin);
        if (!loadResult)
            continue;
        
        for (auto &dataline : data.datalines) {
            auto newTeePad = gst_element_get_request_pad(dataline->getTee(), "src_%u");
            auto newBranchPad = branch->getNewPad(dataline->getType());

            g_print("Branch type : %s\n", typeid(branch).name());

            if (gst_pad_is_linked(newBranchPad)) break;

            auto linkResult = gst_pad_link(newTeePad, newBranchPad);
            if (linkResult != GstPadLinkReturn::GST_PAD_LINK_OK)
                continue;
        }

        successfulCount++;
        branch->setState(data.currentState);
        data.branches[branch->getUUID()] = branch;
    }

    return successfulCount;
}

void PipeTree::onNoMorePads(GstElement* src, PadInfo* data) {
    data->noMorePads = true;
    manageBranchQueue(*data);
}

void PipeTree::onNewPad(GstElement* element, GstPad* newPad, PadInfo* userData) {
    g_print ("PipeBranch: Received new pad '%s'\n", GST_PAD_NAME (newPad));

    // New data line for video or audio
    std::shared_ptr<DataLine> newLine;

    // Check new pad type
    GstCaps *pad_caps = gst_pad_get_current_caps (newPad);
    GstStructure *pad_struct = gst_caps_get_structure (pad_caps, 0);
    const gchar *pad_type = gst_structure_get_name (pad_struct);
    if (g_str_has_prefix (pad_type, "audio/x-raw")) {
        newLine = std::make_shared<AudioLine>(
            userData->bin,
            userData->config.audioencoding,
            userData->config.quality,
            userData->config.volume);
        g_object_set(std::reinterpret_pointer_cast<AudioLine>(newLine)->getVolume(), "mute", userData->config.mute, NULL);
    }
    else if (g_str_has_prefix(pad_type, "video/x-raw")) {
        newLine = std::make_shared<VideoLine>(
            userData->bin,
            userData->config.videoencoding,
            VideoLine::strToResolution(userData->config.resolution),
            userData->config.fps,
            userData->config.bitrate
        );
    }
    else return;

    // Linking pad
    if (newLine->attachToPipeline(newPad) != GstPadLinkReturn::GST_PAD_LINK_OK) {
        g_print ("Type is '%s' but link failed.\n", pad_type);
        return;
    } else g_print ("Link succeeded (type '%s').\n", pad_type);

    userData->datalines.push_back(newLine);
    // Unreference the new pad's caps, if we got them
    if (pad_caps != NULL)
        gst_caps_unref (pad_caps);
}

PipeTree::PipeTree() 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    pipeline(gst_pipeline_new(uuid.c_str()))
{
    g_print("Created tree %s\n", uuid.c_str());
    
    // Setting bin for padinfo data
    padinfo.bin = GST_BIN(pipeline);

    // Creating error callback from pipeline bus
    auto bus = gst_element_get_bus (pipeline);
    gst_bus_add_signal_watch (bus);
    g_signal_connect (G_OBJECT (bus), "message::error", G_CALLBACK(onErrorCallback), nullptr);
    gst_object_unref (bus);
}

PipeTree::PipeTree(const std::string& source)
:   PipeTree()
{
    setSource(source);
}

PipeTree::~PipeTree() {
    g_print("Deleted tree %s\n", uuid.c_str());

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}

// All branches must be in queue
// Queue will be solved as soon as noMorePads signal appears
void PipeTree::addBranch(std::shared_ptr<PipeBranch> branch) {
    padinfo.branchQueue.push(branch);

    // If add branch after pad init
    // then add them asap
    if (padinfo.noMorePads)
        manageBranchQueue(padinfo);
}

void PipeTree::removeBranch(const std::string& name) {
    if (padinfo.branches.contains(name)) 
        padinfo.branches.erase(name);
}

GstElement* PipeTree::getSink(const std::string &name) {
    return padinfo.branches.contains(name) 
        ? padinfo.branches[name]->getSink() 
        : nullptr;
}

void PipeTree::setSource(const std::string &source) {
    this->source = gst_element_factory_make("uridecodebin", str(format("%1%_uridecodebin") % uuid).c_str());
    gst_bin_add(GST_BIN(pipeline), this->source);
    g_object_set(this->source, "uri", source.c_str(), NULL);

    g_signal_connect(this->source, "pad-added", G_CALLBACK(onNewPad), &padinfo);
    g_signal_connect(this->source, "no-more-pads", G_CALLBACK(onNoMorePads), &padinfo);
}

GstStateChangeReturn PipeTree::setState(GstState state) {
    auto stateResult = gst_element_set_state(pipeline, state);
    if (stateResult != GstStateChangeReturn::GST_STATE_CHANGE_FAILURE)
        padinfo.currentState = state;

    return stateResult;
}

void PipeTree::updateBitrate(int bitrate) {
    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto encoder = std::reinterpret_pointer_cast<VideoLine>(dataline)->getEncoder();
            g_object_set(encoder, "bitrate", bitrate, NULL);
        }
    }
}

void PipeTree::updateResolution(const std::string resolution) {
    auto targetResolution = VideoLine::strToResolution(resolution);

    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto scale = std::reinterpret_pointer_cast<VideoLine>(dataline)->getScale();
            g_object_set(scale, "width", targetResolution.width, NULL);
            g_object_set(scale, "height", targetResolution.height, NULL);
        }
    }
}

void PipeTree::updateFrameRate(int fps) {
    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Video) {
            auto rate = std::reinterpret_pointer_cast<VideoLine>(dataline)->getRate();
            g_object_set(rate, "max-rate", fps, NULL);
        }
    }
}

void PipeTree::updateAudioQuality(double quality) {
    if (quality > 1.0) quality = 1.0;
    else if (quality < 0.0) quality = 0.0;

    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto encoder = std::reinterpret_pointer_cast<AudioLine>(dataline)->getEncoder();
            g_object_set(encoder, "quality", quality, NULL);
        }
    }
}

void PipeTree::updateAudioVolume(double volume) {
    if (volume > 1.0) volume = 1.0;
    else if (volume < 0.0) volume = 0.0;

    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto volume_g = std::reinterpret_pointer_cast<AudioLine>(dataline)->getVolume();
            g_object_set(volume_g, "volume", volume, NULL);
        }
    }
}

void PipeTree::mute(bool state) {
    for (auto &dataline : padinfo.datalines) {
        if (dataline->getType() == DataLine::LineType::Audio) {
            auto volume = std::reinterpret_pointer_cast<AudioLine>(dataline)->getVolume();
            g_object_set(volume, "mute", state, NULL);
        }
    }
}

void PipeTree::setConfig(SourceConfigDto& config) {
    padinfo.config = config;
}