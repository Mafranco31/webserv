#include "../inc/header.hpp"

int    clients[1024];
int         maxfd = 0, gid = 0;

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
        err((char*)"Wrong number of arguments");

    struct sockaddr_in  serveraddr;

    // map our html files
	std::map<std::string, std::string>	*site_data = new std::map<std::string, std::string>;
	if (get_site(site_data) == -1) err ((char*)"Wrong html directory in configuration");// in get_html_data, get the html.
	
	//	Print the map
	/*for (std::map<std::string, std::string>::iterator it = site_data->begin(); it != site_data->end(); ++it) {
        std::cout << "Key: " << it->first << " -> Value: " << it->second << std::endl;
    }*/

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
		err((char*)"bind");
    }
	if (listen(serverfd, 100) == -1){
        close(serverfd);
		err((char*)"listen");
    }

	printf("Server listening on port %s\n", argv[1]);
	
    struct kevent change_event, events[MAX_EVENTS];
    int kq, nev;

	kq = kqueue();
    if (kq == -1) {
        close(serverfd);
		err((char*)"kqueue");
    }

	struct timespec timeout;
    timeout.tv_sec = 5;   // Timeout in seconds
    timeout.tv_nsec = 0;

    // Macro to set the kqueue evenet as read and write
	EV_SET(&change_event, serverfd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		err((char*)"kevent");
    }

    while (1) {
		// waiting for event
        nev = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
        if (nev == -1) {
        	close(serverfd);
			err((char*)"kevent");
    	}

        for (int i = 0; i < nev; i++) {
            if (events[i].ident == static_cast<uintptr_t>(serverfd)) {
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
				//send(clientfd, send_buffer.c_str(), size_page, 0);
            } else if (events[i].filter == EVFILT_READ) {
                // Data available to read from a client socket
                char buffer[30000];
                ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);

                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
					// Echo back the message to the client
                    std::cout << "Received from client " << events[i].ident << ": " << std::endl;
					std::cout << buffer << std::endl;
					// Send the asked html page to the client
					std::string path(buffer, 100);

					action_html(events[i].ident, path, site_data);
                } else {
                    // Client disconnected
					std::cout << "\033[1;31mClient " << events[i].ident << " disconnected\033[0m" << std::endl;
                    close(events[i].ident);
                }
            } else if (events[i].filter == EVFILT_WRITE) {
				continue;
				//send(events[i].ident, "YOOOO", 6, 0);
			}
        }
    }

    close(serverfd);
    return 0;
}