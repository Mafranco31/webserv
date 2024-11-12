#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include "header.hpp"
# define MAX_EVENTS 100


class Server {

	private:
		int serverfd;
		int maxfd;
		struct sockaddr_in serveraddr;
		struct kevent change_event, events[MAX_EVENTS];
		struct timespec timeout;
		int	kq;
		int	nev;

		std::map<std::string, std::string> _html_map;
		void ReadPath( std::string path , std::string last_path );
		void ReadFile( std::string file , std::string last_path );
		void Send( int clientfd, char *buffer );

	public:
		Server( void );
		~Server();

		void Initialize( std::string &path_to_html, std::string &path_to_err );
		void Start( void );
		void Stop( void );
		void Wait( void );
		void ManageConnexion( void );

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
};

#endif