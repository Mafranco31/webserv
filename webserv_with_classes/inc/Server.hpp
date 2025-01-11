#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include "header.hpp"
# include "Servers_parse.hpp"
# define MAX_EVENTS 100

class Request;
class Location;
class Webserv;

class Server
{
	private:

	public:

		int serverfd;
		int maxfd;
		struct sockaddr_in serveraddr;

		//KQUEUE
		//struct kevent change_event;//, events[MAX_EVENTS];
		//void ManageConnexion(struct kevent *events);
		
		//EPOLL
		struct epoll_event change_event; //events[MAX_EVENTS];
		void ManageConnexion(struct epoll_event *events);

		char **_env;
		Webserv *_ws;
		//Sender &_sender;
		int _port;
		std::string _host;
		int &ep;
		int &nev;
		std::vector<int> fds;

		Server(char **env, Webserv *ws, std::string host, std::string port, int &serv_ep, int &serv_nev);
		~Server();
		//void Wait( void );
		void Start( void );
		void Stop( void );
		void handle_chunked_request(int client_fd);
		std::map<int, std::string> epmap;
};

class Webserv {

	private:
		//std::map<std::string, std::string> _html_map;
		//void ReadPath( std::string path , std::string last_path );
		//void ReadFile( std::string file , std::string last_path );
		//void Send( int clientfd, char *buffer );

	public:
		std::vector<Server> sub_server;
		char **env;
		int ep;
		int nev;

		//EPOLL
		struct epoll_event events[MAX_EVENTS];

		//KQUEUE
		//struct kevent events[MAX_EVENTS];
		//struct timespec timeout;

		Webserv();
		~Webserv();

		//void Initialize( std::string &path_to_html, std::string &path_to_err );
		void Wait( void );

		//Configuration file
		std::set<std::string> valid_directives;
		std::set<std::string> valid_directives_location;

		void make_list(std::string line);
		void parse(std::string path);

		void data_structure(void);

		void count_servers(void);
		void count_location_blocks(void);
		void sub_location_blocks(void);
		void prepare_location_parse(void);
		void last_function(int &bracket, std::vector<std::string>::iterator &it, Location &location);
		void location_parse(int &bracket, std::vector<std::string>::iterator &it, Location &location, int n);

		void check_input(void);
		void recursive_location(Location &loc);

		void listen_set(void);

		void check(void);

		void recursive_clear(Location &location);
		void clean(void);

		std::vector<std::string> _v;
		Servers_parse *serv;
		int serv_n;

		//listen (configuration file)
		std::set<std::string> _listen_set;
		std::vector<std::string> _host;
		std::vector<std::string> _port;


		//Sender
		std::string http_version;
		std::string path_to_html;
		std::map<std::string, std::string> _html_map;

		void initialize_path( const std::string &path_to_html, const std::string &path_to_err );

		int	Send(int clientfd, std::string buffer, char **env, struct epoll_event *events, int ep);
		void	ReadPath( std::string path , std::string last_path );
		void	ReadFile( std::string file , std::string last_path );
		std::string Post( int clientfd, Request &request );
		std::string Delete( Request &request );

		void choose_server_block(Request &request);
		void server_configuration(Request &request);
		void choose_location_block(Request &request);
		void recursive_location(Location &loc, Request &request);
		void location_configuration(Request &request);

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
