#include <utils/filemanager.hpp>

oatpp::String StaticFilesManager::getExtension(const oatpp::String& filename) {
    v_int32 dotPos;
    for(dotPos = filename->size() - 1; 
        dotPos > 0 && filename->data()[dotPos] != '.'; 
        dotPos--);

    if(dotPos != 0 && dotPos < filename->size() - 1)
        return oatpp::String((const char*)&filename->data()[dotPos + 1], filename->size() - dotPos - 1);
    
    return nullptr;
}

oatpp::String StaticFilesManager::getFileCached(const oatpp::String& path) {
    if(!path) return nullptr;

    std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
    auto& file = m_cache[path];
    if(file) return file;

    oatpp::String filename = m_basePath + "/" + path;
    file = oatpp::String::loadFromFile(filename->c_str());
    return file;
}

oatpp::String StaticFilesManager::getFile(const oatpp::String& path) {
    if(!path) return nullptr;

    oatpp::String filename = m_basePath + "/" + path;
    return oatpp::String::loadFromFile(filename->c_str());
}

oatpp::String StaticFilesManager::guessMimeType(const oatpp::String& filename) {
    auto extension = getExtension(filename);

    if(!extension)                  return nullptr;
    else if(extension == "m3u8")    return "application/x-mpegURL";
    else if(extension == "mp4")     return "video/mp4";
    else if(extension == "ts")      return "video/MP2T";
    else if(extension == "mp3")     return "audio/mp3";
    else                            return nullptr;
}

oatpp::String StaticFilesManager::getBasePath() const {
    return m_basePath;
}