/*#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include "read_file.cpp"
#include "get_file.cpp"
#include <map>*/

#include "../inc/header.hpp"

void    action_html(int fd, std::string buff, std::map<std::string, std::string> *map) {
    int posSpace = buff.find(' ');
    std::string method = buff.substr(0, buff.find(' '));
    std::string page = buff.substr(posSpace + 1, buff.find(' ', posSpace + 1) - posSpace - 1);

    std::cout << "\033[1;34mClient " << fd << " asked a " << method << " to :" << page << "$\033[0m" << std::endl;

    std::map<std::string, std::string>::iterator it = map->find(page);

   /*std::string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Connection: close\r\n"; // Use `keep-alive` for persistent connections
        //response += "\r\n";*/

    std::string response;

    if (!(method == "GET" || method == "HEAD" || method == "POST" || method == "DELETE")) {
        response = "HTTP/1.1 405 Method Not Allowed\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)["/405"].length()) + "\r\n\r\n";
        response += (*map)["/405"];
        send(fd, response.c_str(), response.length(), 0);
    } else if (method == "POST") {
        if (!get_file(fd)) {
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
    } else if (it == map->end()) {
        response = "HTTP/1.1 404 Not Found\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)["/404"].length()) + "\r\n\r\n";
        if (method == "GET") response += (*map)["/404"];
        send(fd, response.c_str(), response.length(), 0);
    } else {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string((*map)[page].length()) + "\r\n\r\n";
        if (method == "GET") response += (*map)[page];
        send(fd, response.c_str(), response.length(), 0);
    }
    //std::cout << response << std::endl;
    //close(fd);
}







/*std::string send_buffer_index = readFileToString("www/index.html");
size_t      size_page_index = send_buffer_index.length();

std::string send_buffer = readFileToString("www/story.html");
size_t      size_page = send_buffer.length();

std::string send_buffer_bad = readFileToString("errwww/404.html");
size_t      size_page_bad = send_buffer_bad.length();



void    send_html_page(int fd, std::string buff) {
    int posSpace = buff.find(' ');
    std::string page = buff.substr(posSpace + 1, buff.find(' ', posSpace + 1) - posSpace - 1);

    std::cout << "\033[1;34mClient " << fd << " asked a GET to :" << page << "$\033[0m" << std::endl;

    std::string path = "www" + page + ".html";
    struct stat bufstat;

    if (page == "/") {
        send(fd, send_buffer_index.c_str(), size_page_index, 0);
        std::cout << "\033[1;32mIndex page sent\033[0m" << std::endl;
        return;
    }


    if (stat(path.c_str(), &bufstat) == -1) {
        send(fd, send_buffer_bad.c_str(), size_page_bad, 0);
        std::cout << "\033[1;31mBad path\033[0m" << std::endl;
        return ;
    }
    if (S_ISDIR(bufstat.st_mode)) {
        send(fd, send_buffer_bad.c_str(), size_page_bad, 0);
        std::cout << "\033[1;31mDirectory asked\033[0m" << std::endl;
    }
    else {
        send(fd, send_buffer.c_str(), size_page, 0);
        std::cout << "\033[1;32m" << page << " page sent\033[0m" << std::endl;
    }
}*/