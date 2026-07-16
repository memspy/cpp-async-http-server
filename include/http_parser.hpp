#pragma once
#include <string_view>
#include <string>
#include <unordered_map>
#include <optional>

struct HttpRequest {
    std::string method;   
    std::string path;    
    std::string query;    
    std::string version;  
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class HttpParser {
public:
    static std::optional<HttpRequest> parse(std::string_view raw) {
        HttpRequest req;

        auto line_end = raw.find("\r\n");
        if (line_end == std::string_view::npos) return std::nullopt;

        auto request_line = raw.substr(0, line_end);
        if (!parseRequestLine(request_line, req)) return std::nullopt;

        auto headers_start = line_end + 2;
        auto headers_end   = raw.find("\r\n\r\n", headers_start);
        if (headers_end == std::string_view::npos) return std::nullopt;

        parseHeaders(raw.substr(headers_start, headers_end - headers_start), req);

        auto body_start = headers_end + 4; 
        if (body_start < raw.size()) {
            auto it = req.headers.find("content-length");
            if (it != req.headers.end()) {
                size_t len = stoul(it->second);
                req.body = std::string(raw.substr(body_start, len));
            }
        }
        return req;
    }

private:
    static bool parseRequestLine(std::string_view line, HttpRequest& req) {
        auto s1 = line.find(' ');
        auto s2 = line.find(' ', s1 + 1);
        if (s1 == std::string_view::npos ||
            s2 == std::string_view::npos) return false;

        req.method  = std::string(line.substr(0, s1));
        req.version = std::string(line.substr(s2 + 1));

 
        auto uri = line.substr(s1 + 1, s2 - s1 - 1);
        auto qm  = uri.find('?');
        if (qm != std::string_view::npos) {
            req.path  = std::string(uri.substr(0, qm));
            req.query = std::string(uri.substr(qm + 1));
        } else {
            req.path = std::string(uri);
        }
        return true;
    }

    static void parseHeaders(std::string_view section, HttpRequest& req) {
        size_t pos = 0;
        while (pos < section.size()) {
            auto eol = section.find("\r\n", pos);
            if (eol == std::string_view::npos) eol = section.size();

            auto line = section.substr(pos, eol - pos);
            auto colon = line.find(':');
            if (colon != std::string_view::npos) {
                auto key = line.substr(0, colon);
                auto val = line.substr(colon + 1);
                while (!val.empty() && val.front() == ' ') val.remove_prefix(1);
                std::string k(key);
                for (auto& c : k) c = tolower(c);
                req.headers[k] = std::string(val);
            }
            pos = eol + 2;
        }
    }
};