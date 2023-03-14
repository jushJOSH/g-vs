#include <videoserver.hpp>

void createMediaCallback(const crow::request& req, crow::response& res);
void deleteMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
void editMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
void snapMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
void streamMediaCallback(const crow::request& req, crow::response& res, const std::string& id);
void archiveMediaCallback(const crow::request& req, crow::response& res, const std::string& id);

crow::Blueprint Videoserver::registerVsapiBlueprint() {
    crow::Blueprint vsapiBp("vsapi");

    CROW_BP_ROUTE(vsapiBp, "/add")
    .methods("POST"_method)
    (createMediaCallback);

    CROW_BP_ROUTE(vsapiBp, "/delete/<string>")
    .methods("DELETE"_method)
    (deleteMediaCallback);

    CROW_BP_ROUTE(vsapiBp, "/edit/<string>")
    .methods("PUT"_method)
    (editMediaCallback);

    CROW_BP_ROUTE(vsapiBp, "/snap/<string>")
    .methods("GET"_method)
    (snapMediaCallback);

    CROW_BP_ROUTE(vsapiBp, "/stream/<string>")
    .methods("GET"_method)
    (snapMediaCallback);

    CROW_BP_ROUTE(vsapiBp, "/archive/<string>")
    .methods("GET"_method)
    (snapMediaCallback);

    return vsapiBp;
}

void createMediaCallback(const crow::request& req, crow::response& res) {
    std::cout << "Create media placeholder\n";
    res.write("Create media placeholder");
    res.end();
}

void deleteMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Delete media placeholder\n";
    res.write("Delete media placeholder");
    res.end();
}

void editMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Edit media placeholder\n";
    res.write("Edit media placeholder");
    res.end();
}

void snapMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Snap media placeholder\n";
    res.write("Snap media placeholder");
    res.end();
}

void streamMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Stream media placeholder\n";
    res.write("Stream media placeholder");
    res.end();
}

void archiveMediaCallback(const crow::request& req, crow::response& res, const std::string& id) {
    std::cout << "Archive media placeholder\n";
    res.write("Archive media placeholder");
    res.end();
}
