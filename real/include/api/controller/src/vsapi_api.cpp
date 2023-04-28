#include <api/controller/vsapi.hpp>
 
#include <video/videoserver/videoserver.hpp>
#include <api/dto/config.hpp>
#include <oatpp/core/utils/ConversionUtils.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using boost::str;
using boost::format;

VSTypes::OatResponse VsapiController::createMedia(const std::shared_ptr<JwtPayload> &payload, const oatpp::String &mediainfo) {
    auto mediaObject = jsonMapper->readFromString<oatpp::Object<MediaDto>>(mediainfo);
    bool success;
    mediaObject->owner = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
    vsapiService.addMedia(mediaObject);
    // for (auto &source : *(mediaObject->sources)) {
    //     vsapiService.addSource(source, mediaObject);
    //     OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    //     if (source->cache_mode->contains("archive")) {
    //         OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

    //         auto createdSource = videoserver->openSource(source.getPtr());
    //         auto filename = str(format("%s/%d.mkv") % config->archivePath.getValue("") % source->id.getValue(-1));
    //         auto createdBranch = createdSource->runArchive(filename);

    //         runningArchives[source->id.getValue(-1)] = {createdSource->getSource(), createdBranch->getUUID()};
    //     } else {
    //         auto runningArchive = runningArchives[source->id.getValue(-1)];
    //         videoserver->removeBranchFromSource(runningArchive.first, runningArchive.second);
    //     }
    // }

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
    // for (auto &source : *(mediaObject->sources)) {
    //     OATPP_COMPONENT(std::shared_ptr<Videoserver>, videoserver);
    //     if (source->cache_mode->contains("archive")) {
    //         OATPP_COMPONENT(oatpp::Object<ConfigDto>, config);

    //         auto createdSource = videoserver->openSource(source.getPtr());
    //         auto filename = str(format("%s/%d.mkv") % config->archivePath.getValue("") % source->id.getValue(-1));
    //         auto createdBranch = createdSource->runArchive(filename);

    //         runningArchives[source->id.getValue(-1)] = {createdSource->getSource(), createdBranch->getUUID()};
    //     } else {
    //         auto runningArchive = runningArchives[source->id.getValue(-1)];
    //         videoserver->removeBranchFromSource(runningArchive.first, runningArchive.second);
    //     }
    // }

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

// VSTypes::OatResponse VsapiController::modifySource(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &source, const oatpp::Int32 &media, const oatpp::String &sourceinfo) {
//     bool success;

//     auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
//     OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
//     OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid),
//                       Status::CODE_404, "No media found");
//     OATPP_ASSERT_HTTP(vsapiService.isSourceBelongsToUser(source, userid),
//                       Status::CODE_404, "No source found");
//     OATPP_ASSERT_HTTP(vsapiService.isMediaContainsSource(media, source),
//                       Status::CODE_404, "No source found for requested media");

//     auto sourceObject = jsonMapper->readFromString<oatpp::Object<SourceDto>>(sourceinfo);
//     sourceObject->id = source;

//     vsapiService.modifySource(sourceObject);
//     return createDtoResponse(Status::CODE_200, sourceObject);
// }

// VSTypes::OatResponse VsapiController::deleteSource(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &source, const oatpp::Int32 &media) {
//     bool success;

//     auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
//     OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    
//     OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid),
//                       Status::CODE_404, "No media found");
//     OATPP_ASSERT_HTTP(vsapiService.isSourceBelongsToUser(source, userid),
//                       Status::CODE_404, "No source found");
//     OATPP_ASSERT_HTTP(vsapiService.isMediaContainsSource(media, source),
//                       Status::CODE_404, "No source found for requested media");


//     vsapiService.removeSource(source);
//     return createResponse(Status::CODE_200, oatpp::utils::conversion::int32ToStr(source));
// }

VSTypes::OatResponse VsapiController::getSpecialMedia(const std::shared_ptr<JwtPayload> &payload, const oatpp::Int32 &media) {
    bool success;

    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");
    OATPP_ASSERT_HTTP(vsapiService.isMediaBelongsToUser(media, userid), Status::CODE_404, "No media found");

    return createDtoResponse(Status::CODE_200, vsapiService.getMediaById(media));
}

VSTypes::OatResponse VsapiController::getMedia(const std::shared_ptr<JwtPayload> &payload) {
    bool success;

    auto userid = oatpp::utils::conversion::strToInt32(payload->userId, success);
    OATPP_ASSERT_HTTP(success, Status::CODE_401, "Unknown user id");

    return createDtoResponse(Status::CODE_200, vsapiService.getMediaByUser(payload->token));
}