#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "read_file.cpp"
#include <sys/event.h>
#include "ft_memset.c"
#include <fcntl.h>
#include <sys/time.h>


#define MAX_EVENTS 10

int    clients[1024];
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

int main(int	argc, char **argv)
{
    if (argc != 2)
        err("Wrong number of arguments");

    struct sockaddr_in  serveraddr;
    socklen_t           len;
	int	clientfd;
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) err(NULL);
    maxfd = serverfd;

	ft_memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
        close(serverfd);
		err("bind");
    }
	if (listen(serverfd, 100) == -1){
        close(serverfd);
		err("listen");
    }

	printf("Server listening on port %s\n", argv[1]);
	
    struct kevent change_event, events[MAX_EVENTS];
    int kq, nev;

	kq = kqueue();
    if (kq == -1) {
        close(serverfd);
		err("kqueue");
    }

	struct timespec timeout;
    timeout.tv_sec = 5;   // Timeout in seconds
    timeout.tv_nsec = 0;

	EV_SET(&change_event, serverfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	//EV_SET(&change_event, serverfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);//la
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		err("kevent");
    }

    while (1) {
		// waiting for event
        nev = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
        if (nev == -1) {
        	close(serverfd);
			err("kevent");
    	}

        for (int i = 0; i < nev; i++) {
            if (events[i].ident == serverfd) {
                // New connection on the server socket
                clientfd = accept(serverfd, NULL, NULL);
                if (clientfd == -1) continue; // err message

            	std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;

                // Add the new client socket to kqueue for monitoring
                EV_SET(&change_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    close(clientfd);
                    continue;
                }
				send(clientfd, send_buffer.c_str(), size_page, 0);
            } else if (events[i].filter == EVFILT_READ) {
                // Data available to read from a client socket
                char buffer[30000];
                ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);

                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    std::cout << "Received from client " << events[i].ident << ": " << std::endl;
                    // Echo back the message to the client
                    std::cout << buffer << std::endl << "End of transmission." << std::endl;

                } else if (bytes_read == 0) {
                    // Client disconnected
					std::cout << "\033[1;31mClient " << events[i].ident << " disconnected\033[0m" << std::endl;
                    close(events[i].ident);
                } else {
                    close(events[i].ident);
                }
            } else if (events[i].filter == EVFILT_WRITE) {
				send(events[i].ident, "YOOOO", 6, 0);
			}
        }
    }

    close(serverfd);
    return 0;
}