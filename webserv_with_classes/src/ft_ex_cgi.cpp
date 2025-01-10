#include "../inc/header.hpp"

class ErrorHttp;

std::string readFileToString(const std::string& filename) {
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

// char **create_arguments(const Request & request, std::string program) {
//     std::string language = request.GetCgiExt();
    
//     std::vector<std::string> v_args;
//     v_args.push_back(program);
//     v_args.push_back("." + request.GetFullUri());

//     std::map<std::string, std::string> args = request.GetMarg();

//     for (std::map<std::string, std::string>::iterator it = args.begin(); it != args.end(); it++) {
//         v_args.push_back(it->second);
//     }

//     char    *arguments[v_args.size()];
//     int i = 0;
//     for (std::vector<std::string>::iterator it = v_args.begin();it != v_args.end();it++) {
//         arguments[i] = const_cast<char *>((*it).c_str());
//         std::cout << "arg: " << arguments[i] << std::endl;
//         i++;
//     }
//     arguments[i] = NULL;
//     return arguments;
// }

// char **create_environment2(const Request & request) {
//     std::vector<std::string> env;

//     env.push_back("REQUEST_METHOD=" + request.GetMethod());
//     // env.push_back("QUERY_STRING=" + request.GetFullUri());
//     env.push_back("SCRIPT_FILENAME=" + request.GetFullUri());
//     env.push_back("CONTENT_TYPE=" + request.GetHeaders()["CONTENT-TYPE"]);
//     env.push_back("CONTENT_LENGTH=" + request.GetHeaders()["CONTENT-LENGTH"]);
//     //env.push_back("PWD=/Users/mathis/Desktop/ecole42/cc052webserv/webserv_with_classes/.");
//     env.push_back("REDIRECT_STATUS=1");

//     // env.push_back("PATH=/Library/Frameworks/Python.framework/Versions/3.12/bin:/usr/local/bin:/usr/local/sbin:/Library/Frameworks/Python.framework/Versions/3.11/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/opt/X11/bin:/Library/Apple/usr/bin:/Users/mathis/anaconda3/bin:/Users/mathis/anaconda3/condabin:/Library/Frameworks/Python.framework/Versions/3.12/bin:/usr/local/sbin:/Library/Frameworks/Python.framework/Versions/3.11/bin");
    
//     std::cout << "ENV:" << std::endl;
//     std::vector<char*> envp;
//     for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it) {
//         envp.push_back(const_cast<char*>(it->c_str()));
//         std::cout << *it << std::endl;
//     }
//     envp.push_back(nullptr);
//     return envp.data();
// }

std::vector<std::string> create_environment(const Request & request) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + request.GetMethod());
    env.push_back("SCRIPT_FILENAME=." + request.GetFullUri());
    if (request.GetMethod() == "POST") {
        env.push_back("CONTENT_TYPE=" + request.GetHeaders()["CONTENT-TYPE"]);
        env.push_back("CONTENT_LENGTH=" + request.GetHeaders()["CONTENT-LENGTH"]);
    }
    else
        env.push_back("QUERY_STRING=" + request.GetQueryString());
    //env.push_back("PWD=/Users/mathis/Desktop/ecole42/cc052webserv/webserv_with_classes/.");
    env.push_back("REDIRECT_STATUS=1");
    return env;
}

std::string ft_ex_cgi(int fd, char **env2, Request & request) {
    int		status = 0;
    (void)env2;
    (void)fd;
//     (void)env2;

    std::vector<std::string> env = create_environment(request);
    
    // Convert environment vector to char* array
    std::cout << "ENV:" << std::endl;
    std::vector<char*> envp;
    for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it) {
        envp.push_back(const_cast<char*>(it->c_str()));
        std::cout << *it << std::endl;
    }
    envp.push_back(nullptr);
    // Null-terminate the array

    // Prepare argv for execve

//  BUILDING PROGRAM
    std::string language = request.GetCgiExt();
    std::string program;
    // if (language == ".py")
    //     program = "/usr/bin/python3";
    if (language == ".php")
        program = "/usr/local/bin/php-cgi";
    else
        throw ErrorHttp("404 Not Found", request.error["404"]);


//  BUILDING ARGUMENTS
    std::vector<std::string> v_args;
    v_args.push_back(program);
    v_args.push_back("." + request.GetFullUri());

    std::map<std::string, std::string> args = request.GetMarg();

    for (std::map<std::string, std::string>::iterator it = args.begin(); it != args.end(); it++) {
        v_args.push_back(it->second);
    }

    char    *arguments[v_args.size()];
    int i = 0;
    for (std::vector<std::string>::iterator it = v_args.begin();it != v_args.end();it++) {
        arguments[i] = const_cast<char *>((*it).c_str());
        std::cout << "arg: " << arguments[i] << std::endl;
        i++;
    }
    arguments[i] = NULL;
    // std::string program = "/usr/local/bin/php-cgi -c php.ini";
    // char **env = create_environment(request);
    // char **arguments = create_arguments(request, program);

    char *envp2[] = {NULL};
//  EXECUTION OF CGI SCRIPT
    int fd_response = open("tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd_response == -1)  throw ErrorHttp("500 Internal Server Error", "/500");

    // int pfd[2];
    // if (pipe(pfd) == -1)    throw ErrorHttp("500 Internal Server Error", "/500");

    int input_pipe[2], output_pipe[2];
    if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1)  throw ErrorHttp("500 Internal Server Error", "/500");

    pid_t pid = fork();
    if (pid == -1)          throw ErrorHttp("500 Internal Server Error", "/500");


    if (pid == 0) {
        // Child process
        close(input_pipe[1]); // Close unused write end of input pipe
        close(output_pipe[0]); // Close unused read end of output pipe
        // Redirect stdin and stdout to the pipes
        dup2(input_pipe[0], STDIN_FILENO);
        dup2(output_pipe[1], STDOUT_FILENO);
        // Close duplicated descriptors
        // close(input_pipe[0]);
        // close(output_pipe[1]);
        if (execve(program.c_str(), envp2, envp.data()) == -1) exit(500);
    } else {
        // Parent process
        close(input_pipe[0]);
        close(output_pipe[1]);
        // Write to PHP-CGI's stdin if needed
        // For example: dprintf(input_pipe[1], "data=value\n");
        write(input_pipe[1], request.GetBody().c_str(), request.GetBodyLength());
        close(input_pipe[1]); // Close after writing (or if not needed)
        // Read PHP-CGI's output
        char buffer[1024];
        ssize_t bytes_read;
        
        while ((bytes_read = read(output_pipe[0], buffer, sizeof(buffer))) > 0) {
            if (write(fd_response, buffer, bytes_read) == -1) {
                throw ErrorHttp("500 Internal Server Error", "/500");
            }
        }

        close(output_pipe[0]);
        if (waitpid(pid, &status, 0) == -1) {
            throw ErrorHttp("500 Internal Server Error", "/500");
        }
    }
    close(fd_response);
    if (status == 500)
        throw ErrorHttp("500 Internal Server Error", "/500");
    // if (status != 1)
    //     std::cout << "CGI script returned status " << status << std::endl;

//  READ RESPONSE FROM CGI SCRIPT
    std::string fd_response_body = readFileToString("tmp/response");

    std::string response = "HTTP/1.1 200 OK\r\n" + fd_response_body;
    return response;
    // std::string response = "HTTP/1.1 200 OK\r\n";
    // response += "Content-Type: text/html\r\n";
    // response += "Connection: close\r\n";
    // response += "Content-Length: " + ft_strlen(fd_response_body) + "\r\n";
    // response += fd_response_body;
    // return response;
}

// std::string ft_ex_cgi(int fd, char **env2, Request & request) {
//     int		status = 0;
//     (void)env2;

//     std::vector<std::string> env = create_environment(request);
    
//     // Convert environment vector to char* array
//     std::cout << "ENV:" << std::endl;
//     std::vector<char*> envp;
//     for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it) {
//         envp.push_back(const_cast<char*>(it->c_str()));
//         std::cout << *it << std::endl;
//     }
//     envp.push_back(nullptr);
//     // Null-terminate the array

//     // Prepare argv for execve

// //  BUILDING PROGRAM
//     std::string language = request.GetCgiExt();
//     std::string program;
//     // if (language == ".py")
//     //     program = "/usr/bin/python3";
//     if (language == ".php")
//         program = "/usr/local/bin/php-cgi -c php.ini";
//     else
//         throw ErrorHttp("404 Not Found", request.error["404"]);


// //  BUILDING ARGUMENTS
//     std::vector<std::string> v_args;
//     v_args.push_back(program);
//     v_args.push_back("." + request.GetFullUri());

//     std::map<std::string, std::string> args = request.GetMarg();

//     for (std::map<std::string, std::string>::iterator it = args.begin(); it != args.end(); it++) {
//         v_args.push_back(it->second);
//     }

//     char    *arguments[v_args.size()];
//     int i = 0;
//     for (std::vector<std::string>::iterator it = v_args.begin();it != v_args.end();it++) {
//         arguments[i] = const_cast<char *>((*it).c_str());
//         std::cout << "arg: " << arguments[i] << std::endl;
//         i++;
//     }
//     arguments[i] = NULL;


// //  EXECUTION OF CGI SCRIPT
//     int fd_response = open("tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
//     if (fd_response == -1) {
//         //delete[] charArray;
//         throw ErrorHttp("500 Internal Server Error", "/500");
//     }

//     int pfd[2];
//     if (pipe(pfd) == -1) {
//         //delete[] charArray;
//         throw ErrorHttp("500 Internal Server Error", "/500");
//     }

//     pid_t pid = fork();
//     if (pid == -1) {
//         //delete[] charArray;
//         throw ErrorHttp("500 Internal Server Error", "/500");
//     }


//     if (pid == 0) {
//         // Child process
//         close(pfd[0]);  // Close read end
//         if (dup2(pfd[1], STDOUT_FILENO) == -1) exit(500); // Redirect stdout to pipe
//         close(pfd[1]);  // Close write end after dup2
//         if (execve(program.c_str(), arguments, envp.data()) == -1) exit(500);
//     } else {
//         // Parent process
//         close(pfd[1]);  // Close write end
//         char buffer[1024];
//         ssize_t bytesRead;

//         if (dup2(pfd[0], STDIN_FILENO) == -1) throw ErrorHttp("500 Internal Server Error", "/500");
//         while ((bytesRead = read(pfd[0], buffer, sizeof(buffer))) > 0) {
//             if (write(fd_response, buffer, bytesRead) == -1) {
//                 throw ErrorHttp("500 Internal Server Error", "/500");
//             }
//         }

//         close(pfd[0]);
//         if (waitpid(pid, &status, 0) == -1) {
//             throw ErrorHttp("500 Internal Server Error", "/500");
//         }
//     }
//     close(fd_response);
//     (void)fd;
//     if (status == 500)
//         throw ErrorHttp("500 Internal Server Error", "/500");
//     // if (status != 1)
//     //     std::cout << "CGI script returned status " << status << std::endl;

// //  READ RESPONSE FROM CGI SCRIPT
//     std::string fd_response_body = readFileToString("tmp/response");
//     std::string response = "HTTP/1.1 200 OK\r\n";
//     response += "Content-Type: text/html\r\n";
//     response += "Connection: close\r\n";
//     response += "Content-Length: " + ft_strlen(fd_response_body) + "\r\n\n";
//     response += fd_response_body;
//     return response;
// }