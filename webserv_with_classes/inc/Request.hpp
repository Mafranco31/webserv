#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "header.hpp"

class Request {

	private:
		std::string	content;
		std::string	method;
		std::string	uri;
		std::string	version;
		std::string body;
		std::map<std::string, std::string> headers;
	public:
	//	Canonical form
		Request();
		~Request();

	//	Methods
		void	Parse( char *buffer );
		void	ParseFirstLine( void );
		void	ParseHeader( void );
		void	ParseBody( void );

	//	Getters
		std::string	GetMethod( void ) const {	return method;	}
		std::string	GetUri( void ) const ;
		std::string	GetVersion( void ) const {	return version;	}
		std::map<std::string, std::string> GetHeaders( void ) const {	return headers;	}
		std::string GetBody( void ) const {	return body;	}

	//	Exceptions
		class ErrorMethodNotImplemented: public std::exception		{	const char	*what() const throw ();	};
		class ErrorPathNotFound: public std::exception		{	const char	*what() const throw ();	};
		class ErrorVersionNotImplemented: public std::exception		{	const char	*what() const throw ();	};
		class ErrorNoHeaderFound: public std::exception		{	const char	*what() const throw ();	};
		class ErrorSpaceInKeyHeaderEnding: public std::exception		{	const char	*what() const throw ();	};
};

std::ostream& operator<<(std::ostream& os, const Request& request);

#endif