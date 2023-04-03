#include <video/source/branches/stream.hpp>
#include <video/source/datalines/videoline.hpp>
#include <video/source/datalines/audioline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void StreamBranch::initPadEvent() {
    PipeBranch::initPadEvent();
    
    this->datalines->nextelement = this->muxer;

    // if all important objects is set
    if (datalines->nextelement && datalines->bin) 
        // Decode bin will send signals like 'New video pad' or 'New audio pad'
        g_signal_connect(decodebin, "pad-added", G_CALLBACK (PipeBranch::onNewPad), datalines.get());
}

StreamBranch::StreamBranch(const SourceConfigDto& config)
:   PipeBranch(
        config,
        "multipartmux",
        "appsink"
    )
{
    g_print("Created stream branch %s\n", uuid.c_str());
}