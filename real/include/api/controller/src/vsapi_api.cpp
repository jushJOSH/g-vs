#include <api/controller/vsapi.hpp>

#include <oatpp/core/utils/ConversionUtils.hpp>

VSTypes::OatResponse VsapiController::createMedia(const std::shared_ptr<JwtPayload> &payload, const oatpp::String &mediainfo) {
    auto mediaObject = jsonMapper->readFromString<oatpp::Object<MediaDto>>(mediainfo);
    bool success;
    mediaObject->owner = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
    vsapiService.addMedia(mediaObject);

    return createDtoResponse(Status::CODE_200, mediaObject);
}

VSTypes::OatResponse VsapiController::modifyMedia(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &media, const oatpp::String &mediainfo) {
    bool success;
    auto mediaObject = jsonMapper->readFromString<oatpp::Object<MediaDto>>(mediainfo);
    mediaObject->owner = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");

    OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, mediaObject->owner),
                      Status::CODE_404, "No media found");

    mediaObject->id = media;
    vsapiService.modifyMedia(mediaObject);
    return createDtoResponse(Status::CODE_200, mediaObject); 
}

VSTypes::OatResponse VsapiController::deleteMedia(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &media) {
    bool success;
    
    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");

    OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid),
                      Status::CODE_404, "No media found");

    vsapiService.removeMedia(media);
    return createResponse(Status::CODE_200, oatpp::utils::conversion::int32ToStr(media));
}

VSTypes::OatResponse VsapiController::modifySource(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &source, const oatpp::Int32 &media, const oatpp::String &sourceinfo) {
    bool success;

    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
    OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid),
                      Status::CODE_404, "No media found");
    OATPP_ASSERT_HTTP(vsapiService.isSourceBelongsToUser(source, userid),
                      Status::CODE_404, "No source found");
    OATPP_ASSERT_HTTP(vsapiService.isMediaContainsSource(media, source),
                      Status::CODE_404, "No source found for requested media");

    auto sourceObject = jsonMapper->readFromString<oatpp::Object<SourceDto>>(sourceinfo);
    sourceObject->id = source;

    vsapiService.modifySource(sourceObject);
    return createDtoResponse(Status::CODE_200, sourceObject);
}

VSTypes::OatResponse VsapiController::deleteSource(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &source, const oatpp::Int32 &media) {
    bool success;

    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
    OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid),
                      Status::CODE_404, "No media found");
    OATPP_ASSERT_HTTP(vsapiService.isSourceBelongsToUser(source, userid),
                      Status::CODE_404, "No source found");
    OATPP_ASSERT_HTTP(vsapiService.isMediaContainsSource(media, source),
                      Status::CODE_404, "No source found for requested media");


    vsapiService.removeSource(source);
    return createResponse(Status::CODE_200, oatpp::utils::conversion::int32ToStr(source));
}