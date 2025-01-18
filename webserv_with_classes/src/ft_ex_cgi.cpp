#include "../inc/header.hpp"

class ErrorHttp;

const int TIMEOUT = 2; // Timeout in seconds

void terminate_child(pid_t child_pid) {
    kill(child_pid, SIGKILL); // Kill the child process
}

std::string ft_ex_cgi_get(Request request ) {
    std::string scriptPath = "." + request.GetUri() + ".php";//request.GetCgiExt();
    std::string postData = request.GetBody();
    std::map<std::string, std::string> envVars;
    envVars["REQUEST_METHOD"] = request.GetMethod();
    envVars["REDIRECT_STATUS"] = "1";
    envVars["SCRIPT_FILENAME"] = scriptPath;
    envVars["QUERY_STRING"] = request.GetQueryString();


    int pfd[2];
    if (pipe(pfd) == -1) throw ErrorHttp("500 Internal Server Error", "/500");

    pid_t pid = fork();
    if (pid < 0) {
        throw ErrorHttp("500 Internal Server Error", request.error["500"]);
    } else if (pid == 0) {
        dup2(pfd[1], STDOUT_FILENO);

        close(pfd[0]);

        std::vector<std::string> envVec;
        for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
            envVec.push_back(it->first + "=" + it->second);
        }
        std::vector<char*> envp;
        for (size_t i = 0; i < envVec.size(); ++i) {
            envp.push_back(const_cast<char*>(envVec[i].c_str()));
        }
        envp.push_back(NULL);

        const char* args[] = {"php-cgi", scriptPath.c_str(), NULL};

        execve("/usr/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        exit(1);
    } else {
        // Parent process
        close(pfd[1]); // Close write end
        char buffer[BUFFER_SIZE];
        std::string response;
        ssize_t bytesRead;

        // Timeout monitoring
        fd_set read_fds;
        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(pfd[0], &read_fds);

        bool timeout_reached = false;
        while (true) {
            int ret = select(pfd[0] + 1, &read_fds, NULL, NULL, &timeout);
            if (ret > 0) {
                if (FD_ISSET(pfd[0], &read_fds)) {
                    bytesRead = read(pfd[0], buffer, BUFFER_SIZE);
                    if (bytesRead > 0) {
                        response.append(buffer, bytesRead);
                    } else {
                        break; // Finished reading
                    }
                }
            } else if (ret == 0) {
                // Timeout reached
                timeout_reached = true;
                break;
            } else {
                // Error in select()
                throw ErrorHttp("500 Internal Server Error", request.error["500"]);
            }
        }

        close(pfd[0]); // Close read end
        int status;

        if (timeout_reached) {
            terminate_child(pid);
            throw ErrorHttp("500 Internal Server Error", request.error["500"]);
        }

        waitpid(pid, &status, 0); // Wait for the child process
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            throw ErrorHttp("500 Internal Server Error", request.error["500"]);
        }

        std::string tot_response = "HTTP/1.1 200 OK\r\n" + response;
        std::cout << "Response: " << tot_response << std::endl;

        return tot_response;
    }
}

int ft_ex_cgi_post(Request request ) {
    std::string scriptPath = "." + request.GetUri() + ".php";//request.GetCgiExt();
    std::string postData = request.GetBody();
    std::map<std::string, std::string> envVars;
    std::string contentType1 = request.GetHeaders()["CONTENT-TYPE"];
    std::string contentType = contentType1.substr(contentType1.find_first_not_of(' '), contentType1.find_last_not_of(' ') - contentType1.find_first_not_of(' '));
    envVars["CONTENT_TYPE"] = contentType;
    envVars["REQUEST_METHOD"] = request.GetMethod();
    envVars["CONTENT_LENGTH"] = ft_itoa(postData.size());
    envVars["REDIRECT_STATUS"] = "1";
    envVars["SCRIPT_FILENAME"] = scriptPath;


    int inPipe[2], outPipe[2];
    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        throw ErrorHttp("500 Internal Server Error", request.error["500"]);
    }

    pid_t pid = fork();
    if (pid < 0) {
        throw ErrorHttp("500 Internal Server Error", request.error["500"]);

    } else if (pid == 0) {
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        close(inPipe[1]);
        close(outPipe[0]);

        std::vector<std::string> envVec;
        for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
            envVec.push_back(it->first + "=" + it->second);
        }
        std::vector<char*> envp;
        for (size_t i = 0; i < envVec.size(); ++i) {
            envp.push_back(const_cast<char*>(envVec[i].c_str()));
        }
        envp.push_back(NULL);

        const char* args[] = {"php-cgi", scriptPath.c_str(), NULL};

        execve("/usr/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        exit(1);
    } else {
        close(inPipe[0]);
        close(outPipe[1]);
    
        write(inPipe[1], postData.c_str(), postData.size());

        close(inPipe[1]);

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(outPipe[0], &read_fds);

        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;

        // Wait for either the pipe to receive data or the timeout
        int select_result = select(outPipe[0] + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result < 0) {
            close(outPipe[0]);
            return 1;
        } else if (select_result == 0) {
            // Timeout occurred
            terminate_child(pid); // Kill the child process
            close(outPipe[0]);
            return 1;
        }

        // Read from the pipe to check if the child terminated
        int status;
        waitpid(pid, &status, 0);
        close(outPipe[0]);
        return status;
    }
}