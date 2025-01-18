#include "Server.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>

Server::Server(char **env, Webserv *ws, std::string host,std::string port, int &serv_ep, int &serv_nev): _env(env), _ws(ws), _host(host), ep(serv_ep), nev(serv_nev)
{
	std::stringstream ss(port);
	ss >> _port;
	//std::cout << _port << std::endl;
}

Server::~Server()
{

}

//	Constructor
Webserv::Webserv () : serv(NULL), serv_n(0), tmp_prefix("") {
	valid_directives.insert("listen");
	valid_directives.insert("root");
	valid_directives.insert("index");
	valid_directives.insert("autoindex");
	valid_directives.insert("server_name");
	valid_directives.insert("error_page");
	valid_directives.insert("client_max_body_size");

	valid_directives_location.insert("root");
	valid_directives_location.insert("return");
	valid_directives_location.insert("index");
	valid_directives_location.insert("autoindex");
	valid_directives_location.insert("limit_except"); //allow methods
	valid_directives_location.insert("error_page");
	valid_directives_location.insert("client_max_body_size");
	//proxy_pass?
	// Setting the timeout for the kqueue

	//KQUEUE
	//timeout.tv_sec = 5;
	//timeout.tv_nsec = 0;

	std::cout << "Default Server constructor called" << std::endl;
}
//	Destructor
Webserv::~Webserv ( void ) {
	std::cout << "Destructor Server called" << std::endl;
}

//	Methods
void	Server::Start( void ) {
	// Creating the socket
	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd == -1) {
		std::cerr << "Error creating socket" << std::endl;
		throw Webserv::ErrorCreatingSocket();
	}
	maxfd = serverfd;
	int opt = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error configurating the socket" << std::endl;
		throw Webserv::ErrorCreatingSocket();
	}

	// Creating the struct sockaddr_in to use the socket
	ft_memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr("ip address");
	//Ahora mismo el host no importa. Resivarlo mas adelante.
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//int port;
	//std::stringstream ss(_port[i]);
	//ss >> port;
    serveraddr.sin_port = htons(this->_port);

	// Binding the socket to the address
	if (bind(serverfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
		std::cout << strerror(errno) << std::endl;
        close(serverfd);
		std::cout << "port = " << this->_port << std::endl;
		throw  Webserv::ErrorBindingSocket();
    }

	// Listening to the socket
	if (listen(serverfd, 100) == -1){
        close(serverfd);
		throw Webserv::ErrorListeningSocket();
    }
	//KQUEUE
	/*
    // Macro to set the kqueue evenet as read and write
	EV_SET(&change_event, serverfd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(ep, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		throw Webserv::ErrorInitializeKqueue();
    }
	*/
	//EPOLL
	//change_event = {};
	ft_memset(&change_event, 0, sizeof(change_event));
	change_event.data.fd = serverfd;
	change_event.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(ep, EPOLL_CTL_ADD, serverfd, &change_event) == -1)
	{
		close(serverfd);
		throw Webserv::ErrorInitializeKqueue(); //Change name.
	}
	std::cout << "\033[1;32mServer started on port " << _port << "...\033[0m" << std::endl;
}

void	Webserv::Wait( void ) {
	nev = epoll_wait(ep, events, MAX_EVENTS, 1000); //Poner -1?
	if (nev == -1)
	{
		//std::cout << strerror(errno) << std::endl;
		std::cerr << "Error: Could not get the new event." << std::endl;
	}
	//std::cout << nev << std::endl;
}

void	Server::ManageConnexion( struct epoll_event *events) {
	//	Iterating over the events to manage the connections
	int	clientfd;
	//std::cout << nev << std::endl;
	//this->Wait();
	for (int i = 0; i < nev; i++)
	{
		//this->Wait();
		if ((events)[i].data.fd == serverfd)
		{
			std::cout << "connection" << std::endl;
			clientfd = accept(serverfd, NULL, NULL);
			if (clientfd == -1) {
				std::cerr << "Error: Cannot accept the new connection." << std::endl;
				continue;
			}
			fcntl(clientfd, F_SETFL, O_NONBLOCK);// Set the client socket as non-blocking
			fds.push_back(clientfd);
			std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;
			//	Add the new client socket to kqueue for monitoring
			change_event.data.fd = clientfd;
			change_event.events = EPOLLIN | EPOLLOUT;
			epmap[clientfd] = "";
			if (epoll_ctl(ep, EPOLL_CTL_ADD, clientfd, &change_event) == -1)
			{
				close(clientfd);
				std::cerr << "Error: Cannot configure kevent for new connection." << std::endl;
			}
		}
		else if ((events[i].events & EPOLLIN) && std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end()) //There's an EPOLLIN event.
		{
			//if (std::find(fds.begin(), fds.end(), events[i].data.fd) == fds.end())
			//	std::cout << "other" << std::endl;
			//else
			//	std::cout << "es: " << *std::find(fds.begin(), fds.end(), events[i].data.fd) << std::endl;
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
			if (bytes_read == 0)
			{
				std::vector<int>::iterator pos_fd = std::find(fds.begin(), fds.end(), events[i].data.fd);
				fds.erase(pos_fd);
				std::cout << "\033[1;31mClient " << events[i].data.fd << " disconnected\033[0m" << std::endl;
				change_event.data.fd = events[i].data.fd;
				change_event.events = EPOLLIN | EPOLLOUT;
				if (epoll_ctl(ep, EPOLL_CTL_DEL, events[i].data.fd, &change_event) == -1)
					std::cout << "Couldn't delete event" << std::endl;
				// _ws->Send(events[i].data.fd, epmap[events[i].data.fd], _env);
				epmap[events[i].data.fd] = "";
				close(events[i].data.fd);
			}
			else if (bytes_read < 0)
			{
				//std::cout << strerror(errno) << std::endl;
				std::cerr << "Error: Could not read in the socket." << std::endl;
			}
			else
			{
				std::string data = "";
				// while (bytes_read > 0){
					buffer[bytes_read] = '\0';
					data.append(buffer);
				// 	bytes_read = read(events[i].data.fd, buffer, sizeof(buffer) - 1);
				// }
				std::cout << "Received from client " << events[i].data.fd << ": " << std::endl; //Create a structure for clients to identify them by a number, and not its fd.
				std::cout << data << "$" << std::endl;
				//if (epmap[events[i].data.fd] != ""){
					epmap[events[i].data.fd] = epmap[events[i].data.fd] + data;
				//}
				std::cout << data << "$" << std::endl;
				std::cout << "still arrives here." << std::endl;
				break ;
				// epmap[events[i].data.fd].append(data);
				// if ((events[i].events & EPOLLET)){
				// 	std::cout << "EPOLLEEEEET" << std::endl;}
			}
		}
		else if ((events[i].events & EPOLLOUT) && epmap[events[i].data.fd] != "" && std::find(fds.begin(), fds.end(), events[i].data.fd) != fds.end())
		{
			std::cout << "Ready to write!!!!!" << std::endl;
			int ret =_ws->Send(events[i].data.fd, epmap[events[i].data.fd] , _env);
			if (ret == 2)
				continue ;
			else if (ret == 1)
				epmap[events[i].data.fd] = "";
			else
				close(events[i].events);
			break ;
		}
	}
}
/*
void	Webserv::Wait( void ) {
	// waiting for event
	nev = kevent(ep, NULL, 0, events, MAX_EVENTS, &timeout);
	//std::cout <<"\033[1;33mEvent found : " << nev << "\033[0m" << std::endl;
	if (nev == -1) {
		std::cerr << "Error: Could not get the new event." << std::endl;
	}
}

void	Server::ManageConnexion( struct kevent *events) {
	//	Iterating over the events to manage the connections
	int	clientfd;
	for (int i = 0; i < nev; i++) {
		if (events[i].ident == static_cast<uintptr_t>(serverfd))
		{
			std::cout << "connection" << std::endl;
			clientfd = accept(serverfd, NULL, NULL);
			if (clientfd == -1) {
				std::cerr << "Error: Cannot accept the new connection." << std::endl;
				continue;
			}
			fcntl(clientfd, F_SETFL, O_NONBLOCK);// Set the client socket as non-blocking
			fds.push_back(clientfd);
			std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;
		//	Add the new client socket to kqueue for monitoring
			EV_SET(&change_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
			if (kevent(ep, &change_event, 1, NULL, 0, NULL) == -1) {
				close(clientfd);
				std::cerr << "Error: Cannot configure kevent for new connection." << std::endl;
			}
		}
		else if (events[i].filter == EVFILT_READ && std::find(fds.begin(), fds.end(), events[i].ident) != fds.end()) {
		//	Data available to read from a client socket
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);
			if (bytes_read == 0) {
				std::cout << "\033[1;31mClient " << events[i].ident << " disconnected\033[0m" << std::endl;
				close(events[i].ident);
			}
			else if (bytes_read < 0)
				std::cerr << "Error: Could not read in the socket." << std::endl;
			else
			{
				std::string data = "";
				while (bytes_read > 0){
					buffer[bytes_read] = '\0';
					data.append(buffer);
					bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);
				}
				std::cout << "Received from client " << events[i].ident << ": " << std::endl; //Create a structure for clients to identify them by a number, and not its fd.
				std::cout << data << "$" << std::endl;
				if (epmap[events[i].ident] != ""){
					data = epmap[events[i].ident] + data;
				}
				std::cout << data << "$" << std::endl;
				if (_ws->Send(events[i].ident, data, _env) == 2)
					epmap[events[i].ident] = data;
				else
					epmap[events[i].ident] = "";
			}
			break ;
			//close(events[i].ident);//pas sur
		}
	}
}
*/
void	Server::Stop( void ) {
	close(serverfd);
	std::cout << "\033[1;32mServer stopped on port " << _port << "...\033[0m" << std::endl;
}
//	Exceptions
const char *Webserv::ErrorReadingHtmlPath::what()		const throw()	{	return "Error: could not access to the html directory.";	}

const char *Webserv::ErrorCreatingSocket::what()		const throw()	{	return "Error: could not create the socket.";	}

const char *Webserv::ErrorBindingSocket::what()		const throw()	{	return "Error: could not bind the socket.";		}

const char *Webserv::ErrorListeningSocket::what()	const throw()	{	return "Error: could not listen the socket.";	}

const char *Webserv::ErrorAcceptingSocket::what()	const throw()	{	return "Error: could not accept the new connection.";	}

const char *Webserv::ErrorCreatingKqueue::what()	const throw()	{	return "Error: could not create kqueue instance.";		}

const char *Webserv::ErrorInitializeKqueue::what()	const throw()	{	return "Error: could not initialize kqueue instance.";		}

const char *Webserv::ErrorGettingEvent::what()		const throw()	{	return "Error: could not get the new event.";	}

const char *Webserv::ErrorReadingSocket::what()		const throw()	{	return "Error: could not read in the socket.";	}

const char *Webserv::ErrorSendingData::what()		const throw()	{	return "Error: could not send data to the client.";	}
