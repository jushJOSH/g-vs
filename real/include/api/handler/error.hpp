#pragma once

#include <api/dto/status.hpp>
#include <oatpp/web/server/handler/ErrorHandler.hpp>
#include <oatpp/web/protocol/http/outgoing/ResponseFactory.hpp>

#include <types.hpp>

class ErrorHandler : public oatpp::web::server::handler::ErrorHandler {
private:
  typedef oatpp::web::protocol::http::outgoing::Response OutgoingResponse;
  typedef oatpp::web::protocol::http::Status Status;
  typedef oatpp::web::protocol::http::outgoing::ResponseFactory ResponseFactory;

private:
  std::shared_ptr<oatpp::data::mapping::ObjectMapper> m_objectMapper;
public:
  ErrorHandler(const std::shared_ptr<oatpp::data::mapping::ObjectMapper>& objectMapper);
  VSTypes::OatResponse handleError(const Status& status, const oatpp::String& message, const Headers& headers) override;
};