#pragma once

#include <api/db/client/vsapi.hpp>
#include <api/db/client/users.hpp>
#include <api/db/dto/media.hpp>
#include <api/db/dto/user.hpp>
#include <types.hpp>

#include <oatpp/web/protocol/http/Http.hpp>
#include <oatpp/core/macro/component.hpp>

/*  
    QUERY(getMediaByUser,
    QUERY(getSourceByMedia,
    QUERY(changeDefaultSource,
    QUERY(addMedia,
    QUERY(removeMedia,
    QUERY(addSource,
    QUERY(linkMediaSource,
    QUERY(removeSource,
    QUERY(createMediaIfNotExists,
    QUERY(createSourceIfNotExists,
    QUERY(createMedia_SourceIfNotExists,
    QUERY(modifyMedia,
    QUERY(modifySource,
*/

class VsapiService {
// Vsapi client handlers
public:
    oatpp::Vector<oatpp::Object<MediaDto>> getMediaByUser(const oatpp::String& authToken);
    oatpp::Vector<oatpp::Object<SourceDto>> getSourceByMedia(const oatpp::Object<MediaDto>& dto);
    void changeDefaultSource(oatpp::Int32 newDefaultSource, const oatpp::Object<MediaDto>& dto);
    void addMedia(const oatpp::Object<MediaDto>& dto);
    void modifyMedia(const oatpp::Object<MediaDto>& dto);
    void removeMedia(const oatpp::Object<MediaDto>& dto);
    void addSource(const oatpp::Object<SourceDto>& sourceDto, const oatpp::Object<SourceDto>& mediaDto);
    void modifySource(const oatpp::Object<SourceDto>& sourceDto);
    void removeSource(const oatpp::Object<SourceDto>& dto);

    void initVsapiTable();
    
private:
    void createMediaIfNotExists();
    void createSourceIfNotExists();
    void createMedia_SourceIfNotExists();

    // Injecting database component
    OATPP_COMPONENT(std::shared_ptr<VsapiClient>, vsapi_database);
};