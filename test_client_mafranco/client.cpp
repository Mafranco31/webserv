#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERVER_IP "127.0.0.1"  // Change this to the server's IP address
#define PORT 80  // Port 80 for HTTP
#define BUFFER_SIZE 30000

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char *message;
    char server_reply[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Set server address information
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server on port %d.\n", PORT);

    // Send data (HTTP GET request)
    while(1) {
        //message = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";  // Simple HTTP request
        std::string message;
        std::cin >> message;
        if (send(sock, message.c_str(), message.length(), 0) < 0) {
            perror("Send failed");
            close(sock);
            exit(EXIT_FAILURE);
        }
        std::cout << "Data sent : " << message << std::endl;

        // Receive response from server
        ssize_t received_len = recv(sock, server_reply, BUFFER_SIZE, 0);
        if (received_len < 0) {
            perror("Receive failed");
        } else {
            server_reply[received_len] = '\0';  // Null-terminate the received string
            std::cout << "Server reply : " << server_reply << std::endl;
        }
    }
    // Close the socket
    close(sock);
    printf("Connection closed.\n");

    return 0;
}
