#include <api/db/service/vsapi.hpp>
#include <types.hpp>
#include <misc.hpp>

oatpp::Vector<oatpp::Object<MediaDto>> VsapiService::getMediaByUser(const oatpp::String& authToken) {
    auto result = vsapi_database->getMediaByUser(authToken);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());

    // Fetch whole vector
    auto receivedData = result->fetch<oatpp::Vector<oatpp::Object<MediaDto>>>();
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
}

void VsapiService::modifyMedia(const oatpp::Object<MediaDto>& dto) {
    auto result = vsapi_database->modifyMedia(dto);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
} 

void VsapiService::removeMedia(const oatpp::Int32 mediaid) {
    auto result = vsapi_database->removeMedia(mediaid);
    OATPP_ASSERT_HTTP(result->isSuccess(), VSTypes::OatStatus::CODE_500, result->getErrorMessage());
}

void VsapiService::addSource(const oatpp::Object<SourceDto>& sourceDto, const oatpp::Object<SourceDto>& mediaDto) {
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

void VsapiService::removeSource(const oatpp::Object<SourceDto>& dto) {
    auto result = vsapi_database->removeSource(dto->id);
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

void VsapiService::createMedia_SourceIfNotExists() {
    auto result_media_source = vsapi_database->createMedia_SourceIfNotExists();
    OATPP_ASSERT_HTTP(result_media_source->isSuccess(), VSTypes::OatStatus::CODE_500, result_media_source->getErrorMessage());
}

void VsapiService::initVsapiTable() {
    // Important order!
    // Media & Media_Source has f_key to source -> Source #1
    // Media_Source has f_key to media -> Media #2
    // Media_Source #3
    createSourceIfNotExists();
    createMediaIfNotExists();
    createMedia_SourceIfNotExists();
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
    auto found = std::find_if(receivedData->begin(), receivedData->end(), [sourceid](const MediaDto::Wrapper elem) {
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