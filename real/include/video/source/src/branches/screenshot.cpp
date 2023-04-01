#include <video/source/branches/screenshot.hpp>
#include <video/videoserver/videoserver.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::format;
using boost::str;

ScreenshotBranch::ScreenshotBranch(const SourceConfigDto& config)
:   PipeBranch(config)
{
    this->sink = gst_element_factory_make("filesink", str(format("%1%_sink") % uuid).c_str());
    this->videoencoder = gst_element_factory_make("pngenc", str(format("%1%_encoder") % uuid).c_str());

    g_object_set(sink, "location", dtoConfig.archive_path->c_str(), NULL);
    g_object_set(videoencoder, "snapshot", true, NULL);
}

ScreenshotBranch::~ScreenshotBranch() {
    // Sending eos so it can properly save videofile
    gst_element_send_event(
        queue,
        gst_event_new_eos()
    );
}