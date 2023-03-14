#include <gst/gst.h>
#include <algorithm>
#include "pipeline.hpp"
#include "bus.hpp"
#include "brick.hpp"
#include <videoserver.hpp>

#include <crow.h>

int
tutorial_main (int argc, char *argv[])
{
  GstMessage *msg;

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the empty pipeline */
  auto src = GBrick("videotestsrc", "source");
  auto filter = GBrick("vertigotv", "filter");
  auto converter = GBrick("videoconvert", "converter");
  auto sink = GBrick("fpsdisplaysink", "sink");

  auto bricks = {
    src, filter, converter, sink
  };

  if (!src || !filter || !converter || !sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }
  GPipeline pipe("test-pipeline");
  auto failed = pipe.addToPipelineMany(bricks);
  if (gst_element_link_many (src, filter, converter, sink, NULL) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    return -1;
  }

  /* Modify the source's properties */
  g_object_set (src, "pattern", 0, NULL);

  /* Start playing */
  auto result = pipe.updateState(GST_STATE_PLAYING);
  if (result == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    return -1;
  }

  /* Wait until error or EOS */

  auto &bus = pipe.getBus();
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      (GstMessageType)((int)GST_MESSAGE_ERROR | (int)GST_MESSAGE_EOS));

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
            debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }

  return 0;
}

int main() {
  Videoserver server("127.0.0.1", 1337);
}
