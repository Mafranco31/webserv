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
    env.push_back("SCRIPT_FILENAME=." + request.GetFullUri() + request.GetCgiExt());
    if (request.GetMethod() == "POST") {
        env.push_back("CONTENT_TYPE=" + request.GetHeaders()["CONTENT-TYPE"]);
        env.push_back("CONTENT_LENGTH=" + request.GetHeaders()["CONTENT-LENGTH"]);
    }
    else
    {
        env.push_back("QUERY_STRING=" + request.GetQueryString());
        // env.push_back("CONTENT_LENGTH=0");
    }
    //env.push_back("PWD=/Users/mathis/Desktop/ecole42/cc052webserv/webserv_with_classes/.");
    env.push_back("REDIRECT_STATUS=1");
    return env;
}

// std::string ft_ex_cgi(int fd, char **env2, Request & request) {
//     (void)fd;
//     (void)env2;

//     // Command to execute the script using Python
//     std::string command = "python3 ." + request.GetFullUri() + " > tmp/response";

//     int fd_response = open("tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
//     if (fd_response == -1)  throw ErrorHttp("500 Internal Server Error", "/500");

//     if (request.GetMethod() == "POST") {
//         int fd_body = open("tmp/body", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
//         if (fd_body == -1)  throw ErrorHttp("500 Internal Server Error", "/500");
//         write(fd_body, request.GetContent().c_str(), sizeof(request.GetContent().c_str()));
//     }

//     std::cout << "Command = " << command << std::endl;
//     int result = std::system(command.c_str());

//     // Check the return value of the command
//     if (result != 0) {
//         throw ErrorHttp("500 Internal Server Error", "/500");
//     }

//     std::string fd_response_body = readFileToString("tmp/response");
//     return fd_response_body;
// }

std::string ft_ex_cgi(int fd, char **env2, Request & request) {
    int		status = 0;
    (void)env2;
    (void)fd;
//     (void)env2;

    std::vector<std::string> env = create_environment(request);

    //Convert environment vector to char* array
    // std::cout << "ENV:" << std::endl;
    // std::vector<char*> envp;
    // for (std::vector<std::string>::iterator it = env.begin(); it != env.end(); ++it) {
    //     envp.push_back(const_cast<char*>(it->c_str()));
    //     std::cout << *it << std::endl;
    // }
    // envp.push_back(NULL);

    char    *envc[env.size()];
    int j = 0;
    for (std::vector<std::string>::iterator it = env.begin();it != env.end();it++) {
        envc[j] = const_cast<char *>((*it).c_str());
        std::cout << "env: " << envc[j] << std::endl;
        j++;
    }
    envc[j] = NULL;

//  BUILDING PROGRAM
    std::string language = request.GetCgiExt();
    std::string program;
    // if (language == ".php")
    //     program = "/usr/bin/php-cgi";
	//std::cout << language << std::endl;
    if (language == ".php")
        program = "/usr/bin/php-cgi";
    else
        throw ErrorHttp("404 Not Found", request.error["404"]);

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
    // std::string program = "/usr/local/bin/php-cgi -c php.ini";
    // char **env = create_environment(request);
    // char **arguments = create_arguments(request, program);

    // char *envp2[] = {NULL};
    char *envp2[4];
    envp2[0] = const_cast<char *>(program.c_str());
    envp2[1] = const_cast<char *>("-c");
    envp2[2] = const_cast<char *>("./php.ini");
    envp2[3] = NULL;
//  EXECUTION OF CGI SCRIPT
    int fd_response = open("./tmp/response", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd_response == -1)  throw ErrorHttp("500 Internal Server Error", "/500");

    int input_pipe[2];
    if (pipe(input_pipe) == -1 ) throw ErrorHttp("500 Internal Server Error", "/500");
    // int input_pipe[2], output_pipe[2];
    // if (pipe(input_pipe) == -1 || pipe(output_pipe) == -1)  throw ErrorHttp("500 Internal Server Error", "/500");

    pid_t pid = fork();
    if (pid == -1)          throw ErrorHttp("500 Internal Server Error", "/500");


    if (pid == 0) {
        dup2(fd_response, 1);
         close(input_pipe[1]); // Close unused write end of input pipe
        // // close(output_pipe[0]); // Close unused read end of output pipe
        // // // Redirect stdin and stdout to the pipes
         dup2(input_pipe[0], 0);
        // // dup2(output_pipe[1], STDOUT_FILENO);

        // // close(STDIN_FILENO);
        // // close(STDOUT_FILENO);
        close(input_pipe[0]);
        close(fd_response);
        // close(input_pipe[1]);
        // sleep(10);
		std::cout << program.c_str() << std::endl;
        if (execve(program.c_str(), envp2, envc) == -1) exit(500);
		std::cout << "should not happen" << std::cout;
    } else {
        close(input_pipe[0]);
        // // close(output_pipe[1]);

        // // dup2(STDIN_FILENO, output_pipe[0]);
        //sleep(1);
        // if (request.GetMethod() == "POST")
        // write(input_pipe[1], request.GetBody().c_str(), request.GetBody().length()+1);

        write(input_pipe[1], request.GetBody().c_str(), request.GetBody().size());
        //write(input_pipe[1], "Helo", 5);
		close(input_pipe[1]);

        // char buffer[1024];
        // ssize_t bytes_read;

        // while ((bytes_read = read(output_pipe[0], buffer, sizeof(buffer))) > 0) {
        //     std::cout << "Read " << bytes_read << " bytes from output_pipe[0]." << std::endl;
        //     std::cout << buffer << std::endl;
        //     if (write(fd_response, buffer, bytes_read) == -1) {
        //         throw ErrorHttp("500 Internal Server Error", "/500");
        //     }
        // }
		if (waitpid(pid, &status, 0) == -1) {
			throw ErrorHttp("500 Internal Server Error", "/500");
    }
	if (waitpid(pid, &status, 0) == -1) {
		throw ErrorHttp("500 Internal Server Error", "/500");
    }
        // close(output_pipe[0]);
    }
    //close(fd_response);
    if (status == 500)
	{
		std::cout << "holaaa" << std::endl;
        throw ErrorHttp("500 Internal Server Error", "/500");
	}
	//std::cout << "Alguno llega aquí" << std::endl;
    std::string fd_response_body = "HTTP/1.1 200 OK\n" + readFileToString("tmp/response");
    return fd_response_body;

    // std::string response = "HTTP/1.1 200 OK" + fd_response_body;
    // return response;
    // std::string response = "HTTP/1.1 200 OK\r\n";
    // response += "Content-Type: text/html\r\n";
    // response += "Connection: close\r\n";
    // response += "Content-Length: " + ft_strlen(fd_response_body) + "\r\n";
    // response += fd_response_body;
    // return response;
}
