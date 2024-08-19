#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "read_file.cpp"

int    clients[1024];
fd_set      read_set, write_set, current;
int         maxfd = 0, gid = 0;
std::string        send_buffer = readFileToString("index.html");

size_t  size_page = send_buffer.length();

void    err(char  *msg)
{
    if (msg)
        write(2, msg, strlen(msg));
    else
        write(2, "Fatal error", 11);
    write(2, "\n", 1);
    exit(1);
}

int     main(int ac, char **av)
{
    if (ac != 2)
        err("Wrong number of arguments");

    struct sockaddr_in  serveraddr;
    socklen_t           len;
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) err(NULL);
    maxfd = serverfd;

    FD_ZERO(&current);
    FD_SET(serverfd, &current);
    bzero(clients, sizeof(clients));
    bzero(&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(av[1]));

    if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1 || listen(serverfd, 100) == -1)
        err(NULL);
    printf("Server FD: %d\n", serverfd);
    //read_set = write_set = current;
    while (1)
    {
        read_set = write_set = current;
        if (select(maxfd + 1, &read_set, &write_set, 0, 0) == -1) continue;
        for (int fd = 2; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                if (fd == serverfd)
                {
                    int clientfd = accept(serverfd, (struct sockaddr *)&serveraddr, &len);
                    if (clientfd == -1) continue;
                    if (clientfd > maxfd) maxfd = clientfd;
                    clients[clientfd] = gid++;
                    FD_SET(clientfd, &current);
                    send(clientfd, send_buffer.c_str(), size_page, 0);
                }
                break;
            }
        }
    }
    return (0);
}