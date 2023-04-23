#include "api/interceptor/request.hpp"

#include <oatpp/core/data/share/MemoryLabel.hpp>

std::shared_ptr<RequestInterceptor::OutgoingResponse> RequestInterceptor::intercept(const std::shared_ptr<IncomingRequest>& request) {
    auto props = request->getConnection()->getInputStreamContext().getProperties().getAll_Unsafe();
    
    for (auto &prop : props)
        OATPP_LOGD("Interceptor", "'%s' - '%s'", prop.first.getData(), prop.second.getData());

    return nullptr;
}