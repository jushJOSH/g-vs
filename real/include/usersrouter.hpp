#pragma once

#include <vsrouter.hpp>

template<class ...T>
class UserRouter : VsRouter<T...> {
public:
    // Using base class constructors. No need to override
    using VsRouter<T...>::VsRouter;

    // Overrided registerRouter method
    void registerRouter(crow::App<T...> &app) override;

// Stack of static callbacks for user manipulation
private:
    static void createUserCallback(const crow::request& req, crow::response& res);
    static void deleteUserCallback(const crow::request& req, crow::response& res);
    static void editUserCallback(const crow::request& req, crow::response& res);
};

template<class ...T>
void UserRouter<T...>::registerRouter(crow::App<T...> &app) {
    crow::Blueprint *targetBp = &this->bp;

    CROW_BP_ROUTE((*targetBp), "/create")
    .methods("POST"_method)
    (createUserCallback);

    CROW_BP_ROUTE((*targetBp), "/delete")
    .methods("DELETE"_method)
    (deleteUserCallback);

    CROW_BP_ROUTE((*targetBp), "/edit")
    .methods("PUT"_method)
    (editUserCallback);
    
    VsRouter<T...>::registerRouter(app);
}

template<class ...T>
void UserRouter<T...>::createUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create user placeholder\n";
    res.write("Create user placeholder");
    res.end();
}

template<class ...T>
void UserRouter<T...>::deleteUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Delete user placeholder\n";
    res.write("Delete user placeholder");
    res.end();
}

template<class ...T>
void UserRouter<T...>::editUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Edit user placeholder\n";
    res.write("Edit user placeholder");
    res.end();
}