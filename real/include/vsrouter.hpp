#pragma once

#include <crow.h>

template<class ...T>
class VsRouter {
public:
    
    /// @brief Default constructor for crow::Blueprint obj
    /// @param defaultPath path param
    VsRouter(const std::string& defaultPath);

    /// @brief Default constructor for crow::Blueprint obj
    /// @param defaultPath path param
    /// @param defaultStatic static param
    VsRouter(const std::string& defaultPath, const std::string& defaultStatic);
    
    /// @brief Default constructor for crow::Blueprint obj
    /// @param defaultPath path param
    /// @param defaultStatic static param
    /// @param defaultTemplate template param
    VsRouter(const std::string& defaultPath, const std::string& defaultStatic, const std::string& defaultTemplate);

    /// @brief Virtual function to override for derived classes
    /// @param app By default just registers protected crow::Blueprint
    virtual void registerRouter(crow::App<T...> &app);
    
    /// @brief Allows to get actual Crow blueprint
    /// @return crow::Blueprint Blueprint
    crow::Blueprint& getBlueprint();

protected:
    crow::Blueprint bp;
};

template<class ...T>
crow::Blueprint& VsRouter<T...>::getBlueprint() {
    return bp;
}

template<class ...T>
VsRouter<T...>::VsRouter(const std::string& defaultPath)
: bp(defaultPath)
{}

template<class ...T>
VsRouter<T...>::VsRouter(const std::string &defaultPath, const std::string &defaultStatic)
: bp(defaultPath, defaultStatic)
{}

template<class ...T>
VsRouter<T...>::VsRouter(const std::string &defaultPath, const std::string &defaultStatic, const std::string &defaultTemplate)
: bp(defaultPath, defaultStatic, defaultTemplate)
{}

template<class ...T>
void VsRouter<T...>::registerRouter(crow::App<T...> &app) {
    app.register_blueprint(bp);
}