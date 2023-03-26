#include <api/multipart/mpstreamer.hpp>

std::shared_ptr<Part> MPStreamer::readNextPart(oatpp::async::Action& action) {
    Sample sample(nullptr);
    try {
        const auto &[alreadyUsed, _sample] = source->getSample();
        sample=_sample;
    } catch (const std::exception& e) {
        OATPP_LOGD("MPStreamer", "No samples present for %s", boost::uuids::to_string(source->getUUID()).c_str());
        return nullptr;
    }
    auto buffer = gst_sample_get_buffer(sample);
    GstMapInfo bufferMap;
    bool success = gst_buffer_map(buffer, &bufferMap, GST_MAP_READ);
    if (!success) {
        OATPP_LOGE("MPStreamer", "Error getting buffer map from sample");
        return nullptr;
    }

    std::string data = (const char*)bufferMap.data;
    auto part = std::make_shared<Part>();
    part->putHeader(oatpp::web::protocol::http::Header::CONTENT_TYPE, "image/jpeg");
    part->setPayload(std::make_shared<oatpp::data::resource::InMemoryData>(data));

    return part;
}

void MPStreamer::writeNextPart(const std::shared_ptr<Part>& part, oatpp::async::Action& action) {
    OATPP_LOGE("MPStreamer", "WRITE action requested 0^0");
}