#include <video/source/branches/archive.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/format.hpp>
#include <algorithm/string.hpp>

using boost::format;
using boost::str;

ArchiveBranch::ArchiveBranch(const SourceConfigDto& config)
:   PipeBranch(config),
    muxer(gst_element_factory_make("mp4mux", str(format("%1%_mp4mux") % uuid).c_str())),
    sink(gst_element_factory_make("filesink", str(format("%1%_filesink") % uuid).c_str()))
{
    g_print("Created archive branch %s\n", uuid.c_str());

    g_object_set(sink, "location", (datalines->dtoConfig.archive_path + "test.mp4")->c_str());

    // Link all branch datalines into single muxer
    bool isLinkedOk = true;
    for (auto& dataline : datalines->datalines)
        isLinkedOk = isLinkedOk && gst_element_link(dataline->getEncoder(), this->muxer);
    
    if (!isLinkedOk)
        throw std::runtime_error("Archive Branch: error on linking datalines");

    gst_element_link(muxer, sink);
}

GstStateChangeReturn ArchiveBranch::setState(GstState state) {
    return gst_element_set_state(queue, state);
}