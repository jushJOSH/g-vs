#include <api/multipart/multipart.hpp>

std::shared_ptr<Part> MPStreamer::readNextPart(oatpp::async::Action& action) {
    auto currentPart = oatpp::String::loadFromFile(this->filename.c_str()).getValue("");
    if (currentPart == previousPart) {
        OATPP_LOGD("Multipart","No new parts");
        return nullptr;
    }

    std::string newBuffer;
    std::copy(currentPart.begin() + previousPart.size(), currentPart.end(), std::back_inserter(newBuffer));
    
    OATPP_LOGD("Multipart","About to send %d", currentPart.end() - currentPart.begin() - previousPart.size());
    auto part = std::make_shared<Part>();
    part->putHeader(oatpp::web::protocol::http::Header::CONTENT_TYPE, "application/x-mpegURL");
    part->setPayload(std::make_shared<oatpp::data::resource::InMemoryData>(newBuffer));

    return part;
}

void MPStreamer::writeNextPart(const std::shared_ptr<Part>& part, oatpp::async::Action& action) {
    OATPP_LOGE("MPStreamer", "WRITE action requested 0^0");
}