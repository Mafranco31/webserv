#include "../inc2/header.hpp"

std::string decode_filepath(std::string &path) {
    std::string new_path;
    std::string num;
    char    c;
    for (    std::string::iterator   it = path.begin();it != path.end();it++) {
        if (*it == '+')
            new_path += ' ';
        else if (*it == '%') {
            it++;
            if (*it == 'E' || *it == 'C') return "";
            num = "";
            num += *it;
            it++;
            num += *it;
            c = std::stoi(num, nullptr, 16);
            if (c < 32 || c > 126) return "";
            new_path += c;
        } else
            new_path += *it;
    }
    return new_path;
}