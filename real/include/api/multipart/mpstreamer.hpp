#pragma once

#include <oatpp/web/mime/multipart/Multipart.hpp>
#include <oatpp/web/mime/multipart/Part.hpp>
#include <oatpp/core/data/resource/InMemoryData.hpp>
#include <oatpp/web/protocol/http/Http.hpp>

#include <thread>
#include <video/source/source.hpp>

using namespace oatpp::web::mime::multipart;

class MPStreamer : public Multipart {
public:
    MPStreamer(std::shared_ptr<StreamBranch> branch)
    :   Multipart(generateRandomBoundary()), branch(branch)
    {}

    std::shared_ptr<Part> readNextPart(oatpp::async::Action& action);
    void writeNextPart(const std::shared_ptr<Part>& part, oatpp::async::Action& action) override;

private:
    std::shared_ptr<StreamBranch> branch;
};
