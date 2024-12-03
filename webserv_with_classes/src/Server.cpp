#include "Server.hpp"
#include <arpa/inet.h>

//	Constructor
Webserv::Webserv ( Sender & s ) : sender(s), serv(NULL), serv_n(0){
	valid_directives.insert("listen");
	valid_directives.insert("root");
	valid_directives.insert("index");
	valid_directives.insert("server_name");
	valid_directives.insert("allow_methods");

	valid_directives_location.insert("root");
	valid_directives_location.insert("alias");
	valid_directives_location.insert("allow_methods");
	valid_directives_location.insert("client_body_buffer_size");
	valid_directives_location.insert("index");
	valid_directives_location.insert("cgi_pass");
	std::cout << "Default Server constructor called" << std::endl;
}
//	Destructor
Webserv::~Webserv ( void ) {
	std::cout << "Destructor Server called" << std::endl;
}

//	Methods
void	Webserv::Start( void ) {
	// Creating the socket
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd == -1) {
		std::cerr << "Error creating socket" << std::endl;
		throw Webserv::ErrorCreatingSocket();
	}
	maxfd = serverfd;

	// Creating the struct sockaddr_in to use the socket
	ft_memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr("ip address");
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//int port;
	//std::stringstream ss(_port[i]);
	//ss >> port;
    serveraddr.sin_port = htons(8080);

	// Binding the socket to the address
	if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
        close(serverfd);
		throw  Webserv::ErrorBindingSocket();
    }

	// Listening to the socket
	if (listen(serverfd, 100) == -1){
        close(serverfd);
		throw Webserv::ErrorListeningSocket();
    }
	//KQUEUE
	/*
	// Creating the kqueue
	kq = kqueue();
    if (kq == -1) {
        close(serverfd);
		throw Webserv::ErrorCreatingKqueue();
    }

	// Setting the timeout for the kqueue
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;

    // Macro to set the kqueue evenet as read and write
	EV_SET(&change_event, serverfd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		throw Webserv::ErrorInitializeKqueue();
    }
	*/
	//EPOLL
	ep = epoll_create(1);
	if (ep == -1)
	{
		close(serverfd);
		throw Webserv::ErrorCreatingKqueue(); //Change name.
	}
	change_event.data.fd = serverfd;
	change_event.events = EPOLLIN;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, serverfd, &change_event) == -1)
	{
		close(serverfd);
		throw Webserv::ErrorInitializeKqueue(); //Change name.
	}
	std::cout << "\033[1;32mServer started on port " << 8080 << "...\033[0m" << std::endl;
}

void	Webserv::Wait( void ) {
	// waiting for event
	/*nev = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
	if (nev == -1) {
		std::cerr << "Error: Could not get the new event." << std::endl;
	}*/
	nev = epoll_wait(ep, events, MAX_EVENTS, 1000); //Poner -1?
	if (nev == -1)
		std::cerr << "Error: Could not get the new event." << std::endl;
}

void	Webserv::ManageConnexion( void ) {
	//	Iterating over the events to manage the connections
	/*
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
	*/
	int	clientfd;
	for (int i = 0; i < nev; i++)
	{
		if (events[i].data.fd == serverfd)
		{
			clientfd = accept(serverfd, NULL, NULL);
			if (clientfd == -1) {
				std::cerr << "Error: Cannot accept the new connection." << std::endl;
				continue;
			}
			fcntl(clientfd, F_SETFL, O_NONBLOCK);// Set the client socket as non-blocking
			std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;
			//	Add the new client socket to kqueue for monitoring
			change_event.data.fd = clientfd;
			change_event.events = EPOLLIN;
			if (epoll_ctl(ep, EPOLL_CTL_ADD, clientfd, &change_event) == -1)
			{
				close(clientfd);
				std::cerr << "Error: Cannot configure kevent for new connection." << std::endl;
			}
		}
		else if (events[i].events & EPOLLIN) //There's an EPOLLIN event.
		{
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
			if (bytes_read == 0)
			{
				std::cout << "\033[1;31mClient " << events[i].data.fd << " disconnected\033[0m" << std::endl;
				change_event.data.fd = events[i].data.fd;
				change_event.events = EPOLLIN;
				if (epoll_ctl(ep, EPOLL_CTL_DEL, events[i].data.fd, &change_event) == -1)
					std::cout << "Couldn't delete event" << std::endl;
				close(events[i].data.fd);
			}
			else if (bytes_read < 0)
				std::cerr << "Error: Could not read in the socket." << std::endl;
			else
			{
				buffer[bytes_read] = '\0';
				std::cout << "Received from client " << events[i].data.fd << ": " << std::endl; //Create a structure for clients to identify them by a number, and not its fd.
				std::cout <<  buffer << "$" << std::endl;
				sender.Send(events[i].data.fd, buffer);
			}
		}
	}
}

void	Webserv::Stop( void ) {
	close(serverfd);
	std::cout << "\033[1;32mServer stopped on port " << serverfd << "...\033[0m" << std::endl;
}

//	Exceptions
const char *Webserv::ErrorReadingHtmlPath::what()		const throw()	{	return "Error: could not access to the html directory.";	}

const char *Webserv::ErrorCreatingSocket::what()		const throw()	{	return "Error: could not create the socket.";	}

const char *Webserv::ErrorBindingSocket::what()		const throw()	{	return "Error: could not bind the socket.";		}

const char *Webserv::ErrorListeningSocket::what()	const throw()	{	return "Error: could not listen the socket.";	}

const char *Webserv::ErrorAcceptingSocket::what()	const throw()	{	return "Error: could not accept the new connection.";	}

const char *Webserv::ErrorCreatingKqueue::what()		const throw()	{	return "Error: could not create kqueue instance.";		}

const char *Webserv::ErrorInitializeKqueue::what()	const throw()	{	return "Error: could not initialize kqueue instance.";		}

const char *Webserv::ErrorGettingEvent::what()		const throw()	{	return "Error: could not get the new event.";	}

const char *Webserv::ErrorReadingSocket::what()		const throw()	{	return "Error: could not read in the socket.";	}

const char *Webserv::ErrorSendingData::what()		const throw()	{	return "Error: could not send data to the client.";	}
