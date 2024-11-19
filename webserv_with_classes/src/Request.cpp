#include "Request.hpp"

Request::Request ( void ) : body(""), body_length(0) {
	std::cout << "Default Request constructor called" << std::endl;
}

Request::~Request ( void ) {
	std::cout << "Destructor Request called" << std::endl;
}

// Methods

void	Request::Parse ( char *buffer ) {
	content = static_cast<std::string>(buffer);

	try {
		ParseFirstLine();
		ParseHeader();
		if (headers.find("content-length") != headers.end())// || headers.find("transfer-encoding") != headers.end())
		{
			body_length = std::atoi(headers["content-length"].c_str());
			ParseBody();
		}
	} catch (std::exception &e) {
		throw ;
	}
}

void	Request::ParseFirstLine ( void ) {
	//	Parsing the first line of the request
	std::string firstLine = content.substr(0, content.find('\n'));
	if (firstLine.empty()) throw ErrorHttp("400 Bad Request", "/400");
	size_t pos = firstLine.find_first_of(" \t\r\v\f");
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	method = firstLine.substr(0, pos);
	
	pos = firstLine.find_first_not_of(" \t\r\v\f", firstLine.find_first_of(" \t\r\v\f"));
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	size_t pos2 = firstLine.find_first_of(" \t\r\v\f", pos);
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	uri = firstLine.substr(pos, pos2 - pos);

	pos = firstLine.find_first_not_of(" \t\r\v\f", pos2);
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	pos2 = firstLine.find_first_of(" \t\r\v\f\n", pos);
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	version = firstLine.substr(pos, pos2 - pos);
}

void	Request::ParseHeader ( void ) {
	//	Parsing the headers of the request
	size_t pos = content.find_first_of("\n") + 1;
	if (!pos || pos == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	std::string header = content.substr(pos, content.size() - pos);
	size_t	pos1 = 0;
	size_t	pos2 = header.find('\n');
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", "/400");
	std::string headerLine = header.substr(pos1, pos2);

	while (headerLine.find_first_of(":") != std::string::npos) {
		if (headerLine[0] == ':') throw ErrorHttp("400 Bad Request", "/400");
		std::string	key = headerLine.substr(0, headerLine.find(':'));
		if (key[key.size() - 1] == ' ') throw ErrorHttp("400 Bad Request", "/400");
		for (std::string::iterator it = key.begin(); it != key.end(); ++it) {
			if (*it >= 97 && *it <= 122) *it -= 32;
		}
		if (headerLine.find(':') + 1 == headerLine.size()) throw ErrorHttp("400 Bad Request", "/400");
		std::string value = headerLine.substr(headerLine.find(':') + 1, headerLine.size() - headerLine.find(':') - 1);
		headers.insert(std::pair<std::string, std::string>(key, value));

		pos1 = pos2 + 1;
		pos2 = header.find('\n', pos1);
		if (pos2 == std::string::npos) break;
		headerLine = header.substr(pos1, pos2 - pos1);
	}
}

void	Request::ParseBody ( void ) {
	//	Parsing the body of the request
	size_t pos = content.find("\n\n") + 2;
	if (pos == std::string::npos) pos = content.find("\n\r\n") + 3;
	if (pos == std::string::npos) return;
	size_t i = content.size() - pos;
	if (i > body_length) i = body_length;
	body = content.substr(pos, i);
}

//	Overload operator<<
std::ostream& operator<<(std::ostream& os, const Request& request) {
	os << "-Method: " << request.GetMethod() << std::endl;
	os << "-URI: " << request.GetUri() << std::endl;
	os << "-Version: " << request.GetVersion() << std::endl;
	os << "-Headers:" << std::endl;
	std::map<std::string, std::string>	headers = request.GetHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		os << it->first << ": " << it->second << std::endl;
	}
	os << "-Body: " << request.GetBody() << std::endl;
	return os;
}

//	Getters

std::string	Request::GetUri( void ) const {
	if (uri.find(".") != std::string::npos) {
		return uri.substr(0, uri.find("."));
	}
	return uri;
}

//	Exceptions
const char	*Request::ErrorMethodNotImplemented::what()	const throw () {	return "Error: method not implemented.";	}

const char	*Request::ErrorPathNotFound::what()	const throw () {	return "Error: path not found.";	}

const char	*Request::ErrorVersionNotImplemented::what()	const throw () {	return "Error: version not implemented.";	}

const char	*Request::ErrorNoHeaderFound::what()	const throw () {	return "Error: no header found.";	}

const char	*Request::ErrorSpaceInKeyHeaderEnding::what()	const throw () {	return "Error: header key ending with a space.";	}