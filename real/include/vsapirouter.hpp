#pragma once

#include <vsrouter.hpp>
#include <vector>
#include <pipeline.hpp>

template<class ...T>
class VsapiRouter : VsRouter<T...> {
public:
    // Using base class constructors. No need to override
    /// @brief Default constructor for crow::Blueprint obj
    /// @param pipelines videoserver pipelines. Need for this api manipulation
    /// @param defaultPath path param
    VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath);

    /// @brief Default constructor for crow::Blueprint obj
    /// @param pipelines videoserver pipelines. Need for this api manipulation
    /// @param defaultPath path param
    /// @param defaultStatic static param
    VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath, const std::string& defaultStatic);
    
    /// @brief Default constructor for crow::Blueprint obj
    /// @param pipelines videoserver pipelines. Need for this api manipulation
    /// @param defaultPath path param
    /// @param defaultStatic static param
    /// @param defaultTemplate template param
    VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath, const std::string& defaultStatic, const std::string& defaultTemplate);

    // Overrided registerRouter method
    void registerRouter(crow::App<T...> &app) override;

// Pipelines
protected:
    std::vector<std::shared_ptr<GPipeline>> pipelines;

// Stack of static callbacks for vsapi
private:
    static void createMediaCallback(const crow::request& req, crow::response& res);
    static void deleteMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
    static void editMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
    static void snapMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
    static void streamMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
    static void archiveMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
};

template<class ...T>
void VsapiRouter<T...>::registerRouter(crow::App<T...> &app) {
    crow::Blueprint *targetBp = &this->bp;

    CROW_BP_ROUTE((*targetBp), "/add")
    .methods("POST"_method)
    (createMediaCallback);

    CROW_BP_ROUTE((*targetBp), "/delete/<string>")
    .methods("DELETE"_method)
    (deleteMediaCallback);

    CROW_BP_ROUTE((*targetBp), "/edit/<string>")
    .methods("PUT"_method)
    (editMediaCallback);

    CROW_BP_ROUTE((*targetBp), "/snap/<string>")
    .methods("GET"_method)
    (snapMediaCallback);

    CROW_BP_ROUTE((*targetBp), "/stream/<string>")
    .methods("GET"_method)
    (streamMediaCallback);

    CROW_BP_ROUTE((*targetBp), "/archive/<string>")
    .methods("GET"_method)
    (archiveMediaCallback);
    
    VsRouter<T...>::registerRouter(app);
}

template<class ...T>
VsapiRouter<T...>::VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath) 
: pipelines(pipelines), VsRouter<T...>::VsRouter(defaultPath)
{}

template<class ...T>
VsapiRouter<T...>::VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath, const std::string& defaultStatic) 
: pipelines(pipelines), VsRouter<T...>::VsRouter(defaultPath, defaultStatic)
{}

template<class ...T>
VsapiRouter<T...>::VsapiRouter(std::vector<std::shared_ptr<GPipeline>>& pipelines, const std::string& defaultPath, const std::string& defaultStatic, const std::string& defaultTemplate) 
: pipelines(pipelines), VsRouter<T...>::VsRouter(defaultPath, defaultStatic, defaultTemplate)
{}


template<class ...T>
void VsapiRouter<T...>::createMediaCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create media placeholder\n";
    res.write("Create media placeholder");
    res.end();
}

template<class ...T>
void VsapiRouter<T...>::deleteMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Delete media placeholder\n";
    res.write("Delete media placeholder");
    res.end();
}

template<class ...T>
void VsapiRouter<T...>::editMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Edit media placeholder\n";
    res.write("Edit media placeholder");
    res.end();
}

template<class ...T>
void VsapiRouter<T...>::snapMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Snap media placeholder\n";
    res.write("Snap media placeholder");
    res.end();
}

template<class ...T>
void VsapiRouter<T...>::streamMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Stream media placeholder\n";
    res.write("Stream media placeholder");
    res.end();
}

template<class ...T>
void VsapiRouter<T...>::archiveMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Archive media placeholder\n";
    res.write("Archive media placeholder");
    res.end();
}