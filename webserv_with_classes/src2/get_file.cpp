#include "../inc2/header.hpp"

int filename_check(std::string filename) {
    for (std::string::iterator it = filename.begin(); it != filename.end(); it++) {
        if (*it < 32 || *it > 126) return (1);
    }
    return (0);
}

int get_file(int fd, std::map<std::string, std::string> *map) {
    char buffer[30000];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read == 0) {
        std::cout << "Error trying to read file posted" << std::endl;
        return (-1);
    }
    std::string buff(buffer);
    
    std::cout << "Received from client " << fd << ":\n" << buff << std::endl;

    std::string boundid = buff.substr(0, buff.find('\n'));
    size_t start_filename = buff.find("filename=\"") + 10;

    std::string filename = buff.substr(start_filename, buff.find('\n', start_filename) - start_filename - 2);

    if (filename_check(filename)) return 400;
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


    std::ofstream outfile(filename);
    if (outfile.is_open()) {
        int pos_start_file = buff.find("\r\n\r\n") + 4;
        int pos_end_file = buff.find(boundid, pos_start_file);
        std::string file_content = buff.substr(pos_start_file, pos_end_file - pos_start_file - 3);
        std::cout << "BUFF IV FOUND: " << file_content << "$" << std::endl;
        outfile << file_content;
        outfile.close();

        std::cout << "Data has been written to the file." << std::endl;
    } else {
        std::cout << "Error creating " << filename << std::endl;
        return (-1);
    }

    if (chdir("..") == -1){
        std::cout << "Error trying to go to directory: .. from " <<  dir_uploads << std::endl;
        return (-1);
    }
    
    std::string response = "HTTP/1.1 201 Created\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string((*map)["/upload"].length()) + "\r\n\r\n";
    response += (*map)["/upload"];
    send(fd, response.c_str(), response.length(), 0);
    
    return (0);
}