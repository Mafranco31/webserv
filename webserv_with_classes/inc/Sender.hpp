#ifndef SENDER_HPP
# define SENDER_HPP

# include <iostream>
# include <string>
# include "header.hpp"
# include "Server.hpp"

class Webserv;

class ErrorHttp;

class Request;

class Sender {

	private:
		std::string http_version;
		std::string path_to_html;
		std::map<std::string, std::string> _html_map;

	public:
		Sender( void );
		Sender( const std::string &path_to_html, const std::string &path_to_err );
		~Sender();

		void	Send(int clientfd, std::string buffer, char **env);
		void	ReadPath( std::string path , std::string last_path );
		void	ReadFile( std::string file , std::string last_path );
		std::string Post( int clientfd, Request &request );
		std::string Delete( Request &request );

		Webserv *_ws;

		void choose_server_block(Request &request);
		void server_configuration(Request &request);
		void choose_location_block(Request &request);
		void recursive_location(Location &loc, Request &request);
};

#endif
