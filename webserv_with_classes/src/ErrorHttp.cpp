#include "ErrorHttp.hpp"

ErrorHttp::ErrorHttp( const std::string & errmsg, const std::string & errcode ) : errmsg(errmsg), errcode(errcode) {};

ErrorHttp::~ErrorHttp ( void ) throw() {
	std::cout << "Destructor ErrorHttp called" << std::endl;
}

const char *ErrorHttp::what() const throw() { return errmsg.c_str(); };

std::string ErrorHttp::get_errcode() const { return errcode; }