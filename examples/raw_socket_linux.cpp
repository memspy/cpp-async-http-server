#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    listen(server_fd, 128);
    std::cout << "listening on :8080\n";

    while(true){
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_d = accept(server_fd,
                                (sockaddr*)&client_addr,
                                &client_len);
        char buf[4096];
        ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
        buf[n] = '\0';

        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "hello world!";
        write(client_fd, response, strlen(response));
        close(client_fd);
    }

}