#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

int main(){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    listen(server_fd, 128);
    std::cout << "listening on :8080\n";

    while(true){
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        
        char buf[4096];
        int n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        buf[n] = '\0';
        std::cout << "Request:\n" << buf << std::endl;

        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "hello world!";
        send(client_fd, response, strlen(response), 0);
        closesocket(client_fd);
    }

    closesocket(server_fd);
    WSACleanup();
}