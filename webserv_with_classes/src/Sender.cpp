#include "Server.hpp"

void Webserv::initialize_path ( const std::string &path_to_html, const std::string &path_to_err ) {
	//std::cout << "Sender constructor called" << std::endl;
	(void)path_to_err; //errwww goes inside www.
	http_version = "HTTP/1.1";
	std::string last_path = "/";
	try {
		ReadPath(path_to_html, last_path);
		//ReadPath(path_to_err, last_path);
	} catch (std::exception &e) {
		throw;
	}
}

void	Webserv::ReadPath( std::string path, std::string last_path) {
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

void	Webserv::ReadFile( std::string file, std::string last_path) {
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
	//std::cout << last_path + file.substr(0, file.find(".")) << std::endl;
}

std::string	Webserv::CreateAutoIndex(Request &request)
{
	std::string dirName = "www" + request.root + request.uri;
	DIR *dir = opendir(dirName.c_str());
	std::string page =\
	"<!DOCTYPE html>\n\
	<html>\n\
	<head>\n\
			<title>" + dirName + "</title>\n\
	</head>\n\
	<body>\n\
	<h1>INDEX</h1>\n\
	<p>\n";

	if (dir == NULL) {
		std::cerr << "\033[31m" << "Error: could not open " << dirName << "\033[0m" << std::endl;
		return "";
	}
	if (dirName[0] != '/')
		dirName = "/" + dirName;
	for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir))
	{
		std::string ndir(dirEntry->d_name);
		if (ndir == "." || ndir == "..") continue;
		if (dirEntry->d_type == DT_DIR)
			ndir = ndir + "/";
		page += createLink(ndir, request);
	}
	page +="\
	</p>\n\
	</body>\n\
	</html>\n";
	closedir(dir);
	return page;
}

std::string	Webserv::createLink(std::string const &dirEntry, Request &request)
{
	std::stringstream   ss;
	ss << "\t\t<p><a href=\"http://" + request._host + ":" <<\
		request._port << request.uri + dirEntry + "\">" + dirEntry + "</a></p>\n";
	return ss.str();
}

void Webserv::choose_server_block(Request &request)
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
	request._port.erase(request._port.find_last_not_of('\r') + 1);
	//std::cout << "$" << request._host << "$" << std::endl;
	//std::cout << "$" << request._port << "$" << std::endl;
	for (int i = 0; i < serv_n; i++ )
	{
		if (serv[i].port == request._port && serv[i].d.find("server_name") != serv[i].d.end())
		{
			for (std::vector<std::string>::iterator it = serv[i].d["server_name"].begin(); it != serv[i].d["server_name"].end(); it++)
			{
				if (request._host == *it)
				{
					request.serv_block = &serv[i];
					//std::cout << "holaaaaaaaaaaa: "<< request.serv_block->d["root"][0] << std::endl;
					return ;
				}
			}
		}
	}
	for (int i = 0; i < serv_n; i++ )
	{
		if ((request._host == "127.0.0.1" || request._host == "localhost") && serv[i].port == request._port)
		{
			request.serv_block = &serv[i];
			//std::cout << "holaaaaaaaaaaa: "<< request.serv_block->d["root"][0] << std::endl;
			return ;
		}
	}
}



void Webserv::recursive_location(Location &loc, Request &request)
{
	//std::cout << "prefix: " << loc.prefix << std::endl;
	//std::cout << "URI: " << request.uri << std::endl;

	//std::string uri_prefix = request.uri.substr(0, loc.prefix.length());
	//std::cout << "size uri_prefix: " << uri_prefix.length() << std::endl;
	//std::cout << "size tmp_prefix: " << tmp_prefix.length() << std::endl;
	std::cout << "A veeer: " << request.uri << std::endl;
	if (request.uri.find(loc.prefix) == 0 && loc.prefix.length() > tmp_prefix.length())
	{
		//std::cout << "size uri_prefix: " << uri_prefix.length() << std::endl;
		//std::cout << "size tmp_prefix: " << tmp_prefix.length() << std::endl;
		std::cout << "prefix: " << loc.prefix << std::endl;
		std::cout << "URI: " << request.uri << std::endl;
		std::string s1 = "hola";
		std::cout << "Por probar: " << s1.find(s1) << std::endl;
		tmp_prefix = loc.prefix;
		request.location_block = &loc;
		//std::cout << "request.location_block: " << request.location_block->prefix << std::endl;
		if (loc.eq == 1 || loc.sub_location_blocks == 0)
			return ;
		else
			for (int i = 0; i < loc.sub_location_blocks; i++)
				recursive_location(loc.sub_block[i], request);
	}
}

void Webserv::choose_location_block(Request &request)
{
	tmp_prefix = "";
	for (int i = 0; i < request.serv_block->location_blocks; i++)
	{
		recursive_location(request.serv_block->location[i], request);
	}
}

void Webserv::server_configuration(Request &request)
{
	_html_map.erase("/"); //We make sure that the "index"
	request.redir = "";
	std::map<std::string, std::vector<std::string> > map = request.serv_block->d;

	if (map.find("root") != map.end())
	{
		if (map["root"].size() != 1 )
			throw Webserv::InvalidConfigurationFile();
		request.root = map["root"][0];
	}
	if (map.find("return") != map.end())
	{
		if (map["return"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		std::cout << map["return"][0] << std::endl;
		request.redir = map["return"][0];
	}
	//request.uri = "/www" + request.root + request.uri;
	std::cout << request.uri << std::endl;
	if (map.find("index") != map.end())
	{
		if (map["index"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		for (std::vector<std::string>::iterator it = map["index"].begin(); it != map["index"].end(); it++)
		{
			request.index = "/www" + request.root + "/" + (*it).substr(0, (*it).find("."));
			//std::cout << "ola? Index: " << request.index << std::endl;
			//std::cout << "Index again: " << request.index << std::endl;
			if (_html_map.find(request.index) != _html_map.end())
			{
				_html_map[("/")] = _html_map[request.index]; //Si el request a "/" redirige al contenido de index.
				if (request.uri[request.uri.size() - 1] == '/')
					request.uri = "/";
				break ;
			}
		}
		//std::cout << "Index again: " << request.index << std::endl;
	}
	std::vector<std::vector<std::string> > err = request.serv_block->err_page;
	if (err.size() > 0)
	{
		//std::cout << "Entered error" << std::endl;
		for (std::vector<std::vector< std::string> >::iterator it1 = err.begin(); it1 != err.end(); it1++)
		{
			if ((*it1).size() < 2)
				continue ;
			std::string tmp = *((*it1).begin() + (*it1).size() - 1);
			for (std::vector<std::string>::iterator it2 = (*it1).begin(); it2 != ((*it1).end() -1); it2++)
			{
				request.error[*it2] = "/www" + request.root + tmp.substr(0, tmp.find("."));
				//std::cout << request.error[*it2] << std::endl;
				//std::cout << "path: " << request.error[*it2] << " error: " << *it2 << std::endl;
			}
		}
	}
	if (map.find("client_max_body_size") != map.end())
	{
		if (map["client_max_body_size"].size() != 1)
			throw Webserv::InvalidConfigurationFile();
		std::stringstream ss(map["client_max_body_size"][0]);
		int n;
		ss >> n;
		if (ss.fail())
			throw Webserv::InvalidConfigurationFile();
		request.limit_size =  n;
		//std::cout << n << std::endl;
	}
}

void Webserv::location_configuration(Request &request)
{
	std::cout << "holaaa" << request.location_block->prefix << std::endl;
	std::map<std::string, std::vector<std::string> > map = request.location_block->data;
	std::cout << "check" << std::endl;
	if (map.find("root") != map.end())
	{
		if (map["root"].size() != 1)
			throw Webserv::InvalidConfigurationFile();
		request.root = map["root"][0];
	}
	std::cout << "root: " << request.root << std::endl;
	if (map.find("index") != map.end())
	{
		if (map["index"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		for (std::vector<std::string>::iterator it = map["index"].begin(); it != map["index"].end(); it++)
		{
			request.index = "/www" + request.root + "/" + (*it).substr(0, (*it).find("."));
			//std::cout << "Index again: " << request.index << std::endl;
			if (_html_map.find(request.index) != _html_map.end())
			{
				//std::cout << "inside: " << request.index << std::endl;
				_html_map["/"] = _html_map[request.index];
				if (request.uri[request.uri.size() - 1] == '/')
					request.uri = "/";
				break ;
			}
		}
		//std::cout << "Index again: " << request.index << std::endl;
	}
	std::vector<std::vector<std::string> > err = request.location_block->err_page;
	if (err.size() > 0)
	{
		for (std::vector<std::vector< std::string> >::iterator it1 = err.begin(); it1 != err.end(); it1++)
		{
			if ((*it1).size() < 2)
				continue ;
			std::string tmp = *((*it1).begin() + (*it1).size() - 1);
			for (std::vector<std::string>::iterator it2 = (*it1).begin(); it2 != ((*it1).end() -1); it2++)
			{
				request.error[*it2] =  "/www" + request.root + tmp.substr(0, tmp.find("."));
				//std::cout << "path: " << request.error[*it2] << " error: " << *it2 << std::endl;
			}
		}
	}
	if (map.find("client_max_body_size") != map.end())
	{
		if (map["client_max_body_size"].size() != 1)
			throw Webserv::InvalidConfigurationFile();
		std::stringstream ss(map["client_max_body_size"][0]);
		int n;
		ss >> n;
		if (ss.fail())
			throw Webserv::InvalidConfigurationFile();
		request.limit_size =  n;
	}
	if (map.find("return") != map.end())
	{
		std::cout << "there's a redirection." << std::endl;
		if (map["return"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		std::cout << map["return"][0] << std::endl;
		request.redir = map["return"][0];
	}
	if (map.find("limit_except") != map.end())
	{
		std::cout << "\033[32mThe method: \033[0m" << request.GetMethod() << std::endl;
		if (std::find(map["limit_except"].begin(), map["limit_except"].end(), request.GetMethod()) == map["limit_except"].end())
		{
			std::cout << "method not allowed" << std::endl;
			throw ErrorHttp("405 Method Not Allowed", request.error["405"]);
		}
	}
	if (map.find("autoindex") != map.end())
	{
		if (map["autoindex"].size() < 1 && !(map["autoindex"][0] == "off" || map["autoindex"][0] == "on"))
			throw Webserv::InvalidConfigurationFile();
		request.autoindex = map["autoindex"][0];
	}
}

int	Webserv::Send(int clientfd, std::string buffer, char **env) {
	Request request = Request();
	std::string response = "";
	std::string body = "";
	(void)env;

	try {
		//std::cout << "\033[34m";
		
		if (request.Parse(buffer, clientfd) == 2)
			return 2;
		choose_server_block(request);
		//std::cout << "arrives here1" << std::endl;
		server_configuration(request);

		choose_location_block(request);

		// for (std::map<std::string, std::string>::iterator it = _html_map.begin(); it != _html_map.end(); it++) {
		// 	std::cout << it->first << " : "  << std::endl;
		// }

		std::cout << "location block: " << request.serv_block->location_blocks << std::endl;
		//std::cout << "location block: " << request.location_block->prefix << std::endl;
		if (request.serv_block->location_blocks != 0 && request.location_block != NULL)
			location_configuration(request);
		if (request.uri[request.uri.size() - 1] != '/') //If it's /, it stays like that because / is been linked to the index. I
			request.uri = "/www" + request.root + request.GetUri();
		if (request.GetMethod() == "GET") {
			if (request.redir != "")
			{
				response = http_version + " 301 Moved Permanently\nLocation: " + request.redir + "\nContent-Length: 0\n\n";
			}
			else if (request.GetIsCgi()) {

				if (_html_map[request.GetUri()] != "") {
					try {
						response = ft_ex_cgi_get(request);
					} catch (ErrorHttp &e) {
						throw ;
					}
					if (send(clientfd, response.c_str(), response.size(), 0) == -1)
						throw Webserv::ErrorSendingData();
					return 1;
				}
				else
					throw ErrorHttp("404 Not Found", request.error["404"]);
			}
			else if (_html_map[request.GetUri()] != "") {
				body = _html_map[request.GetUri()];
				response = http_version  + " 200 OK\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) + "\n\n" + body;
			}
			else if (request.autoindex == "on")
			{
				body = CreateAutoIndex(request);
				response = http_version  + " 200 OK\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) + "\n\n" + body;
			}
			else
				throw ErrorHttp("404 Not Found", request.error["404"]);

		}
		else if (request.GetMethod() == "POST") {
			if (request.limit_size < (int)request.GetBodyLength())
				throw ErrorHttp("413 Request Entity Too Large", request.error["413"]);
			if (request.GetIsCgi()) {
				if (_html_map[request.GetUri()] != "") {
					try {
						if (!ft_ex_cgi_post(request)) {
							body = _html_map["upload_success.html"];
							response = http_version + " 200 Ok\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) +  "\n\n" + body;
						}
						else {
							body = _html_map["upload_fail.html"];
							response = http_version + " 400 Bad Request\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) +  "\n\n" + body;
						}
					} catch (ErrorHttp &e) {
						throw ;
					}
					// std::cout << response << std::endl;
					if (send(clientfd, response.c_str(), response.size(), 0) == -1)
						throw Webserv::ErrorSendingData();
					return 1;
				}
				else
					throw ErrorHttp("404 Not Found", request.error["404"]);
			}
			else
				throw ErrorHttp("400 Bad Request", request.error["400"]);
		}
		else if (request.GetMethod() == "DELETE") {
			response = Delete(request);
		}
		else {
			throw ErrorHttp("501 Not Implemented", request.error["501"]);
		}
	} catch (ErrorHttp &e) {
		body = _html_map[e.get_errcode()];
		response = http_version + " " + e.what() + "\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) + "\n\n" + body;
		std::cout << "response: " << response << std::endl;
	}
	if (send(clientfd, response.c_str(), response.size(), 0) == -1)
		throw Webserv::ErrorSendingData();
	return 1;
}

std::string Webserv::Delete(Request &request) {
	std::string response = "";
	std::string name = "./www" + request.root + request.GetFullUri2();// + request.GetCgiExt();
	std::cout << "DELEETE name: " << name << std::endl;
	if (std::remove(name.c_str())) {
		throw ErrorHttp("500 Internal Server Error", request.error["500"]);
	}
	response = http_version + " 200 OK\nContent-Type: text/html\nContent-Length: 0\n\n";
	return response;
}
