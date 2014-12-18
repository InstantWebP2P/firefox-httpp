#include <gst/gst.h>
#include <glib.h>

static gboolean
bus_call(GstBus* bus, GstMessage* msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop*) data;
  switch (GST_MESSAGE_TYPE(msg)) {
  case GST_MESSAGE_EOS:
    g_print("End of Stream\n");
    g_main_loop_quit(loop);
    break;
  case GST_MESSAGE_ERROR: {
    gchar *debug;
    GError *error;
    gst_message_parse_error(msg, &error, &debug);
    g_free(debug);
    g_printerr("Error: %s\n", error->message);
    g_error_free(error);
    g_main_loop_quit(loop);
    break;
  }
  default:
    break;
  }
}

#define CHECK_OBJ(obj) do {if (!obj) { g_printerr("Object %s can't be created\n", #obj); return -1;}} while(0)

int main(int argc, char* argv[]) {
  GMainLoop * loop;
  GstElement *pipeline, *videosrc, *colorspace, *videoenc,
    *videoq, *audiosrc, *conv, *audioenc, *audioq, *muxer, *sink;
  GstBus *bus;
  GstCaps *videocap;
  // init
  gst_init(&argc, &argv);
  loop = g_main_loop_new(NULL, FALSE);
  // check input
  if (argc != 3) {
    g_printerr("Usage: %s <port number> <device>\n", argv[0]);
    return -1;
  }
  //create element
  pipeline   = gst_pipeline_new("audio-player");
  videosrc   = gst_element_factory_make("v4l2src", "videosrc");
  videocap   = gst_caps_new_simple("video/x-raw-yuv",
		  "width", G_TYPE_INT, 320,
  	      "height", G_TYPE_INT, 240,
  	      "framerate", GST_TYPE_FRACTION, 25, 1,
		  NULL);
  colorspace = gst_element_factory_make("ffmpegcolorspace", "colorspace");
  videoenc   = gst_element_factory_make("vp8enc", "videoenc");
  videoq     = gst_element_factory_make("queue2", "videoq");
  audiosrc   = gst_element_factory_make("autoaudiosrc", "audiosrc");
  conv       = gst_element_factory_make("audioconvert", "converter");
  audioenc   = gst_element_factory_make("vorbisenc", "audioenc");
  audioq     = gst_element_factory_make("queue2", "audioq");
  muxer      = gst_element_factory_make("webmmux", "mux");
  sink       = gst_element_factory_make("tcpclientsink", "sink");

  CHECK_OBJ(pipeline);
  CHECK_OBJ(videosrc);
  CHECK_OBJ(videocap);
  CHECK_OBJ(colorspace);
  CHECK_OBJ(videoenc);
  CHECK_OBJ(videoq);
  CHECK_OBJ(audiosrc);
  CHECK_OBJ(conv);
  CHECK_OBJ(audioenc);
  CHECK_OBJ(audioq);
  CHECK_OBJ(muxer);
  CHECK_OBJ(sink);

  //setup pipeline
  g_object_set(G_OBJECT(sink), "port", atoi(argv[1]), "host", "localhost", NULL);

  g_object_set(G_OBJECT(videosrc), "device", argv[2], NULL);
  g_object_set(G_OBJECT(videoenc), "speed", 2, NULL);
  g_object_set(G_OBJECT(muxer), "streamable", 1, NULL);

  bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
  gst_bus_add_watch(bus, bus_call, loop);
  gst_object_unref(bus);

  gst_bin_add_many(GST_BIN(pipeline),
    videosrc, colorspace, videoenc, videoq, audiosrc, conv,
    audioenc, audioq, muxer, sink, NULL);

  gst_element_link_filtered(videosrc, colorspace, videocap);
  gst_caps_unref(videocap);
  gst_element_link_many(colorspace, videoenc, videoq, muxer, NULL);
  gst_element_link_many(audiosrc, conv, audioenc, audioq, muxer, NULL);
  gst_element_link(muxer, sink);

  g_print("streaming to port %s\n", argv[1]);
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  g_print("running ... \n");
  g_main_loop_run(loop);
  //cleanup
  g_print("returned, stopping\n");
  gst_element_set_state(pipeline, GST_STATE_NULL);
  g_print("deleting pipeline\n");
  gst_object_unref(GST_OBJECT(pipeline));

  return 0;
}
