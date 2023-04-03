#include <video/source/branches/screenshot.hpp>
#include <video/source/datalines/videoline.hpp>
#include <video/source/datalines/audioline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void ScreenshotBranch::initPadEvent() {
    PipeBranch::initPadEvent();
    
    this->datalines->nextelement = this->sink;

    // if all important objects is set
    if (datalines->nextelement && datalines->bin) 
        // Decode bin will send signals like 'New video pad' or 'New audio pad'
        g_signal_connect(decodebin, "pad-added", G_CALLBACK (PipeBranch::onNewPad), datalines.get());
}

ScreenshotBranch::ScreenshotBranch(const SourceConfigDto& config)
:   PipeBranch(
        config,
        "",
        "filesink"
    )
{
    g_print("Created stream branch %s\n", uuid.c_str());
}

bool ScreenshotBranch::loadBin(GstBin *bin) {
    if (this->bin) return true;

    gst_bin_add_many(bin, queue, decodebin, sink, NULL);
    
    return 
        gst_element_link(queue, decodebin);
}

void ScreenshotBranch::unloadBin() {
    gst_bin_remove_many(bin, queue, decodebin, muxer, sink, NULL);
    bin = nullptr;
}