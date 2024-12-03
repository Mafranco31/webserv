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
		try { ReadFile(ndir, last_path); } catch (std::exception &e) { throw; }
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
	if (file == "index.html") _html_map[last_path] = buffer.str();
	std::cout << last_path + file.substr(0, file.find(".")) << std::endl;
}

void	Sender::Send(int clientfd, char *buffer) {
	Request request = Request();
	std::string response = "";
	std::string body = "";
	try {
		request.Parse(buffer);
		if (request.GetMethod() == "GET") {
			if (_html_map[request.GetUri()] != "") {
				body = _html_map[request.GetUri()];
				std::ostringstream oss;
				oss << body.size();
				response = http_version  + " 200\nContent-Type: text/html\nContent-Length: " + oss.str() + "\n\n" + body;
			}
			else {
				throw ErrorHttp("404 Not Found", "/404");
			}
		}
		//else if (request.GetMethod() == "POST") {	Post(clientfd, request);	}
		//else if (request.GetMethod() == "DELETE") {	Delete(clientfd, request);	}
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
