#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <iostream>
#include "http_parser.hpp"
#include "router.hpp"

namespace asio = boost::asio;
using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

awaitable<void> handleConnection(tcp::socket socket, Router& router) {
    try {
        for (;;) {  
            char buf[8192];

            size_t n = co_await socket.async_read_some(
                asio::buffer(buf),
                use_awaitable
            );

            auto req = HttpParser::parse({buf, n});
            if (!req) break;

            auto response = router.handle(*req);

            co_await asio::async_write(socket,
                asio::buffer(response),
                use_awaitable
            );


            auto it = req->headers.find("connection");
            if (it != req->headers.end() && it->second == "close") break;
        }
    } catch (std::exception& e) {
    }
}

awaitable<void> acceptLoop(tcp::acceptor& acceptor, Router& router) {
    for (;;) {
        auto socket = co_await acceptor.async_accept(use_awaitable);

        co_spawn(acceptor.get_executor(),
            handleConnection(std::move(socket), router),
            detached
        );
    }
}

int main() {
    asio::io_context ioc;  
    Router router;
    router.addRoute("GET", "/api/hello", [](const HttpRequest&) {
        return makeJsonResponse(R"({"msg":"hello"})");
    });

    tcp::acceptor acceptor(ioc, {tcp::v4(), 8080});
    co_spawn(ioc, acceptLoop(acceptor, router), detached);

    std::vector<std::thread> threads;
    auto n = std::thread::hardware_concurrency();
    for (size_t i = 1; i < n; ++i)
        threads.emplace_back([&ioc] { ioc.run(); });
    ioc.run();  
}