#pragma once

#include <types.hpp>

namespace VSMisc {
    template<class DTO_T>
    oatpp::Int32 fetchId(const VSTypes::OatQueryResult &qResult) {
        // Fetching id
        auto receivedData = qResult->fetch<oatpp::Vector<oatpp::Object<DTO_T>>>();
        OATPP_ASSERT_HTTP(receivedData->size() == 1, VSTypes::OatStatus::CODE_500, "Unable to insert row"); 
        return receivedData[0]->id;
    }   
};