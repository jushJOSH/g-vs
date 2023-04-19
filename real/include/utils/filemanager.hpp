#pragma once

#include <oatpp/core/data/mapping/ObjectMapper.hpp>
#include <oatpp/core/concurrency/SpinLock.hpp>

#include <unordered_map>

class StaticFilesManager {
private:
    oatpp::String m_basePath;
    oatpp::concurrency::SpinLock m_lock;
    std::unordered_map<oatpp::String, oatpp::String> m_cache;

private:
    oatpp::String getExtension(const oatpp::String& filename);

public:
    StaticFilesManager(const oatpp::String& basePath)
        : m_basePath(basePath)
    {}

    oatpp::String getFileCached(const oatpp::String& path);
    oatpp::String getFile(const oatpp::String& path);
    oatpp::String guessMimeType(const oatpp::String& filename);
    oatpp::String getBasePath() const;
};