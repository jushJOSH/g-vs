#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/orm/Connection.hpp>
#include <oatpp/orm/QueryResult.hpp>

namespace VSTypes {
    typedef std::shared_ptr<oatpp::web::server::api::ApiController::OutgoingResponse> OatResponce;
    typedef oatpp::provider::ResourceHandle<oatpp::orm::Connection> OatDBConnection;
    typedef std::shared_ptr<oatpp::orm::QueryResult> OatQueryResult;
};