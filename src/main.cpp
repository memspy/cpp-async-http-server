#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include "router.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::string serveFile(const HttpRequest& req) {
    auto safePath = req.path;
    while (safePath.find("../") != std::string::npos)
        safePath.erase(safePath.find("../"), 3);

    fs::path filepath = "./public" + safePath;
    if (!fs::exists(filepath) || fs::is_directory(filepath))
        filepath = "./public/index.html";

    std::ifstream file(filepath, std::ios::binary);
    if (!file) return makeErrorResponse(404, "File not found");

    std::string content((std::istreambuf_iterator<char>(file)), {});

    static const std::unordered_map<std::string, std::string> mimes = {
        {".html", "text/html"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".json", "application/json"},
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".svg",  "image/svg+xml"},
    };
    auto ext = filepath.extension().string();
    auto mime_it = mimes.find(ext);
    auto mime = (mime_it != mimes.end())
                    ? mime_it->second : "application/octet-stream";

    return
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: " + mime + "\r\n"
        "Content-Length: " + std::to_string(content.size()) + "\r\n"
        "\r\n" + content;
}

std::string handleGetUsers(const HttpRequest& req) {
    json users = {
        {"id", 1}, {"name", "Alice"}, {"email", "alice@example.com"},
        {"id", 2}, {"name", "Bob"},   {"email", "bob@example.com"}
    };
    return makeJsonResponse(users.dump());
}

std::string handleCreateUser(const HttpRequest& req) {
    try {
        auto body = json::parse(req.body);
        if (!body.contains("name"))
            return makeErrorResponse(400, "'name' is required");

        json created = {
            {"id", 42},
            {"name", body["name"]},
            {"created", true}
        };
        return makeJsonResponse(created.dump(), 201);
    } catch (...) {
        return makeErrorResponse(400, "Invalid JSON");
    }
}

void setupRoutes(Router& router) {
    router.addRoute("GET",  "/api/users",     handleGetUsers);
    router.addRoute("POST", "/api/users",     handleCreateUser);
    router.addRoute("GET",  "/api/users/:id", [](const HttpRequest& req) {
        auto id = req.headers.at("param:id");
        json user = {{"id", stoi(id)}, {"name", "User #" + id}};
        return makeJsonResponse(user.dump());
    });
    router.addRoute("GET", "/:any", serveFile);
}