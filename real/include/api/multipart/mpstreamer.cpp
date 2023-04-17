#include <api/multipart/mpstreamer.hpp>

#include <oatpp/core/macro/component.hpp>
#include <video/videoserver/videoserver.hpp>

std::shared_ptr<Part> MPStreamer::readNextPart(oatpp::async::Action& action) {   
    auto sample = branch->getSample();
    if (sample->isNull())
        return nullptr;

    auto buffer = gst_sample_get_buffer(*sample);
    GstMapInfo bufferMap;
    bool success = gst_buffer_map(buffer, &bufferMap, GST_MAP_READ);
    if (!success) {
        OATPP_LOGE("MPStreamer", "Error getting buffer map from sample");
    }

    std::string data(bufferMap.data, bufferMap.data + bufferMap.size);
    auto part = std::make_shared<Part>();
    part->putHeader(oatpp::web::protocol::http::Header::CONTENT_TYPE, "video/mp4");
    part->setPayload(std::make_shared<oatpp::data::resource::InMemoryData>(data));
    
    return part;
}

void MPStreamer::writeNextPart(const std::shared_ptr<Part>& part, oatpp::async::Action& action) {
    OATPP_LOGE("MPStreamer", "WRITE action requested 0^0");
}

MPStreamer::~MPStreamer() {
    OATPP_LOGD("MPStreamer", "Deleted one");
    g_print("Use count:\n%ld for source\n%ld for branch\n", source.use_count(), branch.use_count());
    auto uuid = branch->getUUID();
    auto src = source->getSource();
    
    OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    videoserver->removeBranchFromSource(src, uuid);
}