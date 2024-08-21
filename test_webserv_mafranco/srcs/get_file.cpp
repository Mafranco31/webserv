#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include "../conf/webserv.conf"
#include <map>
#include <fcntl.h>

int get_file(int fd) {
    char buffer[30000];
    std::string buff;
    ssize_t bytes_read;

    bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    std::string s_buffer(buffer);
    buff += s_buffer;
    
    std::cout << "Received from client " << fd << ":\n" << buff << std::endl;

    std::string boundid = buff.substr(0, buff.find('\n'));
    size_t start_filename = buff.find("filename=\"") + 10;

    std::string filename = buff.substr(start_filename, buff.find('\n', start_filename) - start_filename - 2);

    std::cout << "bound id: " << boundid << std::endl << "filename: " << filename << std::endl;

    std::string dir_uploads(WB_DIR_UPLOADS);

    if (chdir(dir_uploads.c_str()) == -1) {
        std::cout << "Error trying to go to directory: " << dir_uploads << " from /" << std::endl;
        return (-1);
    }
    
    DIR *dir = opendir(".");
    if (!dir) {
        std::cout << "Error opening directory: " << dir_uploads << " from /" << std::endl;
        return (-1);
    }

    struct dirent   *dirent;

    int nb_file = 1;
    size_t ppose;
    size_t  tmppose;

    do {
        tmppose = 0;
        dirent = readdir(dir);
        std::string num_file_bf = '(' + std::to_string(nb_file - 1) + ')';
        std::string num_file = '(' + std::to_string(nb_file) + ')';
        if (dirent) {
            std::string ndir(dirent->d_name);
            if (ndir == "." || ndir == "..") continue;
            if (ndir == filename) {
                // find last '.' in filename
                do {
                    ppose = tmppose;
                    tmppose = filename.find('.', ppose + 1);
                } while (tmppose != std::string::npos);
                // find if there is already a (*nb*) in the filename to increment it
                if (nb_file > 1 && filename.find(num_file_bf, ppose - num_file_bf.length() - 1) != std::string::npos)
                    filename = filename.substr(0, ppose - num_file_bf.length()) + num_file + filename.substr(ppose);
                else
                    filename = filename.substr(0, ppose) + num_file + filename.substr(ppose, filename.length() - ppose);
                nb_file += 1;
                // re execute the opendir to check all files with the new filename
                dir = opendir(".");
                if (!dir) {
                    std::cout << "Error opening directory: " << dir_uploads << " from /" << std::endl;
                    return (-1);
                }
            }
        }
    } while (dirent);

    filename = filename;

    if (open(filename.c_str(), O_CREAT, S_IRWXU) == -1) {
        std::cout << "Error creating " << filename << std::endl;
        return (-1);
    }

    if (chdir("..") == -1){
        std::cout << "Error trying to go to directory: .. from " <<  dir_uploads << std::endl;
        return (-1);
    }

    return (0);
}