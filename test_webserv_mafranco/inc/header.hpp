#ifndef HEADER_HPP
# define HEADER_HPP

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/socket.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/stat.h>
# include <netinet/in.h>

# include <map>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>

# include "../conf/webserv.conf"
//# include "get_html_data.cpp"
//# include "send_html_page.cpp"
//# include "ft_memset.c"

# define MAX_EVENTS 10

void	    *ft_memset(void *s, int c, size_t n);

int         get_file(int fd);

int         get_site(std::map<std::string, std::string> *map);

std::string readFileToString(const std::string& filename);

void        action_html(int fd, std::string buff, std::map<std::string, std::string> *map);

#endif