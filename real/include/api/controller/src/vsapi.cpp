#include <api/controller/vsapi.hpp>
#include <video/live/live.hpp>

#include <chrono>
#include <thread>

VSTypes::OatResponse VsapiController::getLive(const oatpp::String &source) {
    if (!liveStreams.contains(source))
        liveStreams[source] = std::make_shared<LiveHandler>(source);
        
    return createResponse(Status::CODE_200, liveStreams[source]->getPlaylist());
}

VSTypes::OatResponse VsapiController::getStatic(const VSTypes::OatRequest &request, const oatpp::String &uuid) {
    auto filepath = request->getPathTail();
    auto range = request->getHeader(Header::RANGE);
    
    auto handler = std::find_if(liveStreams.begin(), liveStreams.end(), 
    [uuid](std::pair<std::string, std::shared_ptr<LiveHandler>> elem){
        return elem.second->getUUID() == uuid;
    });

    if (handler == liveStreams.end())
        return createResponse(Status::CODE_404, "Stream not found");

    return getStaticFileResponse(handler->second, filepath, range);
}

VSTypes::OatResponse VsapiController::getStaticFileResponse(std::shared_ptr<LiveHandler> handler,
                                                            const oatpp::String& filepath,
                                                            const oatpp::String& rangeHeader) const
{
    auto file = handler->getSegment(filepath);
    return file;
    // OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404, "File not found");

    // VSTypes::OatResponse response = !rangeHeader 
    //                                 ? getFullFileResponse(file)
    //                                 : getRangeResponse(rangeHeader, file);

    // response->putHeader("Accept-Ranges", "bytes");
    // response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);

    // auto mimeType = handler->guessMime(filepath);
    // if(mimeType) response->putHeader(Header::CONTENT_TYPE, mimeType);
    // else OATPP_LOGD("Server", "Unknown Mime-Type. Header not set");

    // return response;
}

VSTypes::OatResponse VsapiController::getFullFileResponse(const oatpp::String& file) const {
    return createResponse(Status::CODE_200, file);
}

VSTypes::OatResponse VsapiController::getRangeResponse(const oatpp::String& rangeStr,
                                                       const oatpp::String& file) const
{
    auto range = oatpp::web::protocol::http::Range::parse(rangeStr.getPtr());

    if(range.end == 0)
        range.end = file->size() - 1;

    OATPP_ASSERT_HTTP(range.isValid() &&
                        range.start < file->size() &&
                        range.end > range.start &&
                        range.end < file->size(), Status::CODE_416, "Range is invalid");

    auto chunk = oatpp::String(&file->data()[range.start], (v_int32)(range.end - range.start + 1));
    auto response = createResponse(Status::CODE_206, chunk);
    oatpp::web::protocol::http::ContentRange contentRange(oatpp::web::protocol::http::ContentRange::UNIT_BYTES,
                                                            range.start, range.end, file->size(), true);

    OATPP_LOGD("Server", "range=%s", contentRange.toString()->c_str());
    
    response->putHeader(Header::CONTENT_RANGE, contentRange.toString());
    return response;
}