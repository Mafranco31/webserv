#include "../inc/header.hpp"

class ErrorHttp;

std::string ft_ex_cgi2(Request request ) {
    std::cout << request << std::endl;
    std::string scriptPath = "." + request.GetUri() + request.GetCgiExt();
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
            std::cout << it->first << " = " << it->second << std::endl;
        }
        std::vector<char*> envp;
        for (size_t i = 0; i < envVec.size(); ++i) {
            envp.push_back(const_cast<char*>(envVec[i].c_str()));
        }
        envp.push_back(NULL);

        const char* args[] = {"php-cgi", scriptPath.c_str(), NULL};

        execve("/usr/local/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        exit(1);
    } else {
        close(inPipe[0]);
        close(outPipe[1]);
    
        write(inPipe[1], postData.c_str(), postData.size());

        close(inPipe[1]);

        // Read response from PHP-CGI
        char buffer[BUFFER_SIZE];
        std::string response;
        ssize_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, BUFFER_SIZE)) > 0) {
            response.append(buffer, bytesRead);
        }
        close(outPipe[0]);

        int statut;
        // Wait for the child process to finish
        waitpid(pid, &statut, 0);
        if (statut != 0)
            throw ErrorHttp("500 Internal Server Error", request.error["500"]);
        return response;
    }
}