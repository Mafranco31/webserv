#ifndef HEADER_HPP
# define HEADER_HPP

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/socket.h>
//# include <sys/event.h> //kqueue
#include <sys/epoll.h> //epoll
# include <sys/time.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/wait.h>

# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <algorithm>

#include <set>

# include "Server.hpp"
# include "Request.hpp"
# include "Sender.hpp"
# include "ErrorHttp.hpp"


# define MAX_EVENTS 100
# define BUFFER_SIZE 8192

void	    *ft_memset(void *s, int c, size_t n);

std::string ex_cgi(std::string cmd, int fd, char **env, std::string method);

std::string ft_ex_cgi2(int fd, char **env, Request & request);

std::string	ft_strlen( std::string s);

size_t	ft_strlcpy(char *dest, std::string src, size_t size);

#endif
