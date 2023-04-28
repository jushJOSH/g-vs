#pragma once

#include <oatpp/core/data/mapping/ObjectMapper.hpp>
#include <oatpp/core/concurrency/SpinLock.hpp>

#include <unordered_map>
#include <filesystem>

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
    {
        if (!std::filesystem::create_directory(m_basePath->c_str())) {
            throw std::runtime_error("Could not create directory " + *m_basePath);
        }
    }
    ~StaticFilesManager() {
        std::filesystem::remove_all(m_basePath->c_str());
    }

    oatpp::String getFileCached(const oatpp::String& path);
    oatpp::String getFile(const oatpp::String& path);
    oatpp::String guessMimeType(const oatpp::String& filename);
    oatpp::String getBasePath() const;
};