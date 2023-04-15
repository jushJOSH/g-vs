#pragma once

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/handler/AuthorizationHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

class RequestInterceptor : public oatpp::web::server::interceptor::RequestInterceptor {
public:
    RequestInterceptor() = default;
    std::shared_ptr<OutgoingResponse> intercept(const std::shared_ptr<IncomingRequest>& request) override;
};