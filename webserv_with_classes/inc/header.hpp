#ifndef HEADER_HPP
# define HEADER_HPP

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <dirent.h>
//# include <sys/socket.h>
//# include <sys/event.h> //kqueue
# include <sys/epoll.h> //epoll
# include <sys/time.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <sys/wait.h>
#include <signal.h>

# include <map>
# include <vector>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <cstdlib>

#include <set>

# include "Server.hpp"
# include "Request.hpp"
# include "ErrorHttp.hpp"


# define MAX_EVENTS 100
# define BUFFER_SIZE 1000000

void	    *ft_memset(void *s, int c, size_t n);

std::string ft_ex_cgi2(Request request );

std::string	ft_itoa(int n);

std::string	ft_strlen( std::string s);

size_t	ft_strlcpy(char *dest, std::string src, size_t size);

std::string decodePercentEncoding(const std::string &encoded);

#endif
