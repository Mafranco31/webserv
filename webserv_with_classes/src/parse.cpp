#include "Server.hpp"
#include "../inc/Servers_parse.hpp"

Servers_parse::Servers_parse(): location(NULL), location_blocks(0)
{

}

Servers_parse::~Servers_parse()
{

}

Location::Location(): sub_location_blocks(0), sub_block(NULL), eq(0)
{

}

Location::~Location()
{

}

//blocks: server
//directives: listen, server_name, listen + default_server,  error_page, client_max_body_size, location, limit_except, return/rewrite, root, autoindex, index, fastcgi_pass, include, GET, POST, fastcgi_param, proxy_pass
void Webserv::make_list(std::string line)
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

void Webserv::parse(std::string path)
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
		throw Webserv::OpenFileException();
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



void Webserv::last_function(int &bracket, std::vector<std::string>::iterator &it, Location &location)
{
	//std::vector<std::string>::const_iterator start;
	std::vector<std::vector<std::string>::iterator> its;
	int n;
	int counter;

	//std::cout << "seg" << std::endl;
	//start = it;
	//std::cout << "sub_location block: "<< *it << std::endl;
	counter = 0;
	n = bracket;
	bracket++;
	it++;
	if (!(*it).compare("="))
	{
		it++;
		location.eq = 1;
	}
	location.prefix = *it;
	it++;
	if (*(it) != "{")
	{
		throw InvalidConfigurationFile();
	}
	it++;
	while (bracket != n)
	{
		if (!(*it).compare("{"))
		{
			if ((*(it - 2)).compare("location") && (*(it - 2)).compare("="))
			{
				throw InvalidConfigurationFile();
			}
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

void Webserv::sub_location_blocks()
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
		else if (bracket == 1 && !((*it).compare("location")) && (((it + 2) != _v.end() && *(it + 2) == "{") || ((it + 1) != _v.end() && (it + 3) != _v.end() && *(it + 1) == "=" && *(it + 3) == "{")))
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

void Webserv::count_location_blocks(void) //inicialmente n = 1, con recursividad va aumentando.
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
				//std::cout << "\033[34mlo que sea\033[0m" <<std::endl;
				//std::cout << serv_it << std::endl;
				if (serv[serv_it].location_blocks != 0)
					this->serv[serv_it].location = new Location[serv[serv_it].location_blocks];
				serv_it++;
			}
		}
		else if (bracket == 1 && !((*it).compare("location")) && (((it + 2) != _v.end() && *(it + 2) == "{") || ((it + 1) != _v.end() && (it + 3) != _v.end() && *(it + 1) == "=" && *(it + 3) == "{")))
		{
			serv[serv_it].location_blocks++;
			it++;
			if (*(it) == "=")
				it++;
			it++;
			if (*it != "{")
			{
				throw InvalidConfigurationFile();
			}
			else
				bracket++;
		}
		else if (bracket == 1 && (*it == "{" || *it == "}"))
		{
			throw InvalidConfigurationFile();
		}
		else
			continue ;
	}
}

void Webserv::count_servers(void)
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
	this->serv = new Servers_parse[server_blocks];
}

void Webserv::location_parse(int &bracket, std::vector<std::string>::iterator &it, Location &location, int n) //n -> bracket inicial
{
	it++;
	if ((*it) == "=")
	{
		it++;
		location.eq = 1;
	}
	location.prefix = *it;
	int count;
	count = 0;
	it++;
	//std::cout << *it << std::endl;
	if ((*it).compare("{"))
		throw InvalidConfigurationFile();
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
				std::string key;
				key = *it;
				if (valid_directives_location.find(key) == valid_directives_location.end())
				{
					throw InvalidConfigurationFile();
				}
				//std::cout << *it << std::endl;
				it++;
				//std::cout << *it << std::endl;
				if (!(*it).compare(";"))
				{
					std::cout << key << ": no parameters." << std::endl;
					throw InvalidConfigurationFile();
				}
				if (key == "error_page")
				{
					location.err_page.push_back(std::vector<std::string>());
					while ((*it).compare(";"))
					{
						if (*it == "{" || *it == "}")
						{
							throw InvalidConfigurationFile();
						}
						(*(location.err_page.begin() + location.err_page.size() - 1)).push_back(*it);
						it++;
					}
				}
				else
				{
					while ((*it).compare(";")) //not equal to ';'
					{
						if (*it == "{" || *it == "}")
						{
							throw InvalidConfigurationFile();
						}
						//std::cout << n << " prefix: " << location.prefix << " la key: " << key << std::endl;
						location.data[key].push_back(*it);
						//this->serv[serv_it].d[key].push_back(*it);
						//Guardar en class location.
						it++;
					}
				}
				//std::cout << "salida: " << *it << std::endl;
			}
		}
		it++;
	}
}

void Webserv::prepare_location_parse(void)
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
			if (bracket == 0) //When bracket is 0 after '}', a new server block begins. -> We've checked previously that this is the case (there's just the word server when bracket == 0).
			{
				serv_it++;
				loc_it = 0;
			}
			else if (bracket == 1)
				loc_it++;
		}
		else if (!(*it).compare("location") && bracket == 1 && (((it + 2) != _v.end() && *(it + 2) == "{") || ((it + 1) != _v.end() && (it + 3) != _v.end() && *(it + 1) == "=" && *(it + 3) == "{")))
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
			if (valid_directives.find(key) == valid_directives.end())
			{
				throw InvalidConfigurationFile();
			}
			it++;
			if (!(*it).compare(";"))
			{
				std::cout << key << ": no parameters." << std::endl;
				throw InvalidConfigurationFile();
			}
			if (key == "error_page")
			{
				this->serv[serv_it].err_page.push_back(std::vector<std::string>());
				while ((*it).compare(";"))
				{
					if (*it == "{" || *it == "}")
					{
						throw InvalidConfigurationFile();
					}
					(*(this->serv[serv_it].err_page.begin() + this->serv[serv_it].err_page.size() - 1)).push_back(*it);
					it++;
				}
			}
			else
			{
				while ((*it).compare(";"))
				{
					if (*it == "{" || *it == "}")
					{
						throw InvalidConfigurationFile();
					}
					this->serv[serv_it].d[key].push_back(*it);
					it++;
				}
			}
		}
	}
}

void Webserv::listen_set(void)
{
	for (int i = 0; i < serv_n; i++)
	{
		//std::cout << serv[i].d["listen"][0] << std::endl;
		if (serv[i].d["listen"].size() != 1)
		{
			std::cout << "no listen port" << std::endl;
			throw InvalidConfigurationFile();
		}
		_listen_set.insert(serv[i].d["listen"][0]);
		if ( serv[i].d["listen"][0].find(':') == std::string::npos) //No ':' -> argument must be just an interger (a port).
		{
			int n;
			std::stringstream ss(serv[i].d["listen"][0]);
			if (!(ss >> n))
				throw InvalidConfigurationFile();
			else
			{
				serv[i].host = "0.0.0.0";
				serv[i].port = serv[i].d["listen"][0];
			}
		}
		else //There's ':' -> host:port
		{
			std::string tmp1(serv[i].d["listen"][0].begin(), serv[i].d["listen"][0].begin() + serv[i].d["listen"][0].find(':'));
			std::string tmp2(serv[i].d["listen"][0].begin() + serv[i].d["listen"][0].find(':') +1 , serv[i].d["listen"][0].end());
			serv[i].host = tmp1;
			serv[i].port = tmp2; //Check validity of ip and port.
		}
		std::cout << i << ": " << serv[i].host << ", " << serv[i].port << std::endl;
	}
	for (std::set<std::string>::iterator it = _listen_set.begin(); it != _listen_set.end(); it++)
	{
		int n;
		std::stringstream ss(*it);
		if ( (*it).find(':') == std::string::npos) //No ':' -> argument must be just an interger (a port).
		{
			if (!(ss >> n))
				throw InvalidConfigurationFile();
			else
			{
				_host.push_back("0.0.0.0");
				_port.push_back(*it);
			}
		}
		else //There's ':' -> host:port
		{
			std::string tmp1((*it).begin(), (*it).begin() + (*it).find(':'));
			std::string tmp2((*it).begin() + (*it).find(':') +1 , (*it).end());
			_host.push_back(tmp1);
			_port.push_back(tmp2); //Check validity of ip and port.
		}
	}
}

void Webserv::recursive_location(Location &loc)
{
	if (loc.eq == 1 && loc.sub_block != 0) //A location = path {} block cannot have nested location blocks.
		throw InvalidConfigurationFile();

	std::cout << "sub_location_blocks: " << loc.sub_location_blocks << std::endl;
	for (int i = 0; i < loc.sub_location_blocks; i++)
		recursive_location(loc.sub_block[i]);

	std::map<std::string, std::vector<std::string> > map = loc.data;
	if (map.find("root") != map.end()) //root
	{
		if (map["root"].size() != 1)
		{
			std::cout << "root: wrong number of parameters." << std::endl;
			throw InvalidConfigurationFile();
		}
	}
	if (map.find("index") != map.end()) //index
	{
		std::cout << "arrives here" << std::endl;
		//std::cout << map["index"].size() << std::endl;
		//std::cout << map["index"][0] << map["index"][1] << std::endl;
		if (map["index"].size() == 0)
		{
			std::cout << "index: wrong number of parameters." << std::endl;
			throw InvalidConfigurationFile();
		}
	}
	if (map.find("return") != map.end()) //return
	{
		if (map["return"].size() != 1)
		{
			std::cout << "return: wrong number of parameters." << std::endl;
			throw InvalidConfigurationFile();
		}
	}
	if (map.find("client_body_buffer_size") != map.end()) //client_body_buffer_size
	{
		if (map["client_body_buffer_size"].size() != 1)
		{
			std::cout << "client_body_buffer_size: wrong number of parameters." << std::endl;
			throw Webserv::InvalidConfigurationFile();
		}
		std::stringstream ss(map["client_body_buffer_size"][0]);
		int n;
		ss >> n;
		if (ss.fail() || n < 0 ) //If it's not a natural number.
		{
			std::cout << "client_body_buffer_size: the parameter must be a positive number." << std::endl;
			throw Webserv::InvalidConfigurationFile();
		}
	}
}

void Webserv::check_input(void)
{
	for (int i = 0; i < serv_n; i++)
	{
		for (int j = 0; j < serv[i].location_blocks; j++)
		{
			recursive_location(serv[i].location[j]);
		}
	}
}

void Webserv::data_structure(void)
{
	count_servers();
	count_location_blocks();
	sub_location_blocks();
	prepare_location_parse();
	check_input();
	listen_set();
}

void Webserv::check(void)
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

void Webserv::recursive_clear(Location &location)
{
	for (int i = 0; i < location.sub_location_blocks; i++)
		recursive_clear(location.sub_block[i]);
	location.data.clear();
	if (location.sub_location_blocks > 0)
		if (location.sub_block)
			delete [] location.sub_block;
}

void Webserv::clean(void)
{
	for (int i = 0; i < serv_n; i++)
	{
		for (int j = 0; j < serv[i].location_blocks; j++)
			recursive_clear(serv[i].location[j]);
		if (serv[i].location_blocks > 0)
		{
			if (serv[i].location)
				delete [] serv[i].location;
		}
	}
	_v.clear();
	delete [] serv;
}

const char *Webserv::OpenFileException::what() const throw()
{
	return ("Error: Could not open file.");
}

const char *Webserv::InvalidConfigurationFile::what() const throw()
{
	return ("Error: Invalid configuration file.");
}
