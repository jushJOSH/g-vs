#include <videoserver.hpp>

void createUserCallback(const crow::request& req, crow::response& res);
void deleteUserCallback(const crow::request& req, crow::response& res);
void editUserCallback(const crow::request& req, crow::response& res);

crow::Blueprint Videoserver::registerUserBlueprint() {
    crow::Blueprint userBp("user");

    CROW_BP_ROUTE(userBp, "/create")
    .methods("POST"_method)
    (createUserCallback);

    CROW_BP_ROUTE(userBp, "/delete")
    .methods("DELETE"_method)
    (deleteUserCallback);

    CROW_BP_ROUTE(userBp, "/edit")
    .methods("PUT"_method)
    (editUserCallback);

    return userBp;
}

void createUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create user placeholder\n";
    res.write("Create user placeholder");
    res.end();
}

void deleteUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Delete user placeholder\n";
    res.write("Delete user placeholder");
    res.end();
}

void editUserCallback(const crow::request& req, crow::response& res) {
    std::cout << "Edit user placeholder\n";
    res.write("Edit user placeholder");
    res.end();
}