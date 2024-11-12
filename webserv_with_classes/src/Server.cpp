#include "Server.hpp"

//	Constructor
Server::Server ( void ) {
	std::cout << "Default Server constructor called" << std::endl;
}
//	Destructor
Server::~Server ( void ) {
	std::cout << "Destructor Server called" << std::endl;
}

//	Methods
void	Server::Initialize( std::string &path_to_html, std::string &path_to_err ) {
	std::string last_path = "/";
	try {
		ReadPath(path_to_html, last_path);
    	ReadPath(path_to_err, last_path);
	} catch (std::exception &e) {
		throw;
	}
}

void	Server::ReadPath( std::string path, std::string last_path) {
	if (chdir(path.c_str()) == -1) throw Server::ErrorReadingHtmlPath();

	if (path.at(path.length() - 1) != '/') path += '/';
	if (path.at(0) == '/') path = path.substr(1);
	DIR *dir = opendir(".");
	struct dirent   *dirent;
	while ((dirent = readdir(dir)) != NULL) {
		std::string ndir(dirent->d_name);
		if (ndir == "." || ndir == "..") continue;
		if (dirent->d_type == DT_DIR){
			try { ReadPath(ndir, last_path + path); } catch (std::exception &e) { throw; }
			continue; }
		try { ReadFile(ndir, last_path); } catch (std::exception &e) { throw; }
	}
	
	if (chdir("..")) throw Server::ErrorReadingHtmlPath();
	closedir(dir);
}

void	Server::ReadFile( std::string file, std::string last_path) {
	std::ifstream ifs(file);
	if (!ifs.is_open()) throw Server::ErrorReadingHtmlPath();
	std::stringstream buffer;
    buffer << ifs.rdbuf();
	ifs.close();
	if (file.find(".") != std::string::npos) {
		_html_map[last_path + file.substr(0, file.find("."))] = buffer.str();
	}
	else
		_html_map[last_path + file] = buffer.str();
	if (file == "index.html") _html_map[last_path] = buffer.str();
	std::cout << last_path + file.substr(0, file.find(".")) << std::endl;
}

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
		close(serverfd);
		throw Server::ErrorGettingEvent();
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
				throw Server::ErrorAcceptingSocket();
			}
			fcntl(clientfd, F_SETFL, O_NONBLOCK);// Set the client socket as non-blocking
			std::cout << "\033[1;32mNew client connected: " << clientfd << "\033[0m" << std::endl;
		//	Add the new client socket to kqueue for monitoring
			EV_SET(&change_event, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
			if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
				close(clientfd);
				throw Server::ErrorInitializeKqueue();
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
			else if (bytes_read < 0) throw Server::ErrorReadingSocket();
			else {
				buffer[bytes_read] = '\0';
				std::cout << "Received from client " << events[i].ident << ": " << std::endl;
				std::cout <<  buffer << "$" << std::endl;
			//	Send the data back to the client
				Server::Send(events[i].ident, buffer);
			}
		}
	}
}

void	Server::Send(int clientfd, char *buffer) {
	Request request = Request();
	try {
		request.Parse(buffer);
	} catch (std::exception &e) {
		throw;
	}
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";
	std::string body = "";
	if (request.GetMethod() == "GET") {
		if (_html_map[request.GetUri()] != "")
			body = _html_map[request.GetUri()];
		else {
			body = _html_map["errwww/404.html"];	
		}
		response += std::to_string(body.size()) + "\n\n" + body;
		if (send(clientfd, response.c_str(), response.size(), 0) == -1)
			throw Server::ErrorSendingData();
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