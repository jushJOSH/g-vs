#include <api/multipart/mpstreamer.hpp>

std::shared_ptr<Part> MPStreamer::readNextPart(oatpp::async::Action& action) {
    std::this_thread::sleep_for(std::chrono::seconds(1)); // For Simple-API you may sleep just like that
    
    oatpp::String frameData = "<frame-binary-jpeg-data>"; // frame data. One jpeg image.
    
    auto part = std::make_shared<Part>();
    part->putHeader(oatpp::web::protocol::http::Header::CONTENT_TYPE, "image/jpeg");
    part->setPayload(std::make_shared<oatpp::data::resource::InMemoryData>(frameData));

    return part;
}

void MPStreamer::writeNextPart(const std::shared_ptr<Part>& part, oatpp::async::Action& action) {
    OATPP_LOGE("MPStreamer", "WRITE action requested 0^0");
}