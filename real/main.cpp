#include <gst/gst.h>
#include <glib.h>
#include <gst/app/app.h>

#include <iostream>

int totalsamples = 0;
static GstFlowReturn on_new_sample(GstElement* appsink, gpointer data) {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));
    GstBuffer *buffer = gst_sample_get_buffer(sample);

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

int main(int argc, char *argv[]) {
  GstElement *pipeline, *src, *appsink, *convert;
  GMainLoop *loop;

  /* Инициализация GStreamer */
  gst_init(&argc, &argv);

  /* Создание элементов */
  pipeline = gst_pipeline_new("pipeline");
  src = gst_element_factory_make("videotestsrc", "src");
  convert = gst_element_factory_make("videoconvert", "convert");
  appsink = gst_element_factory_make("appsink", "sink");

  /* Добавление элементов в пайплайн */
  gst_bin_add_many(GST_BIN(pipeline), src, appsink, convert, NULL);

  /* Установка свойств элемента appsink */
  g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, NULL);

  /* Подключение обработчика сигнала new-sample */
  g_signal_connect(appsink, "new-sample", G_CALLBACK(on_new_sample), NULL);

  /* Установка связей между элементами */
  if (!gst_element_link_many(src, convert, appsink, NULL))
  {
    std::cout << "Фигня давай все по новой" << std::endl;
  }

  /* Запуск пайплайна */
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* Создание и запуск главного цикла GStreamer */
  loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);

  /* Остановка пайплайна и освобождение ресурсов */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT(pipeline));
  g_main_loop_unref(loop);

  return 0;
}