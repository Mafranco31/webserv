#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include "header.hpp"
# define MAX_EVENTS 100

class Sender;

class Location
{
	public:
		Location();
		~Location();
		std::string prefix;
		int sub_location_blocks;
		Location *sub_block;
		std::map<std::string, std::vector<std::string> > data;
};

class Servers
{
	public:
		Servers();
		~Servers();
		std::map<std::string, std::vector<std::string> > d; //eg. server_name (key) server.com server.org server.net (parameters -> vector)
		Location *location;
		int location_blocks;

};

class Server {

	private:
		int serverfd;
		int maxfd;
		struct sockaddr_in serveraddr;
		struct kevent change_event, events[MAX_EVENTS];
		struct timespec timeout;
		int	kq;
		int	nev;

		char **env;
		Sender & sender;

		//std::map<std::string, std::string> _html_map;
		//void ReadPath( std::string path , std::string last_path );
		//void ReadFile( std::string file , std::string last_path );
		//void Send( int clientfd, char *buffer );

	public:
		Server( Sender &s, char **env );
		~Server();

		//void Initialize( std::string &path_to_html, std::string &path_to_err );
		void Start( void );
		void Stop( void );
		void Wait( void );
		void ManageConnexion( void );

		//Configuration file
		void make_list(std::string line);
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
		std::vector<std::string> _v;
		Servers *serv;
		int serv_n;

	//	Exceptions
		class ErrorCreatingSocket: public std::exception	{	const char	*what() const throw ();	};
		class ErrorBindingSocket: public std::exception		{	const char	*what() const throw ();	};
		class ErrorListeningSocket: public std::exception	{	const char	*what() const throw ();	};
		class ErrorAcceptingSocket: public std::exception	{	const char	*what() const throw ();	};
		class ErrorCreatingKqueue: public std::exception	{	const char	*what() const throw ();	};
		class ErrorInitializeKqueue: public std::exception	{	const char	*what() const throw ();	};
		class ErrorGettingEvent: public std::exception		{	const char	*what() const throw ();	};
		class ErrorReadingSocket: public std::exception		{	const char	*what() const throw ();	};
		class ErrorSendingData: public std::exception		{	const char	*what() const throw ();	};
		class ErrorReadingHtmlPath: public std::exception	{	const char	*what() const throw ();	};
		class OpenFileException: public std::exception		{	const char	*what() const throw ();	};
		class InvalidConfigurationFile: public std::exception	{const char	*what() const throw ();	};
};

#endif
