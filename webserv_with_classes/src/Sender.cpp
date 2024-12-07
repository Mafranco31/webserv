#include "Sender.hpp"

Sender::Sender( void ) {
	std::cout << "Default Sender constructor called" << std::endl;
}

Sender::~Sender ( void ) {
	std::cout << "Destructor Sender called" << std::endl;
}

Sender::Sender ( const std::string &path_to_html, const std::string &path_to_err ) {
	std::cout << "Sender constructor called" << std::endl;
	http_version = "HTTP/1.1";
	std::string last_path = "/";
	try {
		ReadPath(path_to_html, last_path);
    	ReadPath(path_to_err, last_path);
	} catch (std::exception &e) {
		throw;
	}
}

void	Sender::ReadPath( std::string path, std::string last_path) {
	if (chdir(path.c_str()) == -1) throw Webserv::ErrorReadingHtmlPath();

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
		try { ReadFile(ndir, last_path + path); } catch (std::exception &e) { throw; }
	}

	if (chdir("..")) throw Webserv::ErrorReadingHtmlPath();
	closedir(dir);
}

void	Sender::ReadFile( std::string file, std::string last_path) {
	std::ifstream ifs(file.c_str());
	if (!ifs.is_open()) throw Webserv::ErrorReadingHtmlPath();
	std::stringstream buffer;
    buffer << ifs.rdbuf();
	ifs.close();
	if (file.find(".") != std::string::npos) {
		_html_map[last_path + file.substr(0, file.find("."))] = buffer.str();
	}
	else
		_html_map[last_path + file] = buffer.str();
	//if (file == "index.html")
	//{
	//	std::cout << "Index last path: " << last_path << std::endl;
	//	_html_map[last_path] = buffer.str();
	//}
	std::cout << last_path + file.substr(0, file.find(".")) << std::endl;
}

void Sender::choose_server_block(Request &request)
{
	//Primero: exact match
	std::string host = request.GetHeaders()["HOST"];
	//std::cout << host << std::endl;
	if (host.find(':') == std::string::npos)
	{
		request._host = host;
		request._host.erase(std::remove(request._host.begin(), request._host.end(), ' '), request._host.end());
		//request._host.erase(std::remove(request._host.begin(), request._host.end(), ' '), request._host.end());
		request._port = "80";
	}
	else
	{
		request._host = std::string(host.begin() + 1, host.begin() + host.find(':'));
		//request._host.erase(std::remove(request._host.begin(), request._host.end(), ' '), request._host.end());
		request._port =  std::string(host.begin() + host.find(':') + 1 , host.end());

	}
	//request._host = request._host.substr(request._host.find_first_not_of('\r'));
	request._host.erase(request._host.find_last_not_of('\r') + 1);
	//request._port = request._port.substr(request._port.find_first_not_of('\r'));
	request._port.erase(request._port.find_last_not_of('\r') + 1);
	std::cout << "$" << request._host << "$" << std::endl;
	//request._port = request._port.substr(request._port.find_first_not_of('\0'));
	//request._port.erase(request._port.find_last_not_of('\0') + 1);
	std::cout << "$" << request._port << "$" << std::endl;
	for (int i = 0; i < _ws->serv_n; i++ )
	{
		if (_ws->serv[i].port == request._port && _ws->serv[i].d.find("server_name") != _ws->serv[i].d.end())
		{
			for (std::vector<std::string>::iterator it = _ws->serv[i].d["server_name"].begin(); it != _ws->serv[i].d["server_name"].end(); it++)
			{
				if (request._host == *it)
				{
					request.serv_block = _ws->serv[i];
					//std::cout << "holaaaaaaaaaaa: "<< request.serv_block.d["root"][0] << std::endl;
					return ;
				}
			}
		}
	}
	for (int i = 0; i < _ws->serv_n; i++ )
	{
		if ((request._host == "127.0.0.1" || request._host == "localhost") && _ws->serv[i].port == request._port)
		{
			request.serv_block = _ws->serv[i];
			//std::cout << "holaaaaaaaaaaa: "<< request.serv_block.d["root"][0] << std::endl;
			return ;
		}
	}
}

void Sender::choose_location_block(Request &request)
{
	for (int i = 0; i < request.serv_block.location_blocks; i++)
	{
		//if (request.serv_block.location[i].prefix == request.GetUri)
	}
}

void Sender::server_configuration(Request &request)
{
	if (request.serv_block.d.find("root") != request.serv_block.d.end())
		request.root = request.serv_block.d["root"][0];
	if (request.serv_block.d.find("index") != request.serv_block.d.end())
	{
		for (std::vector<std::string>::iterator it = request.serv_block.d["index"].begin(); it != request.serv_block.d["index"].end(); it++)
		{
			request.index = request.root + "/" + (*it).substr(0, (*it).find("."));
			std::cout << "Index again: " << request.index << std::endl;
			if (_html_map.find(request.index) != _html_map.end())
			{
				std::cout << "inside: " << request.index << std::endl;
				_html_map[(request.root + "/")] = _html_map[request.index];
				break ;
			}
		}
		//std::cout << "Index again: " << request.index << std::endl;
	}
	std::cout << "root" << request.root << std::endl;
	request.uri = request.root + request.uri;
}

void	Sender::Send(int clientfd, std::string buffer, char **env) {
	Request request = Request();
	std::string response = "";
	std::string body = "";
	try {
		request.Parse(buffer);
		std::cout << "\033[34m" << request << "\033[0m" << std::endl;
		choose_server_block(request);
		server_configuration(request);
		//choose_location_block(request);
		std::cout << "uri" << request.uri << std::endl;
		//choose server block
		//set general variables/default variables.
		//choose location block

		if (request.GetMethod() == "GET") {
			if (_html_map[request.GetUri()] != "") {
				if (_html_map[request.GetUri()].find('\n') == std::string::npos)
					body = ex_cgi(_html_map[request.GetUri()], clientfd, env, request.GetMethod());
				else
					body = _html_map[request.GetUri()];
				response = http_version  + " 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(body.size()) + "\n\n" + body;
			}
			else {
				throw ErrorHttp("404 Not Found", request.e_404);
			}
		}
		else if (request.GetMethod() == "POST") {
			if (_html_map[request.GetUri()] != "") {
				body = _html_map[request.GetUri()];
				std::ostringstream oss; //Lo pongo así porque me dice que el to_string() no es de c++ 98 -> oss.str()
				oss << body.size();
				response = Post(clientfd, request);
				response = response + oss.str() + "\n\n" + body;
			}
			else {
				throw ErrorHttp("404 Not Found", "/404");
			}
		}
		else if (request.GetMethod() == "DELETE") {
			response = Delete(request);
		}
		// else if (request.GetMethod() == "PUT") {	Put(clientfd, request);	}
		// else if (request.GetMethod() == "HEAD") {	Head(clientfd, request);	}
		// else if (request.GetMethod() == "OPTIONS") {	Options(clientfd, request);	}
		// else if (request.GetMethod() == "TRACE") {	Trace(clientfd, request);	}
		// else if (request.GetMethod() == "CONNECT") {	Connect(clientfd, request);	}
		// else if (request.GetMethod() == "PATCH") {	Patch(clientfd, request);	}
		else {
			throw ErrorHttp("501 Not Implemented", "/501");
		}
	} catch (ErrorHttp &e) {
		body = _html_map[e.get_errcode()];
		std::ostringstream oss;
		oss << body.size();
		response = http_version + " " + e.what() + "\nContent-Type: text/html\nContent-Length: " + oss.str() + "\n\n" + body;
	}
	if (send(clientfd, response.c_str(), response.size(), 0) == -1)
		throw Webserv::ErrorSendingData();
}

std::string	Sender::Post(int clientfd, Request &request) {
	std::string response = "";
	std::string data = "";
	(void)clientfd;

	std::cout << "Longueur du body : " <<  request.GetBodyLength() << std::endl;
	if (request.GetBodyLength() < 1) {
		char buffer[BUFFER_SIZE];

		ssize_t bytes_read = read(clientfd, buffer, sizeof(buffer) - 1);
		if (bytes_read < 0) {
			std::cout << "error ici " << std::endl;
			throw ErrorHttp("500 Internal Server Error", "/500");
		}
		else {
			while (bytes_read > 0) {
				buffer[bytes_read] = '\0';
				data = data + std::string(buffer);
				bytes_read = read(clientfd, buffer, sizeof(buffer) - 1);
				if (bytes_read < 0)
					throw ErrorHttp("500 Internal Server Error", "/500");
			}
		}
	}
	else {
		data = request.GetBody();
	}

	size_t name_start = data.find("filename=") + 10;
	std::string name = data.substr(name_start, data.find("\"", name_start) - name_start);

	size_t content_type_start = data.find("\n\r\n") + 3;
	size_t content_type_end = data.find("------WebKitFormBoundary", content_type_start);
	std::string content = data.substr(content_type_start, content_type_end - content_type_start - 2);

	if (request.GetHeaders()["CONTENT-TYPE"] != "") {
		/*
			Manage accepted content types
		*/
		std::cout << "File received : " << std::endl;
		std::string file_path = "." + request.GetUri() + "/" + name;
		std::cout <<  content << "$" << std::endl << "at" << file_path << "$" << std::endl;
		std::ofstream ofs(file_path);
		if (ofs.is_open()) {
			ofs << content;
			ofs.close();
		} else {
			std::cout << "error ici 2" << std::endl;
			throw ErrorHttp("500 Internal Server Error", "/500");
		}
		response = http_version + " 201 Created\nContent-Type: text/html\nContent-Length: ";
	}
	else {
		throw ErrorHttp("415 Unsupported Media Type", "/415");
	}

	return response;
}

std::string Sender::Delete(Request &request) {
	std::string response = "";
	std::string body = "";
	std::string name = "./uploads" + request.GetFullUri();

	std::cout << "Delete de : " << name << "$" << std::endl;
	if (std::remove(name.c_str())) {
		throw ErrorHttp("500 Internal Server Error", "/500");
	}

	body = _html_map["delete"];
	response = http_version + " 200 OK\nContent-Type: text/html\nContent-Length: " + std::to_string(body.size()) + "\n\n" + body;
	return response;
}
