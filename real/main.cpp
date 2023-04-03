#include <video/source/source.hpp>

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    SourceConfigDto config;
    config.archive_path = "/home/egor/";

    Source testSource("rtsp://193.19.103.188:1935/live/Pl_Lunincev.stream");
    auto screenshot = std::make_shared<ScreenshotBranch>(config);
    auto archive = std::make_shared<ArchiveBranch>(config);
    auto stream = std::make_shared<StreamBranch>(config);

    testSource.addBranch("screenshot", screenshot);
    testSource.addBranch("archive", archive);
    testSource.addBranch("stream", stream);

    testSource.setState();

    auto loop = g_main_loop_new(NULL, false);
    g_main_loop_run(loop);
}