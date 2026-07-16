#pragma once
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include "http_parser.hpp"

using Handler   = std::function<std::string(const HttpRequest&)>;
using PathParams = std::unordered_map<std::string, std::string>;

struct Route {
    std::string              method;
    std::string              pattern;  
    std::regex               regex;    
    std::vector<std::string> paramNames; 
    Handler                  handler;
};


auto compilePattern(const std::string& pattern)
    -> std::pair<std::regex, std::vector<std::string>>
{
    std::string regexStr = "^";
    std::vector<std::string> names;
    size_t pos = 0;
    while (pos < pattern.size()) {
        if (pattern[pos] == ':') {
            auto end = pattern.find('/', pos);
            if (end == std::string::npos) end = pattern.size();
            names.push_back(pattern.substr(pos + 1, end - pos - 1));
            regexStr += "([^/]+)";
            pos = end;
        } else {
            if (pattern[pos] == '.') regexStr += "\\.";
            else regexStr += pattern[pos];
            ++pos;
        }
    }
    regexStr += "$";
    return {std::regex(regexStr), names};
}

class Router {
public:
    void addRoute(std::string method,
                  std::string pattern,
                  Handler    handler)
    {
        auto [rx, names] = compilePattern(pattern);
        routes_.push_back({
            std::move(method),
            std::move(pattern),
            std::move(rx),
            std::move(names),
            std::move(handler)
        });
    }

    std::string handle(const HttpRequest& req) {
        for (const auto& route : routes_) {
            if (route.method != req.method) continue;

            std::smatch match;
            if (!std::regex_match(req.path, match, route.regex)) continue;

            HttpRequest enriched = req;
            for (size_t i = 0; i < route.paramNames.size(); ++i)
                enriched.headers["param:" + route.paramNames[i]] = match[i+1];

            return route.handler(enriched);
        }
        return makeErrorResponse(404, "Not Found");
    }

private:
    std::vector<Route> routes_;
};


inline std::string makeJsonResponse(const std::string& body,
                                     int status = 200) {
    return
        "HTTP/1.1 " + std::to_string(status) + " OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: keep-alive\r\n"
        "\r\n" + body;
}

inline std::string makeErrorResponse(int code, std::string_view msg) {
    std::string body = R"({"error":"") + std::string(msg) + R"("})";
    return
        "HTTP/1.1 " + std::to_string(code) + " Error\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" + body;
}