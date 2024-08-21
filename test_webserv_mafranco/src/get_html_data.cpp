/*#include <map>
#include <string>
#include <iostream>
#include "../conf/webserv.conf"
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "read_file.cpp"*/
//#include "read_file.cpp"

#include "../inc/header.hpp"


/*  drop comments to do a recursive search over all subdirectories  */
int    read_path_content(char *path, std::map<std::string, std::string> *map) {
    if (chdir(path) == -1){
        std::cout << "Error trying to go to directory: " << path << " from /" << std::endl;
        return (-1);
    }
    
    DIR *dir = opendir(".");

    struct dirent   *dirent;

    //int f_redirect = 0;

    std::string s_path(path);
    if (s_path.at(s_path.length() - 1) != '/')
        s_path += '/';

    do {
        dirent = readdir(dir);
        if (dirent) {
            std::string ndir(dirent->d_name);
            if (ndir == "." || ndir == "..") continue;
            std::string n_path = s_path + ndir;
            if (dirent->d_type == DT_REG) {
                //std::cout << dirent->d_name << std::endl;
                if (ndir.substr(ndir.length() - 5) == ".html") {
                    //if (f_redirect == 0) (*map)[path] = ndir;
                    //if (ndir == "index.html") f_redirect = 1;
                    if (ndir == "index.html")(*map)["/"] = n_path;
                    (*map)['/' + ndir.substr(0, ndir.length() - 5)] = n_path;
                }
            }/*
            else if (dirent->d_type == DT_DIR){
                std::cout << dirent->d_name << std::endl;
                if (read_path_content(dirent->d_name, map) == -1) return (-1);
            }*/
        }
    } while (dirent);

    if (chdir("..") == -1){
        std::cout << "Error trying to go to directory: .. from " << path << std::endl;
        return (-1);
    }
    return (0);
}


int get_site(std::map<std::string, std::string> *map) {
    if (read_path_content((char*)WB_DIR_PATH, map) == -1) return (-1);
    if (read_path_content((char*)"errwww", map) == -1) return (-1);

    for (std::map<std::string, std::string>::iterator it = map->begin(); it != map->end(); ++it) {
        std::string buffer = readFileToString(it->second);
        (*map)[it->first] = buffer;
    }
    
    return (0);
}