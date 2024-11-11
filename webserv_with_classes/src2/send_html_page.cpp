#include "../inc2/header.hpp"

void    action_html(int fd, std::string buff, std::map<std::string, std::string> *map, char **env) {
    //  Getting the method of the request (GET, HEAD, POST, DELETE...)
    int posSpace = buff.find(' ');
    std::string method = buff.substr(0, buff.find(' '));
    std::string page = buff.substr(posSpace + 1, buff.find(' ', posSpace + 1) - posSpace - 1);
    std::cout << "\033[1;34mClient " << fd << " asked a " << method << " to :" << page << "$\033[0m" << std::endl;

    std::string response;
    int errcode = 0;

    
    if ((method == "GET" || method == "HEAD" || method == "POST" || method == "DELETE")) {
        if (method == "POST") {
            errcode = get_file(fd, map);
        } else if (method == "DELETE") {
            errcode = delete_file(fd, map, page);
        } else {
            std::map<std::string, std::string>::iterator it = map->find(page);
            if (it != map->end()) {
                if ((*map)[page].length() < 100) {
                    std::cout << "\033[1;31mCOMMING IN EX_CGI\033[0m" << std::endl;
                    errcode = ex_cgi((*map)[page], fd, env, method);
                } else {
                    response = "HTTP/1.1 200 OK\r\n";
                    response += "Content-Type: text/html\r\n";
                    response += "Connection: close\r\n";
                    response += "Content-Length: " + std::to_string((*map)[page].length()) + "\r\n\r\n";
                    if (method == "GET")
                        response += (*map)[page];
                    send(fd, response.c_str(), response.length(), 0);
                }
            } else if (method == "GET" && page.find("download.php?file=") != std::string::npos) {
                errcode = download_file(fd, map, page);
            } else errcode = 404;
        }
    } else errcode = 501;

    if (errcode != 0) {
        std::string errcode_str = std::to_string(errcode);
        response = "HTTP/1.1 " + errcode_str + " " + get_str_errcode(errcode) + "\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        errcode_str = '/' + errcode_str;
        response += "Content-Length: " + std::to_string((*map)[errcode_str].length()) + "\r\n\r\n";
        response += (*map)[errcode_str];
        send(fd, response.c_str(), response.length(), 0);
    }
}

    //  looking if the url asked is a existing page of our website
    /*std::map<std::string, std::string>::iterator it = map->find(page);

    //  If the method does not exists, send a error 405
    if (!(method == "GET" || method == "HEAD" || method == "POST" || method == "DELETE")) {
        response = "HTTP/1.1 501 Not Implemented\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)["/501"].length()) + "\r\n\r\n";
        response += (*map)["/501"];
        send(fd, response.c_str(), response.length(), 0);
    //  If the method is POST, try to create the file and return 201 if works or 400 otherwise
    } else if (method == "POST") {
        if (!get_file(fd, map, page)) {
            response = "HTTP/1.1 201 Created\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Connection: close\r\n";
            response += "Content-Length: " + std::to_string((*map)[page].length()) + "\r\n\r\n";
            response += (*map)[page];
            send(fd, response.c_str(), response.length(), 0);
        } else {
            response = "HTTP/1.1 400 Bad Request\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Connection: close\r\n";
            response += "Content-Length: " + std::to_string((*map)["/400"].length()) + "\r\n\r\n";
            response += (*map)["/400"];
            send(fd, response.c_str(), response.length(), 0);
        }
    //  If the method is GET, if the webpage does not exist, send a error 404 Not Found
    } else if (it == map->end()) {
        response = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)["/404"].length()) + "\r\n\r\n";
        if (method == "GET") response += (*map)["/404"];
        send(fd, response.c_str(), response.length(), 0);
    //  Otherwise, we send the webpage that exist with status code 200 OK
    } else {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)[page].length()) + "\r\n\r\n";
        if (method == "GET") response += (*map)[page];
        send(fd, response.c_str(), response.length(), 0);
    }
}*/