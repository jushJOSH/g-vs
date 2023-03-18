#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/orm/Connection.hpp>
#include <oatpp/orm/QueryResult.hpp>
#include <oatpp/web/protocol/http/Http.hpp>

namespace VSTypes {
    typedef std::shared_ptr<oatpp::web::server::api::ApiController::OutgoingResponse> OatResponce;
    typedef oatpp::provider::ResourceHandle<oatpp::orm::Connection> OatDBConnection;
    typedef std::shared_ptr<oatpp::orm::QueryResult> OatQueryResult;
    typedef oatpp::web::protocol::http::Status OatStatus;
};