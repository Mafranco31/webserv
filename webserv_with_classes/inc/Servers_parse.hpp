#ifndef SERVER_PARSE_HPP
# define SERVER_PARSE_HPP

# include <iostream>
# include <string>
# include "header.hpp"

class Location
{
	public:
		Location();
		~Location();
		std::string prefix;
		int sub_location_blocks;
		Location *sub_block;
		int eq;
		std::map<std::string, std::vector<std::string> > data;
		std::vector<std::vector<std::string> > err_page;
};

class Servers_parse
{
	public:
		Servers_parse();
		~Servers_parse();
		std::map<std::string, std::vector<std::string> > d; //eg. server_name (key) server.com server.org server.net (parameters -> vector)
		std::vector<std::vector<std::string> > err_page;
		Location *location;
		int location_blocks;
		std::string host;
		std::string port;

};

#endif
