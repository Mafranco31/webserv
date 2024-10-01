#include "../inc/header.hpp"

int     download_file(int fd, std::map<std::string, std::string> *map, std::string page) {
    int errcode = 0;

    (void)map;
    std::string     dir_uploads(WB_DIR_UPLOADS);
    std::string search_string = "download.php?file=";
    if (page.find(search_string) == std::string::npos) return (404);
    size_t start_pos = page.find(search_string);
    start_pos += search_string.length();  // Move start_pos to the end of "delete.php?file="
    size_t end_pos = page.find(" ", start_pos);
    std::string file_path = page.substr(start_pos, end_pos - start_pos);
    file_path = decode_filepath(file_path);
    if (file_path == "") return 400;
    std::string response;

    std::string file_str;

    if (chdir(WB_DIR_UPLOADS) == -1){
        std::cout << "Error trying to go to directory: " << WB_DIR_UPLOADS << " from /" << std::endl;
        return (500);
    }

    DIR *dir = opendir(".");
    int found = 0;

    struct dirent   *dirent;
    do {
        dirent = readdir(dir);
        if (dirent) {
            std::string ndir(dirent->d_name);
            if (dirent->d_type == DT_REG) {
                if (ndir == file_path) {
                    file_str = readFileToString(file_path);
                    found = 1;
                    break;
                }
            }
        }
    } while (dirent);

    if (chdir("..") == -1){
        std::cout << "Error trying to go to directory .. from : " << WB_DIR_UPLOADS << " from /" << std::endl;
        return (500);
    }

    if (found == 1) {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Connection: close\r\n";
        response += "Content-Length: " + std::to_string(file_str.length()) + "\r\n\r\n";
        response += file_str;
        send(fd, response.c_str(), response.length(), 0);
    } else errcode = 404;
    return (errcode);
}