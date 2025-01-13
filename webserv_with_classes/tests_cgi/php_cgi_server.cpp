#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096
#define SERVER_PORT 8080

// Parse HTTP headers
std::map<std::string, std::string> parseHeaders(const std::string& headers) {
    std::map<std::string, std::string> headerMap;
    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line) && line != "\r") {
        size_t delimiter = line.find(": ");
        if (delimiter != std::string::npos) {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 2);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            headerMap[key] = value;
        }
    }

    return headerMap;
}

// Execute PHP-CGI
std::string executePhpCgi(const std::string& scriptPath, const std::string& postData, const std::map<std::string, std::string>& envVars) {
    int inPipe[2], outPipe[2];
    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { // Child process
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);
        close(inPipe[1]);
        close(outPipe[0]);

        std::vector<std::string> envVec;
        for (const auto& [key, value] : envVars) {
            envVec.push_back(key + "=" + value);
        }
        std::vector<char*> envp;
        for (const auto& entry : envVec) {
            envp.push_back(const_cast<char*>(entry.c_str()));
        }
        envp.push_back(nullptr);

        const char* args[] = {"php-cgi", nullptr};
        execve("/usr/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        perror("execve");
        exit(1);
    } else {
        close(inPipe[0]);
        close(outPipe[1]);

        write(inPipe[1], postData.c_str(), postData.size());
        close(inPipe[1]);

        char buffer[BUFFER_SIZE];
        std::string response;
        ssize_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, BUFFER_SIZE)) > 0) {
            response.append(buffer, bytesRead);
        }
        close(outPipe[0]);
        waitpid(pid, nullptr, 0);

        return response;
    }

    return "";
}

// Start the server
void startServer() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        exit(1);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("listen");
        close(serverSocket);
        exit(1);
    }

    std::cout << "Server is running on port " << SERVER_PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            perror("accept");
            continue;
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesRead <= 0) {
            close(clientSocket);
            continue;
        }
        buffer[bytesRead] = '\0';

        std::string request(buffer);
        auto headersEnd = request.find("\r\n\r\n");
        std::string headers = request.substr(0, headersEnd);
        std::string body = request.substr(headersEnd + 4);

        auto headerMap = parseHeaders(headers);

        std::map<std::string, std::string> envVars;
        envVars["REQUEST_METHOD"] = "POST";
        envVars["CONTENT_TYPE"] = headerMap["Content-Type"];
        envVars["CONTENT_LENGTH"] = headerMap["Content-Length"];
        envVars["REDIRECT_STATUS"] = "1";
        envVars["SCRIPT_FILENAME"] = "./script.php"; // Change this to your PHP script path


        std::string response = executePhpCgi(envVars["SCRIPT_FILENAME"], body, envVars);

        std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + response;
        send(clientSocket, httpResponse.c_str(), httpResponse.size(), 0);

        close(clientSocket);
    }

    close(serverSocket);
}

int main() {
    startServer();
    return 0;
}
