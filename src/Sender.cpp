#include "Server.hpp"

void Webserv::initialize_path ( const std::string &path_to_html, const std::string &path_to_err ) {
	(void)path_to_err; //errwww goes inside www.
	http_version = "HTTP/1.1";
	std::string last_path = "/";
	try {
		ReadPath(path_to_html, last_path);
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
	buffer << '\0';
	if (file.find(".") != std::string::npos) {
		_html_map[last_path + file.substr(0, file.find("."))] = buffer.str();
	}
	else
		_html_map[last_path + file] = buffer.str();
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
		throw ErrorHttp("404 Not Found", request.error["404"]);
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
	if (host.find(':') == std::string::npos)
	{
		request._host = host;
		request._host.erase(std::remove(request._host.begin(), request._host.end(), ' '), request._host.end());
		request._port = "80";
	}
	else
	{
		request._host = std::string(host.begin() + 1, host.begin() + host.find(':'));
		request._port =  std::string(host.begin() + host.find(':') + 1 , host.end());

	}
	request._host.erase(request._host.find_last_not_of('\r') + 1);
	request._port.erase(request._port.find_last_not_of('\r') + 1);
	for (int i = 0; i < serv_n; i++ )
	{
		if (serv[i].port == request._port && serv[i].d.find("server_name") != serv[i].d.end())
		{
			for (std::vector<std::string>::iterator it = serv[i].d["server_name"].begin(); it != serv[i].d["server_name"].end(); it++)
			{
				if (request._host == *it)
				{
					request.serv_block = &serv[i];
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
			return ;
		}
	}
	if (request.serv_block == NULL)
    {
        throw ErrorHttp("400 Bad Request", request.error["400"]);
    }
}



void Webserv::recursive_location(Location &loc, Request &request)
{
	if (request.uri.find(loc.prefix) == 0 && loc.prefix.length() > tmp_prefix.length())
	{
		tmp_prefix = loc.prefix;
		request.location_block = &loc;
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
		request.redir = map["return"][0];
	}
	if (map.find("index") != map.end())
	{
		if (map["index"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		for (std::vector<std::string>::iterator it = map["index"].begin(); it != map["index"].end(); it++)
		{
			request.index = "/www" + request.root + "/" + (*it).substr(0, (*it).find("."));
			if (_html_map.find(request.index) != _html_map.end())
			{
				 //Si el request a "/" redirige al contenido de index.
				if (request.uri[request.uri.size() - 1] == '/')
					request.index_uri = _html_map[request.index];
				break ;
			}
		}
	}
	std::vector<std::vector<std::string> > err = request.serv_block->err_page;
	if (err.size() > 0)
	{
		for (std::vector<std::vector< std::string> >::iterator it1 = err.begin(); it1 != err.end(); it1++)
		{
			if ((*it1).size() < 2)
				continue ;
			std::string tmp = *((*it1).begin() + (*it1).size() - 1);
			for (std::vector<std::string>::iterator it2 = (*it1).begin(); it2 != ((*it1).end() -1); it2++)
			{
				request.error[*it2] = "/www" + request.root + tmp.substr(0, tmp.find("."));
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
}

void Webserv::location_configuration(Request &request)
{
	std::map<std::string, std::vector<std::string> > map = request.location_block->data;
	if (map.find("root") != map.end())
	{
		if (map["root"].size() != 1)
			throw Webserv::InvalidConfigurationFile();
		request.root = map["root"][0];
	}
	if (map.find("index") != map.end())
	{
		if (map["index"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		for (std::vector<std::string>::iterator it = map["index"].begin(); it != map["index"].end(); it++)
		{
			request.index = "/www" + request.root + "/" + (*it).substr(0, (*it).find("."));
			if (_html_map.find(request.index) != _html_map.end())
			{
				if (request.uri[request.uri.size() - 1] == '/')
					request.index_uri = _html_map[request.index];
				break ;
			}
		}
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
		if (map["return"].size() < 1)
			throw Webserv::InvalidConfigurationFile();
		request.redir = map["return"][0];
	}
	if (map.find("limit_except") != map.end())
	{
		if (std::find(map["limit_except"].begin(), map["limit_except"].end(), request.GetMethod()) == map["limit_except"].end())
		{
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
		
		if (request.Parse(buffer, clientfd) == 2)
			return 2;
		choose_server_block(request);
		server_configuration(request);

		choose_location_block(request);
		
		if (request.serv_block->location_blocks != 0 && request.location_block != NULL)
			location_configuration(request);
		if (request.uri[request.uri.size() - 1] != '/') //If it's /, it stays like that because / is been linked to the index. I
			request.uri = "/www" + request.root + request.GetUri();
		if (request.GetMethod() == "GET") {
			if (request.redir != "")
			{
				response = http_version + " 301 Moved Permanently\nLocation: " + request.redir + "\nContent-Length: 0\n\n";
			}
			else if (request.uri[request.uri.size() - 1] == '/')
			{
				if (request.index_uri != "")
					body = request.index_uri;
				else if (request.autoindex == "on")
					body = CreateAutoIndex(request);
				else
					throw ErrorHttp("404 Not Found", request.error["404"]);
				response = http_version  + " 200 OK\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) + "\n\n" + body;
			}
			else if (request.GetIsCgi()) {

				if (_html_map[request.GetUri()] != "") {
					try {
						response = ft_ex_cgi_get(request);
					} catch (ErrorHttp &e) {
						throw ;
					}
					if (send(clientfd, response.c_str(), response.size(), 0) < 1){
						return 3;
					}
					return 1;
				}
				else
					throw ErrorHttp("404 Not Found", request.error["404"]);
			}
			else if (_html_map[request.GetUri()] != "") {
				body = _html_map[request.GetUri()];
				response = http_version  + " 200 OK\nContent-Type: text/html\nContent-Length: " + ft_strlen(body) + "\n\n" + body;
			}
			else
				throw ErrorHttp("404 Not Found", request.error["404"]);

		}
		else if (request.GetMethod() == "POST") {
			if (request.limit_size < (long int)request.GetBodyLength())
				throw ErrorHttp("413 Request Entity Too Large", request.error["413"]);

			if (request.GetHeaders()["CONTENT-TYPE"].find("plain/text") != std::string::npos){
				if (!homemade_post(request)) {
					response = http_version + " 200 Ok\nContent-Type: text/html\nContent-Length: 0" +  "\n\n";
				}
				else {
					response = http_version + " 400 Bad Request\nContent-Type: text/html\nContent-Length: 0" + "\n\n";
				}
				if (send(clientfd, response.c_str(), response.size(), 0) < 1){
					return 3;
				}
				return 1;
			}
			else if (request.GetIsCgi()) {
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
					if (send(clientfd, response.c_str(), response.size(), 0) < 1){
						return 3;
					}
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
	}
	if (send(clientfd, response.c_str(), response.size(), 0) < 1){
		return 3;
	}
	return 1;
}

bool Webserv::fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

std::string Webserv::getUniqueFilename(const std::string& baseName, const std::string& extension, const std::string& directory) {
    int counter = 1;
    std::string newFileName;

    // Construct the initial file name
    newFileName = directory + "/" + baseName + extension;

    // Iterate and check if the file already exists
    while (fileExists(newFileName)) {
        // Construct a new file name with an incremented counter
        newFileName = directory + "/" + baseName + "_" + ft_itoa(counter) + extension;
        ++counter;
    }

    return newFileName;
}


int Webserv::homemade_post(Request &request) {
	std::string content = request.GetBody();

    // Output file stream to create and write to a file

	std::string uniqueFileName = getUniqueFilename("newFile", ".txt", "www" + request.root + "/cgi-bin/uploads");

    std::ofstream ofs(uniqueFileName.c_str());

    // Check if the file was created successfully
    if (ofs.is_open()) {
			ofs << content;
			ofs.close();
	} else {
		return 1;
	}
    return 0;
}


std::string Webserv::Delete(Request &request) {
	std::string response = "";
	std::string name = "./www" + request.root + request.GetFullUri2();// + request.GetCgiExt();
	if (std::remove(name.c_str())) {
		throw ErrorHttp("500 Internal Server Error", request.error["500"]);
	}
	response = http_version + " 200 OK\nContent-Type: text/html\nContent-Length: 0\n\n";
	return response;
}
