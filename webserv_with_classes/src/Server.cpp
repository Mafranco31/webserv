#include "Server.hpp"

//	Constructor
Server::Server ( Sender & s ) : sender(s) {
	std::cout << "Default Server constructor called" << std::endl;
}
//	Destructor
Server::~Server ( void ) {
	std::cout << "Destructor Server called" << std::endl;
}

//	Methods
void	Server::Start( void ) {
	// Creating the socket
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd == -1) {
		std::cerr << "Error creating socket" << std::endl;
		throw Server::ErrorCreatingSocket();
	}
	maxfd = serverfd;

	// Creating the struct sockaddr_in to use the socket
	ft_memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(80);

	// Binding the socket to the address
	if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
        close(serverfd);
		throw  Server::ErrorBindingSocket();
    }

	// Listening to the socket
	if (listen(serverfd, 100) == -1){
        close(serverfd);
		throw Server::ErrorListeningSocket();
    }

	// Creating the kqueue
	kq = kqueue();
    if (kq == -1) {
        close(serverfd);
		throw Server::ErrorCreatingKqueue();
    }

	// Setting the timeout for the kqueue
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    // Macro to set the kqueue evenet as read and write
	EV_SET(&change_event, serverfd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		throw Server::ErrorInitializeKqueue();
    }

	std::cout << "\033[1;32mServer started on port " << 8080 << "...\033[0m" << std::endl;
}

void	Server::Wait( void ) {
	// waiting for event
	nev = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
	if (nev == -1) {
		std::cerr << "Error: Could not get the new event." << std::endl;
	}
}

void	Server::ManageConnexion( void ) {
	//	Iterating over the events to manage the connections
	int	clientfd;
	for (int i = 0; i < nev; i++) {
		if (events[i].ident == static_cast<uintptr_t>(serverfd)) {
		// New connection on the server socket
			clientfd = accept(serverfd, NULL, NULL);
			if (clientfd == -1) {
				std::cerr << "Error: Cannot accept the new connection." << std::endl;
				continue;
			}
			fcntl(clientfd, F_SETFL, O_NONBLOCK);// Set the client socket as non-blocking
			std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;
		//	Add the new client socket to kqueue for monitoring
			EV_SET(&change_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
			if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
				close(clientfd);
				std::cerr << "Error: Cannot configure kevent for new connection." << std::endl;
			}
		}
		else if (events[i].filter == EVFILT_READ) {
		//	Data available to read from a client socket
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);
			if (bytes_read == 0) {
				std::cout << "\033[1;31mClient " << events[i].ident << " disconnected\033[0m" << std::endl;
				close(events[i].ident);
			}
			else if (bytes_read < 0)
				std::cerr << "Error: Could not read in the socket." << std::endl;
			else {
				buffer[bytes_read] = '\0';
				std::cout << "Received from client " << events[i].ident << ": " << std::endl;
				std::cout <<  buffer << "$" << std::endl;
				sender.Send(events[i].ident, buffer);
			}
		}
	}
}

void	Server::Stop( void ) {
	close(serverfd);
	std::cout << "\033[1;32mServer stopped on port " << serverfd << "...\033[0m" << std::endl;
}

//	Exceptions
const char *Server::ErrorReadingHtmlPath::what()		const throw()	{	return "Error: could not access to the html directory.";	}

const char *Server::ErrorCreatingSocket::what()		const throw()	{	return "Error: could not create the socket.";	}

const char *Server::ErrorBindingSocket::what()		const throw()	{	return "Error: could not bind the socket.";		}

const char *Server::ErrorListeningSocket::what()	const throw()	{	return "Error: could not listen the socket.";	}

const char *Server::ErrorAcceptingSocket::what()	const throw()	{	return "Error: could not accept the new connection.";	}

const char *Server::ErrorCreatingKqueue::what()		const throw()	{	return "Error: could not create kqueue instance.";		}

const char *Server::ErrorInitializeKqueue::what()	const throw()	{	return "Error: could not initialize kqueue instance.";		}

const char *Server::ErrorGettingEvent::what()		const throw()	{	return "Error: could not get the new event.";	}

const char *Server::ErrorReadingSocket::what()		const throw()	{	return "Error: could not read in the socket.";	}

const char *Server::ErrorSendingData::what()		const throw()	{	return "Error: could not send data to the client.";	}