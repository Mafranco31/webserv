#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "header.hpp"
# include "Servers_parse.hpp"

class ErrorHttp;

class Servers_parse;

class Request {

	private:
		std::string	content;
		std::string	method;
		//std::string	uri;
		std::string	version;
		std::string body;
		size_t body_length;
		std::map<std::string, std::string> headers;
		int is_cgi;
		std::string cgi_ext;
		std::map<std::string, std::string>  _marg;
		int	nb_args;
		int client_socket;
		std::string query_string;

	public:

		std::string _host;
		std::string _port;
		Servers_parse *serv_block;
		Location *location_block;

		std::string	uri;

		//Configuration parameters:
		std::string root;
		std::string index;
		int limit_size;

		std::map<std::string, std::string> error;


	//	Canonical form
		Request();
		~Request();

	//	Methods
		void	Parse( std::string buffer, int clientfd );
		void	ParseFirstLine( void );
		void	ParseHeader( void );
		void	IsCGI();
		void	get_args(std::string args);
		void	ChunkedBody();
		// void	ParseBody( void );

	//	Getters
		std::string	GetMethod( void ) const {	return method;	}
		std::string	GetUri( void ) const ;
		std::string GetFullUri( void ) const {	return uri;	}
		std::string	GetVersion( void ) const {	return version;	}
		std::string	GetQueryString( void ) const {	return query_string;	}
		std::map<std::string, std::string> GetHeaders( void ) const {	return headers;	}
		std::string GetBody( void ) const {	return body;	}
		size_t GetBodyLength( void ) const {	return body_length;	}
		int GetIsCgi( void ) const { return is_cgi; }
		std::string GetCgiExt (void) const {return cgi_ext;}
		int GetNbArgs( void ) const { return nb_args; }
		std::map<std::string, std::string> GetMarg( void ) const {	return _marg;	}

	//	Exceptions
		class ErrorMethodNotImplemented: public std::exception		{	const char	*what() const throw ();	};
		class ErrorPathNotFound: public std::exception		{	const char	*what() const throw ();	};
		class ErrorVersionNotImplemented: public std::exception		{	const char	*what() const throw ();	};
		class ErrorNoHeaderFound: public std::exception		{	const char	*what() const throw ();	};
		class ErrorSpaceInKeyHeaderEnding: public std::exception		{	const char	*what() const throw ();	};
};

std::ostream& operator<<(std::ostream& os, const Request& request);

#endif
