#ifndef ERRORHTTP_HPP
# define ERRORHTTP_HPP

# include <iostream>
# include <string>

class ErrorHttp: public std::exception {

		private:
		std::string errmsg;
		std::string errcode;
	public:
		ErrorHttp( const std::string & errmsg, const std::string & errcode );
		~ErrorHttp() throw();
		const char *what() const throw();
		std::string get_errcode( ) const;
};

#endif