
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main(void) {
    std::string firstLine = "GET   /   HTTP/1.1\n";
    
    size_t pos = firstLine.find(9 | 11 | 12 | 13 | 32);
	std::string method = firstLine.substr(0, pos);

	size_t pos2 = firstLine.find(!(9 & 11 & 12 & 13 & 32), pos + 1);
	pos = firstLine.find(9 | 11 | 12 | 13 | 32, pos2 + 1);
	std::string uri = content.substr(pos2, pos - pos2);

	pos2 = firstLine.find(!(9 & 11 & 12 & 13 & 32), pos + 1);
	pos = firstLine.find((9 | 11 | 12 | 13 | 32), pos2 + 1);
	std::string version = content.substr(pos2, pos - pos2);


	std::cout << "Method : " << method << std::endl;
	std::cout << "URI : " << uri << std::endl;
	std::cout << "Version : " << version << std::endl;


    return 0;
}