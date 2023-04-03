#include <video/source/branches/archive.hpp>
#include <video/source/datalines/videoline.hpp>
#include <video/source/datalines/audioline.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

void ArchiveBranch::initPadEvent() {
    PipeBranch::initPadEvent();
    
    this->datalines->nextelement = this->muxer;

    // if all important objects is set
    if (datalines->nextelement && datalines->bin) 
        // Decode bin will send signals like 'New video pad' or 'New audio pad'
        g_signal_connect(decodebin, "pad-added", G_CALLBACK (PipeBranch::onNewPad), datalines.get());
}

ArchiveBranch::ArchiveBranch(const SourceConfigDto& config)
:   PipeBranch(
        config,
        "mp4mux", 
        "filesink"
    )
{
    g_print("Created archive branch %s\n", uuid.c_str());
    std::string location_s = datalines->dtoConfig.archive_path.getValue("") + "test.mp4";
    g_object_set(sink, "location", location_s.c_str(), NULL);

    // Link all branch datalines into single muxer
    bool isLinkedOk = true;
    for (auto& dataline : datalines->datalines)
        isLinkedOk = isLinkedOk && gst_element_link(dataline->getEncoder(), this->muxer);
    
    if (!isLinkedOk)
        throw std::runtime_error("Archive Branch: error on linking datalines");

    gst_element_link(muxer, sink);
}