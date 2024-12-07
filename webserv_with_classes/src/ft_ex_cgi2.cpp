#include "../inc/header.hpp"

class ErrorHttp;

std::string readFileToString2(const std::string& filename) {
    // Create an ifstream object to read the file
    std::ifstream file(filename.c_str());

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    // Use a stringstream to read the file contents into a std::string
    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file into the buffer

    // Close the file
    file.close();

    // Return the contents of the file as a std::string
    return buffer.str();
}

std::string ft_ex_cgi2(int fd, char **env, Request & request) {
    std::string ret;

    std::string language = request.GetCgiExt();
    int		status = 0;
    std::string program;

    if (language == ".py")
        program = "/bin/python3";
    else if (language == ".php")
        program = "/bin/php";
    else
        throw ErrorHttp("404 Not Found", request.e_404);

    std::cout << "program = " << program << "request = " << "." << request.GetUri() << language << std::endl;
    std::vector<std::string> v;
    v.push_back(program);
    v.push_back("." + request.GetUri() + language);
    char    *arguments[v.size()];
    int i = 0;
    for (std::vector<std::string>::iterator it = v.begin();it != v.end();it++) {
        arguments[i] = const_cast<char *>((*it).c_str());
        std::cout << "arg: " << arguments[i] << std::endl;
        i++;
    }
    arguments[i] = NULL;

    int fd_response = open("tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd_response == -1) {
        //delete[] charArray;
        throw ErrorHttp("500 Internal Server Error", "/500");
    }

    int pfd[2];
    if (pipe(pfd) == -1) {
        //delete[] charArray;
        throw ErrorHttp("500 Internal Server Error", "/500");
    }

    pid_t pid = fork();
    if (pid == -1) {
        //delete[] charArray;
        throw ErrorHttp("500 Internal Server Error", "/500");
    }


    if (pid == 0) {
        // Child process
        close(pfd[0]);  // Close read end
        if (dup2(pfd[1], STDOUT_FILENO) == -1) exit(500); // Redirect stdout to pipe
        close(pfd[1]);  // Close write end after dup2
        if (execve(program.c_str(), arguments, env) == -1) exit(500);
    } else {
        // Parent process
        close(pfd[1]);  // Close write end
        char buffer[1024];
        ssize_t bytesRead;

        // Read from the pipe and write to the response file
        while ((bytesRead = read(pfd[0], buffer, sizeof(buffer))) > 0) {
            if (write(fd_response, buffer, bytesRead) == -1) {
            //delete[] charArray;
            throw ErrorHttp("500 Internal Server Error", "/500");
            }
        }

        close(pfd[0]); // Close read end after done
        if (waitpid(pid, &status, 0) == -1) {
            //delete[] charArray;
            throw ErrorHttp("500 Internal Server Error", "/500"); // Wait for child process
        }
    }
    //delete[] charArray;
    close(fd_response); // Close response file descriptor
    (void)fd;
    if (status == 500)
        throw ErrorHttp("500 Internal Server Error", "/500");

    std::string fd_response_body = readFileToString2("tmp/response");

    //return fd_response_str;
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + ft_strlen(fd_response_body) + "\r\n\n";
    //response += fd_response_header;
    if (request.GetMethod()== "GET") {
        response += fd_response_body;
    }
    std::cout << request.GetMethod() << std::endl;
    //std::remove("tmp/response");
    return response;
    //send(fd, response.c_str(), response.length(), 0);
    //return 0; // Assuming success if no errors occurred
}
