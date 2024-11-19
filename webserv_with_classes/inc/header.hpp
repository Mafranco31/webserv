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
# include <vector>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>

# include "Server.hpp"
# include "Request.hpp"
# include "Sender.hpp"
# include "ErrorHttp.hpp"

# define MAX_EVENTS 100
# define BUFFER_SIZE 8192

void	    *ft_memset(void *s, int c, size_t n);

#endif