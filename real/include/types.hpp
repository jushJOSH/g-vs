#pragma once

#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/orm/Connection.hpp>
#include <oatpp/orm/QueryResult.hpp>
#include <oatpp/web/protocol/http/Http.hpp>

namespace VSTypes {
    // // Web section
    // typedef std::shared_ptr<oatpp::web::server::api::ApiController::OutgoingResponse> OatResponse;
    
    // DB section
    typedef oatpp::provider::ResourceHandle<oatpp::orm::Connection> OatDBConnection;
    typedef std::shared_ptr<oatpp::orm::QueryResult> OatQueryResult;

    // HTTP
    typedef oatpp::web::protocol::http::Status OatStatus;
    typedef std::shared_ptr<oatpp::web::protocol::http::incoming::Request> OatRequest;
    typedef std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> OatResponse;
    typedef oatpp::web::protocol::http::outgoing::ResponseFactory OatResponseFactory;
};