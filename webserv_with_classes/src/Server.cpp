#include "Server.hpp"

//	Constructor
Server::Server ( Sender & s, char **env ) : env(env), sender(s), serv(NULL), serv_n(0){
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
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    // Macro to set the kqueue evenet as read and write
	EV_SET(&change_event, serverfd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        close(serverfd);
		throw Server::ErrorInitializeKqueue();
    }

	std::cout << "\033[1;32mServer started on port " << 80 << "...\033[0m" << std::endl;
}

void	Server::Wait( void ) {
	// waiting for event
	nev = kevent(kq, NULL, 0, events, MAX_EVENTS, &timeout);
	std::cout <<"\033[1;33mEvent found : " << nev << "\033[0m" << std::endl;
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
				std::string data = "";
				while (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					data = data + std::string(buffer);
					bytes_read = read(events[i].ident, buffer, sizeof(buffer) - 1);
				}
				std::cout << "Received from client " << events[i].ident << ": " << std::endl;
				std::cout <<  data << "$" << std::endl;
				sender.Send(events[i].ident, data, env);
			}
			//close(events[i].ident);//pas sur
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

Servers::Servers(): location(NULL), location_blocks(0)
{

}

Servers::~Servers()
{

}

Location::Location()
{

}

Location::~Location()
{

}

//blocks: server
//directives: listen, server_name, listen + default_server,  error_page, client_max_body_size, location, limit_except, return/rewrite, root, autoindex, index, fastcgi_pass, include, GET, POST, fastcgi_param, proxy_pass
void Server::make_list(std::string line)
{
	std::istringstream stream(line);
	std::string word;

	while (stream >> word)
	{
		if (!word.compare("include"))
		{
			stream >> word;
			this->parse(word);
			stream >> word;
			if (word.compare(";"))
				throw InvalidConfigurationFile();
		}
		else
			_v.push_back(word);
	}
}

void Server::parse(std::string path)
{
	std::ifstream ifs;
	std::string line;
	std::string total;
	//bool	quotation;
	//bool doble_quotation;

	//En caso de que haga falta hacer wildcards, cambiarlo aquí: un while?
	//quotation = 0;
	//doble_quotation = 0;
	ifs.open(path.c_str());
	if (!ifs.is_open())
	{
		throw Server::OpenFileException();
	}
	while (std::getline(ifs, line))
	{
		size_t pos;
		if (line.find('#') != std::string::npos)
		{
			line.erase(line.begin() + line.find('#'), line.end()); //Erase commented parts of configuration file.
			if (line.size() == 0)
				continue ;
		}
		//size_t pos = line.find('\'');
		/*while (pos != std::string::npos)
		{
			if (quotation == 0)
				quotation = 1;
			else
				quotation = 0;
			pos = line.find('\'', pos + 1);
		}
		pos = line.find('\"');
		while (pos != std::string::npos)
		{
			if (quotation == 0)
				quotation = 1;
			else
				quotation = 0;
			pos = line.find('\"', pos + 1);
		}*/
		//Ir añadiendo todo a una nueva string sin comentarios.
		//if (line[line.size() - 1] == ';') //Acaba la directriz? -> Puede haber espacios al final.
		pos = line.find('{');
		while (pos != std::string::npos)
		{
			line.insert(line.find('{', pos), 1, ' ');
			line.insert(line.find('{', pos) + 1, 1, ' ');
			pos = line.find('{', pos + 2);
		}
		pos = line.find('}');
		while (pos != std::string::npos)
		{
			line.insert(line.find('}', pos), 1, ' ');
			line.insert(line.find('}', pos) + 1, 1, ' ');
			pos = line.find('}', pos + 2);
		}
		pos = line.find(';');
		while (pos != std::string::npos)
		{
			line.insert(line.find(';', pos), 1, ' ');
			pos = line.find(';', pos + 2);
		}
		total += line + '\n';
	}
	if (total.size() == 0) //If file is empty.
		throw InvalidConfigurationFile();
	ifs.close();
	make_list(total);
} //Esta función es recursiva así que no puedo añadir nada más después de make_list.



void Server::last_function(int &bracket, std::vector<std::string>::iterator &it, Location &location)
{
	std::vector<std::string>::const_iterator start;
	std::vector<std::vector<std::string>::iterator> its;
	int n;
	int counter;

	//std::cout << "seg" << std::endl;
	start = it;
	counter = 0;
	n = bracket;
	bracket++;
	it++;
	it++;
	if (*(it) != "{")
		throw InvalidConfigurationFile();
	it++;
	while (bracket != n)
	{
		if (!(*it).compare("{"))
		{
			if ((*(it - 2)).compare("location"))
				throw InvalidConfigurationFile();
			bracket++;
			if (bracket == n + 2)
			{
				std::vector<std::string>::iterator copy_it;
				copy_it = it;
				copy_it -= 2;
				//std::cout << "\033[31m" << *it << "\033[0m" << std::endl;
				//std::cout << "\033[31m" << *copy_it << "\033[0m" << std::endl;
				its.push_back(copy_it); //iterator vector for each block
			}
		}
		else if (!(*it).compare("}"))
		{
			bracket--;
			if (bracket == n + 1)
				counter++;
		}
		it++;
	}
	//std::cout << "Server counter: " << counter << std::endl;
	location.sub_location_blocks = counter;
	//std::cout << "\033[31mseg fault: " << location.sub_location_blocks << "\033[0m" << std::endl;
	if (counter != 0)
		location.sub_block = new Location[counter];
	n++;
	for (int i = 0; i < counter; i++)
	{
		//std::cout << "Again last function" << std::endl;
		last_function(n, its[i], location.sub_block[i]);
	}
	//std::cout << "\033[32m exit iterator: " << *(it) << "\033[0m" << std::endl;
}

void Server::sub_location_blocks()
{
	int bracket;
	int serv_it;
	int loc_it;

	bracket = 0;
	serv_it = 0;
	loc_it = 0;
	for (std::vector<std::string>::iterator it = _v.begin(); it < _v.end(); it++)
	{
		if (*it == "{")
			bracket++;
		else if (*it == "}")
		{
			bracket--;
			if (bracket == 0)
			{
				serv_it++;
				loc_it = 0;
			}
			if (bracket == 1)
				loc_it++;
		}
		else if (bracket == 1 && !((*it).compare("location")) && (it + 2) != _v.end() && *(it + 2) == "{")
		{
			//std::cout << "server: " << serv_it << ", location block:" << loc_it << std::endl;
			last_function(bracket, it, serv[serv_it].location[loc_it]);
			//std::cout << "\033[34m exit iterator: " << *(it) << ", bracket value: " << bracket << "\033[0m" << std::endl;
			it--; //porque volverá a ser incrementado al final del bucle.
			//std::cout << "\033[34m exit iterator: " << *(it) << ", bracket value: " << bracket << "\033[0m" << std::endl;
			if (bracket == 0)
			{
				serv_it++;
				loc_it = 0;
			}
			else if (bracket == 1)
				loc_it++;
		}
		else
			continue ;
	}
}

void Server::count_location_blocks(void) //inicialmente n = 1, con recursividad va aumentando.
{
	int bracket;
	int serv_it;

	bracket = 0;
	serv_it = 0;
	for (std::vector<std::string>::iterator it = _v.begin(); it < _v.end(); it++)
	{
		if (*it == "{" && bracket != 1)
			bracket++;
		else if (*it == "}")
		{
			bracket--;
			if (bracket == 0)
			{
				this->serv[serv_it].location = new Location[serv[serv_it].location_blocks];
				serv_it++;
			}
		}
		else if (bracket == 1 && !((*it).compare("location")) && (it + 2) != _v.end() && *(it + 2) == "{")
		{
			serv[serv_it].location_blocks++;
			it++;
			it++;
			if (*it != "{")
				throw InvalidConfigurationFile();
			else
				bracket++;
		}
		else if (bracket == 1 && (*it == "{" || *it == "}"))
			throw InvalidConfigurationFile();
		else
			continue ;
	}
}

void Server::count_servers(void)
{
	//Check tipo directives. -> lista con las directivas válidas.
	int bracket;
	int server_blocks;

	bracket = 0;
	server_blocks = 0;
	//recursive
	for (std::vector<std::string>::iterator it = _v.begin(); it < _v.end(); it++)
	{
		if (*it == "{")
		{	if (bracket == 0 && (it == _v.begin() || (*(it - 1)).compare("server")))
				throw InvalidConfigurationFile();
			bracket++;
		}
		else if (*it == "}")
			bracket--;
		else if (bracket == 0)
		{
			//std::cout << *it << std::endl;
			if (!((*it).compare("server")))
				server_blocks++;
			else
				throw InvalidConfigurationFile();
		}
		else
			continue ;
	}
	if (bracket != 0)
		throw InvalidConfigurationFile();

	//std::cout << "number of server blocks: " << server_blocks << std::endl;
	serv_n = server_blocks;
	this->serv = new Servers[server_blocks];
}

void Server::location_parse(int &bracket, std::vector<std::string>::iterator &it, Location &location, int n)
{
	it++;
	location.prefix = *it;
	int count;

	count = 0;
	it++;
	if ((*it).compare("{"))
	{
		std::cout << "mal" << std::endl;
		throw InvalidConfigurationFile();
	}
	bracket++;
	it++;
	while (bracket == n + 1) // == n + 1?
	{
		if (!(*it).compare("}"))
			bracket--;
		else
		{
			if (!(*it).compare("location"))
			{
				//std::cout << "count: " << count << std::endl;
				location_parse(bracket, it, location.sub_block[count], n + 1);
				it--;
				count++;
			}
			else
			{
				std::string key;	//Check if key is valid.

				key = *it;
				//std::cout << *it << std::endl;
				it++;
				//std::cout << *it << std::endl;
				while ((*it).compare(";")) //not equal to ';'
				{
					if (*it == "{" || *it == "}")
					{
						std::cout << "mal2" << std::endl;
						throw InvalidConfigurationFile();
					}
					location.data[key].push_back(*it);
					//this->serv[serv_it].d[key].push_back(*it);
					//Guardar en class location.
					it++;
				}
				//std::cout << "salida: " << *it << std::endl;
			}
		}
		it++;
	}
}

void Server::data_structure(void)
{
	int bracket;
	int serv_it;
	int loc_it;

	count_servers();
	count_location_blocks();
	sub_location_blocks();
	//for (int i = 0; i < serv_n; i++) //Para cada server block.
	//	for(int j = 0; j < serv[i].location_blocks; j++) //Para cada location block.
	//		sub_location_blocks(); //A partir de aquí ya se puede hacer con recursividad ya hay al menos algo de memoria alocada para cada server block.
	//for (int i = 0; i < serv_n; i++)
	//{
	//	std::cout << serv[i].location_blocks << std::endl;
	//}
	bracket = 0;
	serv_it = 0;
	loc_it = 0;
	for (std::vector<std::string>::iterator it = _v.begin(); it < _v.end(); it++)
	{
		if (*it == "{")
			bracket++;
		else if (*it == "}")
		{
			bracket--;
			if (bracket == 0) //When bracket is 0 after '}', a new server block begins. -> We've checked previously that this is the case (there's just the word server when bracket == 0).
			{
				serv_it++;
				loc_it = 0;
			}
			else if (bracket == 1)
				loc_it++;
		}
		else if (!(*it).compare("location") && bracket == 1 && (it + 2) != _v.end() && *(it + 2) == "{")
		{
			//std::cout << "\033[32m serv_it: " << serv_it << ", loc_it: " << loc_it << "\033[0m" << std::endl;
			location_parse(bracket, it, serv[serv_it].location[loc_it], 1);
			if (bracket == 0)
			{
				serv_it++;
				loc_it = 0;
			}
			else if (bracket == 1)
				loc_it++;
			it--;
		}
		else if (bracket == 1) //bracket == 1 and it's not location.
		{
			std::string key;
			key = *it;
			//Check if key is valid.
			it++;
			while ((*it).compare(";"))
			{
				if (*it == "{" || *it == "}")
				{
					std::cout << "mal3" << std::endl;
					throw InvalidConfigurationFile();
				}
				this->serv[serv_it].d[key].push_back(*it);
				it++;
			}
		}

	}
}

void Server::check(void)
{
	//std::cout << serv[0].d["server_name"][0] << std::endl;
	std::map<std::string, std::vector<std::string> >::iterator it;
	for (it = serv[0].d.begin(); it != serv[0].d.end(); ++it) {
		std::cout << "Key: \033[31m" << it->first << ",\033[0m Value: \033[34m" << it->second[0] << "\033[0m" << std::endl;
	}
	std::cout << "\033[34m location block:\033[0m" << std::endl;
	for (int i = 0; i < serv_n; i++)
	{
		for (int j = 0; j < serv[i].location_blocks; j++)
		{
			std::cout << "\033[32mserv_n: " << i << ", loc_n: " << j << "\033[0m" << std::endl;
			std::cout << "\033[30mprefix:" << serv[i].location[j].prefix << "\033[0m" << std::endl;
			for (it = serv[i].location[j].data.begin(); it != serv[i].location[j].data.end(); ++it)
			{
				std::cout << "Key: \033[31m" << it->first << ",\033[0m Value: \033[34m" << it->second[0] << "\033[0m" << std::endl;
			}
		}
	}
}

void Server::recursive_clear(Location &location)
{
	for (int i = 0; i < location.sub_location_blocks; i++)
		recursive_clear(location.sub_block[i]);
	location.data.clear();
	if (location.sub_location_blocks > 0)
		delete [] location.sub_block;
}

void Server::clean(void)
{
	for (int i = 0; i < serv_n; i++)
	{
		for (int j = 0; j < serv[i].location_blocks; j++)
		{
			recursive_clear(serv[i].location[j]);
		}
		if (serv[i].location_blocks > 0)
			delete [] serv[i].location;
	}
	_v.clear();
	delete [] serv;
}

const char *Server::OpenFileException::what() const throw()
{
	return ("Error: Could not open file.");
}

const char *Server::InvalidConfigurationFile::what() const throw()
{
	return ("Error: Invalid configuration file.");
}
