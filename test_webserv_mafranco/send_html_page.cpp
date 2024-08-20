#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include "read_file.cpp"
#include <map>

void    send_html_page(int fd, std::string buff, std::map<std::string, std::string> *map) {
    int posSpace = buff.find(' ');
    std::string page = buff.substr(posSpace + 1, buff.find(' ', posSpace + 1) - posSpace - 1);

    std::cout << "\033[1;34mClient " << fd << " asked a GET to :" << page << "$\033[0m" << std::endl;

    std::map<std::string, std::string>::iterator it = map->find(page);
    if (it == map->end()) {
        send(fd, (*map)["/404"].c_str(), (*map)["/404"].length(), 0);
    } else {
        send(fd, (*map)[page].c_str(), (*map)[page].length(), 0);
    }
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