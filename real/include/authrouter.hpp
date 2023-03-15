#pragma once

#include <vsrouter.hpp>

template<class ...T>
class AuthRouter : VsRouter<T...> {
public:
    // Using base class constructors. No need to override
    using VsRouter<T...>::VsRouter;

    // Overrided registerRouter method
    void registerRouter(crow::App<T...> &app) override;

// Stack of static callbacks for registation
private:
    static void createTokenCallback(const crow::request& req, crow::response& res);
    static void refreshTokenCallback(const crow::request& req, crow::response& res);
};

template<class ...T>
void AuthRouter<T...>::registerRouter(crow::App<T...> &app) {
    crow::Blueprint *targetBp = &this->bp;

    CROW_BP_ROUTE((*targetBp), "/create")
    .methods("GET"_method)
    (createTokenCallback);

    CROW_BP_ROUTE((*targetBp), "/refresh")
    .methods("POST"_method)
    (refreshTokenCallback);
    
    VsRouter<T...>::registerRouter(app);
}

template<class ...T>
void AuthRouter<T...>::createTokenCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create token placeholder\n";
    res.write("Create token placeholder");
    res.end();
}

template<class ...T>
void AuthRouter<T...>::refreshTokenCallback(const crow::request& req, crow::response& res) {
    std::cout << "Refresh token placeholder\n";
    res.write("Refresh token placeholder");
    res.end();
}