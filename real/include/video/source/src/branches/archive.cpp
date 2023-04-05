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

ArchiveBranch::ArchiveBranch(const std::string &path)
:   PipeBranch(
        "filesink",
        "mp4mux"
    )
{
    g_print("Created archive branch %s\n", uuid.c_str());
    g_object_set(sink, "location", path.c_str(), NULL);
}

bool ArchiveBranch::loadBin(GstBin *bin) {
    if (!this->bin)
        this->bin = bin;

    gst_bin_add_many(bin, multiqueue, muxer, sink, NULL);
    
    return gst_element_link(muxer, sink);
}

void ArchiveBranch::unloadBin() {
    gst_bin_remove_many(bin, multiqueue, muxer, sink, NULL);
    bin = nullptr;
}