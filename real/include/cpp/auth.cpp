#include <videoserver.hpp>

void createTokenCallback(const crow::request& req, crow::response& res);
void refreshTokenCallback(const crow::request& req, crow::response& res);

crow::Blueprint Videoserver::registerAuthBlueprint() {
    crow::Blueprint authBp("auth");

    CROW_BP_ROUTE(authBp, "/create")
    .methods("GET"_method)
    (createTokenCallback);

    CROW_BP_ROUTE(authBp, "/refresh")
    .methods("POST"_method)
    (refreshTokenCallback);

    return authBp;
}

void createTokenCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create token placeholder\n";
    res.write("Create token placeholder");
    res.end();
}

void refreshTokenCallback(const crow::request& req, crow::response& res) {
    std::cout << "Refresh token placeholder\n";
    res.write("Refresh token placeholder");
    res.end();
}