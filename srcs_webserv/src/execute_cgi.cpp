#include "../inc/header.hpp"

int ex_cgi(std::string & cmd, int fd, char **env, std::string method) {
    std::string ret;

    int		status = 0;

    //  Parsing file to get 'arg' arg to use execve
    std::vector<std::string>    v;
    size_t  sep = 0;
    size_t  sep_before = 0;
    while (sep_before < cmd.length()) {
        sep = cmd.find(' ', sep_before);
        if (sep == std::string::npos) sep = cmd.length();
        if (sep > sep_before) v.push_back(cmd.substr(sep_before, sep - sep_before));
        sep_before = sep + 1;
    }
    
    char    *arg[v.size()];
    int i = 0;
    for (std::vector<std::string>::iterator it = v.begin();it != v.end();it++) {
        arg[i] = const_cast<char *>((*it).c_str());
        i++;
    }

    int fd_response = open("tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd_response == -1) return 500;

    int pfd[2];
    if (pipe(pfd) == -1) return 500;

    pid_t pid = fork();
    if (pid == -1) return 500;


    if (pid == 0) {
        // Child process
        close(pfd[0]);  // Close read end
        if (dup2(pfd[1], STDOUT_FILENO) == -1) exit(500); // Redirect stdout to pipe
        close(pfd[1]);  // Close write end after dup2
        if (execve(cmd.substr(0, cmd.find(' ')).c_str(), arg, env) == -1) exit(500);
    } else {
        // Parent process
        close(pfd[1]);  // Close write end
        char buffer[1024];
        ssize_t bytesRead;

        // Read from the pipe and write to the response file
        while ((bytesRead = read(pfd[0], buffer, sizeof(buffer))) > 0) {
            if (write(fd_response, buffer, bytesRead) == -1) return 500;
        }

        close(pfd[0]); // Close read end after done
        if (waitpid(pid, &status, 0) == -1) return 500; // Wait for child process
    }

    close(fd_response); // Close response file descriptor
    (void)fd;
    if (status == 500)
        return 500;
    
    std::string fd_response_str = readFileToString("tmp/response");
    std::string fd_response_header = fd_response_str.substr(0, fd_response_str.find("<!DOCTYPE html>"));
    std::string fd_response_body = fd_response_str.substr(fd_response_str.find("<!DOCTYPE html>"));
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string(fd_response_body.length() - 2) + "\r\n";
    response += fd_response_header;
    if (method == "GET")
        response += fd_response_body;
    std::remove("tmp/response");
    send(fd, response.c_str(), response.length(), 0);
    return 0; // Assuming success if no errors occurred
}