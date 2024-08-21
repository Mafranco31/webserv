#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <map>
#include <fcntl.h>

int main(void) {
    std::string str="Bonjour(1)(1).bjr";
    int nb=1;
    std::string rp = '(' + std::to_string(nb) + ')';

    if (str.find(rp, str.find('.') - rp.length()) != std::string::npos) {
        std::cout << "Trouvé ici : " << str.find(rp, str.find('.') - rp.length()) << std::endl;
    }

    return (0);
}