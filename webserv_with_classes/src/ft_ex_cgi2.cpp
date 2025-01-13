#include "../inc/header.hpp"


std::string transformBody(const std::string& input) {
    std::string result;

    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if (*it == '"') {
            result += '\\'; // Add backslash before the quote
        }
        result += *it; // Add the current character
    }
    return result;
}

std::string transformContentType(const std::string& input) {
    std::string result = input;

    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    return result;
}

//std::string executePhpCgi(const std::string& scriptPath, const std::string& postData, const std::map<std::string, std::string>& envVars) {
std::string ft_ex_cgi2(const Request & request ) {
    std::cout << request << std::endl;
    std::string scriptPath = "." + request.GetUri() + request.GetCgiExt();
    std::string postData = transformBody(request.GetBody());
    std::string contenttype = transformContentType(request.GetHeaders()["CONTENT-TYPE"]);
    std::map<std::string, std::string> envVars;
    envVars["REQUEST_METHOD"] = "POST";
    envVars["CONTENT_TYPE"] = contenttype;
    // envVars["CONTENT_TYPE"] = "multipart/form-data; boundary=----WebKitFormBoundarypwU7QDV3czBcGsB6";
    envVars["CONTENT_LENGTH"] = std::string(ft_itoa(postData.size()));
    envVars["REDIRECT_STATUS"] = "1";
    envVars["SCRIPT_FILENAME"] = scriptPath; // Change this to your PHP script path


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
        // Redirect stdin and stdout
        dup2(inPipe[0], STDIN_FILENO);
        dup2(outPipe[1], STDOUT_FILENO);

        // Close unused pipe ends
        close(inPipe[1]);
        close(outPipe[0]);

        // Prepare environment variables
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

        // Prepare arguments for execve
        const char* args[] = {"php-cgi", scriptPath.c_str(), NULL};

        // Execute PHP-CGI
        execve("/usr/local/bin/php-cgi", const_cast<char* const*>(args), envp.data());
        perror("execve");
        exit(1);
    } else { // Parent process
        // Close unused pipe ends
        close(inPipe[0]);
        close(outPipe[1]);

        // Write POST data to PHP-CGI
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

        // Wait for the child process to finish
        waitpid(pid, NULL, 0);

        return response;
    }

    return "";
}