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

ScreenshotBranch::ScreenshotBranch(const std::string &path)
:   PipeBranch(
        "filesink"
    )
{
    g_print("Created screenshot branch %s\n", uuid.c_str());
    g_object_set(sink, "location", path.c_str(), NULL);
}

bool ScreenshotBranch::loadBin(GstBin *bin) {
    if (!this->bin) 
        this->bin = bin;

    gst_bin_add_many(bin, multiqueue, sink, NULL);
    
    return 
        true;
}

void ScreenshotBranch::unloadBin() {
    gst_bin_remove_many(bin, multiqueue, sink, NULL);
    bin = nullptr;
}