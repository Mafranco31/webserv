#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("socket");
        exit(1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        close(clientSocket);
        exit(1);
    }

    std::string postData = "key=value";
    std::string request = "POST / HTTP/1.1\r\n";
    request += "Host: " + std::string(SERVER_ADDRESS) + "\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";
    request += "Content-Length: " + std::to_string(postData.size()) + "\r\n\r\n";
    request += postData;

    send(clientSocket, request.c_str(), request.size(), 0);

    char buffer[4096];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << buffer << std::endl;
    }

    close(clientSocket);
    return 0;
}
