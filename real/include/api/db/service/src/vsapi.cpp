#include <api/db/service/vsapi.hpp>
#include <types.hpp>
#include <misc.hpp>

oatpp::Vector<oatpp::Object<MediaDto>> VsapiService::getMediaByUser(const oatpp::String& authToken) {
    auto result = vsapi_database->getMediaByUser(authToken);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    // Fetch whole vector
    auto receivedData = result->fetch<oatpp::Vector<oatpp::Object<MediaDto>>>();
    for (auto &elem : *receivedData)
        elem->sources = getSourceByMedia(elem);

    return receivedData;
}

oatpp::Vector<oatpp::Object<SourceDto>> VsapiService::getSourceByMedia(const oatpp::Object<MediaDto>& dto) {
    auto result = vsapi_database->getSourceByMedia(dto->id);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    // Fetch whole vector
    auto receivedData = result->fetch<oatpp::Vector<oatpp::Object<SourceDto>>>();
    return receivedData;
}

void VsapiService::changeDefaultSource(oatpp::Int32 newDefaultSource, const oatpp::Object<MediaDto>& dto) {
    auto result = vsapi_database->changeDefaultSource(newDefaultSource, dto->id);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void VsapiService::addMedia(const oatpp::Object<MediaDto>& dto) {
    auto result = vsapi_database->addMedia(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
    // Fetching id
    dto->id = VSMisc::fetchId<MediaDto>(result);
    
    for (auto &elem : *(dto->sources))
        addSource(elem, dto);
}

void VsapiService::modifyMedia(const oatpp::Object<MediaDto>& dto) {
    auto result = vsapi_database->modifyMedia(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    for (auto &elem : *(dto->sources)) {
        OATPP_ASSERT_HTTP(isMediaContainsSource(dto->id, elem->id), VSTypes::OatStatus::CODE_403, "Requested source does not belong to requested media! Check your ID");
        modifySource(elem);
    }
} 

void VsapiService::removeMedia(const oatpp::Int32 mediaid) {
    auto result = vsapi_database->removeMedia(mediaid);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void VsapiService::addSource(const oatpp::Object<SourceDto>& sourceDto, const oatpp::Object<MediaDto>& mediaDto) {
    sourceDto->media = mediaDto->id;
    auto create_result = vsapi_database->addSource(sourceDto);
    OATPP_ASSERT_HTTP(create_result->isSuccess(), VSTypes::OatStatus::CODE_500, create_result->getErrorMessage());
    
    // Fetching id
    sourceDto->id = VSMisc::fetchId<SourceDto>(create_result);

    // Linking(park) created source to media
    auto link_result = vsapi_database->linkMediaSource(mediaDto->id, sourceDto->id);
    OATPP_ASSERT_HTTP(link_result->isSuccess(), VSTypes::OatStatus::CODE_500, link_result->getErrorMessage());
}

void VsapiService::modifySource(const oatpp::Object<SourceDto>& dto) {
    auto result = vsapi_database->modifySource(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void VsapiService::removeSource(const oatpp::Int32 &sourceid) {
    auto result = vsapi_database->removeSource(sourceid);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void VsapiService::createMediaIfNotExists() {
    auto result_media = vsapi_database->createMediaIfNotExists();
    OATPP_ASSERT_HTTP(result_media->isSuccess(), VSTypes::OatStatus::CODE_500, result_media->getErrorMessage());
}

void VsapiService::createSourceIfNotExists() {
    auto result_source = vsapi_database->createSourceIfNotExists();
    OATPP_ASSERT_HTTP(result_source->isSuccess(), VSTypes::OatStatus::CODE_500, result_source->getErrorMessage());
}

void VsapiService::initVsapiTable() {
    // Important order!
    // Media #1
    // Source #2
    createMediaIfNotExists();
    createSourceIfNotExists();
}

bool VsapiService::isMediaBelongsToUser(oatpp::Int32 mediaid, oatpp::Int32 userid) {
    auto query_result = vsapi_database->isMediaBelongsToUser(userid);
    OATPP_ASSERT_HTTP(query_result->isSuccess(), VSTypes::OatStatus::CODE_500, query_result->getErrorMessage());

    auto receivedData = query_result->fetch<oatpp::Vector<oatpp::Object<MediaDto>>>();
    auto found = std::find_if(receivedData->begin(), receivedData->end(), [mediaid](const MediaDto::Wrapper elem) {
        return elem->id == mediaid;
    });

    return found != receivedData->end();
}

bool VsapiService::isSourceBelongsToUser(oatpp::Int32 sourceid, oatpp::Int32 userid) {
    auto query_result = vsapi_database->isMediaBelongsToUser(userid);
    OATPP_ASSERT_HTTP(query_result->isSuccess(), VSTypes::OatStatus::CODE_500, query_result->getErrorMessage());

    auto receivedData = query_result->fetch<oatpp::Vector<oatpp::Object<SourceDto>>>();
    auto found = std::find_if(receivedData->begin(), receivedData->end(), [sourceid](const SourceDto::Wrapper elem) {
        return elem->id == sourceid;
    });

    return found != receivedData->end();
}

bool VsapiService::isMediaContainsSource(oatpp::Int32 mediaid, oatpp::Int32 sourceid) {
    auto query_result = vsapi_database->isMediaHasSource(mediaid, sourceid);
    OATPP_ASSERT_HTTP(query_result->isSuccess(), VSTypes::OatStatus::CODE_500, query_result->getErrorMessage());
    auto receivedData = query_result->fetch<oatpp::Vector<oatpp::Object<SourceDto>>>();

    return !receivedData->empty();
}

oatpp::Object<SourceDto> VsapiService::getSourceById(oatpp::Int32 sourceid) {
    auto query_result = vsapi_database->getSourceById(sourceid);
    OATPP_ASSERT_HTTP(query_result->isSuccess(), VSTypes::OatStatus::CODE_500, query_result->getErrorMessage());
    auto receivedSource = query_result->fetch<oatpp::Vector<oatpp::Object<SourceDto>>>();
    OATPP_ASSERT_HTTP(receivedSource->size(), VSTypes::OatStatus::CODE_404, "Source not found");

    return receivedSource[0];
}

oatpp::Object<MediaDto> VsapiService::getMediaById(oatpp::Int32 mediaid) {
    auto query_result = vsapi_database->getMediaById(mediaid);
    OATPP_ASSERT_HTTP(query_result->isSuccess(), VSTypes::OatStatus::CODE_500, query_result->getErrorMessage());
    auto receivedMedia = query_result->fetch<oatpp::Vector<oatpp::Object<MediaDto>>>();
    OATPP_ASSERT_HTTP(receivedMedia->size(), VSTypes::OatStatus::CODE_404, "Media not found");
    receivedMedia[0]->sources = getSourceByMedia(receivedMedia[0]);

    return receivedMedia[0];
}