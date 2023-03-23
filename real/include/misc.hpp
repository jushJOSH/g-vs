#pragma once

#include <types.hpp>

namespace VSMisc {
    template<class DTO_T>
    oatpp::Int32 fetchId(const VSTypes::OatQueryResult &qResult) {
        // Fetching id
        auto receivedData = qResult->fetch<oatpp::Vector<oatpp::Object<DTO_T>>>();

        if (receivedData->empty()) return -1; 
        else return receivedData[0]->id;
    }   
};