/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfFile.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlitran <dlitran@student.42barcelona.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 12:47:44 by dlitran           #+#    #+#             */
/*   Updated: 2024/11/11 12:47:44 by dlitran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <fstream>
#include <sstream>



class Location
{
	public:
		Location();
		~Location();
		std::string prefix;
		int sub_location_blocks; //initialize to 0.
		Location *sub_block;
		std::map<std::string, std::vector<std::string> > data;
		//std::string *valid_directives_location = {"a", "b"};
};

class Servers
{
	public:
		Servers();
		~Servers();
		std::map<std::string, std::vector<std::string> > d; //eg. server_name (key) server.com server.org server.net (parameters -> vector)
		Location *location;
		int location_blocks; //initialize to 0.

};

class ConfFile
{
	private:
		void make_list(std::string line);
		std::vector<std::string> _v;
		//valid_directives = {"listen", "root", "index", "server_name", "location", "allow_methods", "cgi_pass"}
		Servers *serv;
		int serv_n;


	public:
		ConfFile();
		~ConfFile();
		void parse(std::string path);
		void data_structure(void);
		void count_servers(void);
		void count_location_blocks(void);
		void sub_location_blocks(void);
		void last_function(int &bracket, std::vector<std::string>::iterator &it, Location &location);
		void location_parse(int &bracket, std::vector<std::string>::iterator &it, Location &location, int n);
		void check(void);
		void recursive_clear(Location &location);
		void clean(void);

		class OpenFileException: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		class InvalidConfigurationFile: public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
};
