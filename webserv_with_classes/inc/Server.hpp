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

	public:
		Server( void );
		~Server();

		void Start( void );
		void Stop( void );
		void Wait( void );
		void ManageConnexion( void );
		void Send( int clientfd, char *buffer );

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

};

#endif