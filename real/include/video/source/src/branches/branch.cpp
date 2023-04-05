#include <video/source/branches/branch.hpp>

#include <video/source/datalines/audioline.hpp>
#include <video/source/datalines/videoline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

PipeBranch::PipeBranch(const std::string &sink, const std::string &muxer) 
:   uuid(boost::uuids::to_string(boost::uuids::random_generator_mt19937()())),
    multiqueue(gst_element_factory_make("multiqueue", str(format("%1%_multiqueue") % uuid).c_str())),
    muxer(gst_element_factory_make(muxer.c_str(), str(format("%1%_%2%") % uuid % muxer).c_str())),
    sink(gst_element_factory_make(sink.c_str(), str(format("%1%_%2%") % uuid % sink).c_str()))
{   
    g_print("Branch created %s\n", uuid.c_str());
    
    g_signal_connect(multiqueue, "pad-added", G_CALLBACK(onNewPad),
        this->muxer ? this->muxer : this->sink);
}

PipeBranch::PipeBranch(GstBin* bin, const std::string &sink, const std::string &muxer) 
:   PipeBranch(sink, muxer)
{
    if (!loadBin(bin))
        throw std::runtime_error("PipeBranch: failed to link queue and decodebin");
}

void PipeBranch::onNewPad(GstElement* element, GstPad* newSrcPad, GstElement* nextElement) {
    g_print ("PipeBranch: Received new pad '%s'\n", GST_PAD_NAME (newSrcPad));
    
    // Check new pad type
    GstCaps *pad_caps = gst_pad_get_current_caps (newSrcPad);
    GstStructure *pad_struct = gst_caps_get_structure (pad_caps, 0);
    const gchar *pad_type = gst_structure_get_name (pad_struct);
    
    GstElementFactory *factory = gst_element_get_factory(nextElement);
    const gchar *element_name = gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(factory));

    GstPad* newSinkPad = nullptr;
    if (g_str_has_suffix(element_name, "mux"))
        newSinkPad = gst_element_get_request_pad(nextElement, "sink_%u");

    else if (g_str_has_suffix(element_name, "sink")) 
        newSinkPad = gst_element_get_static_pad(nextElement, "sink");
        
    else {
        g_print("PipeBranch: unsupported nextElement type. Expected mux or sink");
        return;
    }    

    if (gst_pad_is_linked(newSinkPad)) 
        return;

    // Linking pad
    if (gst_pad_link(newSrcPad, newSinkPad)) {
        g_print ("Type is '%s' but link failed.\n", pad_type);
        return;
    } else g_print ("Link succeeded (type '%s').\n", pad_type);

    // Unreference the new pad's caps, if we got them
    if (pad_caps != NULL) gst_caps_unref (pad_caps);
}

GstElement* PipeBranch::getQueue() const {
    return this->multiqueue;
}

GstElement* PipeBranch::getSink() const {
    return this->sink;
}

GstPad* PipeBranch::getNewPad() {
    auto newQueuePad = gst_element_get_request_pad(multiqueue, "sink_%u");
}

void PipeBranch::setMuxer(GstElement* muxer) {
    this->muxer = muxer;
}

void PipeBranch::setSink(GstElement* sink) {
    this->sink = sink;
}

bool PipeBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(bin, multiqueue, sink, NULL);
    if (this->muxer) gst_bin_add(bin, muxer);
    
    return 
        this->muxer ? gst_element_link(muxer, sink) : true;
}

void PipeBranch::unloadBin() {
    gst_bin_remove_many(bin, multiqueue, sink, NULL);
    if (this->muxer) gst_bin_remove(bin, muxer);
    bin = nullptr;
}

GstBin*  PipeBranch::getBin() const {
    return bin;
}

GstStateChangeReturn PipeBranch::setState(GstState state) {
    return gst_element_set_state(multiqueue, state);
}

std::string PipeBranch::getUUID() const {
    return uuid;
}