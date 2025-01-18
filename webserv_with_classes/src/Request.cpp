#include "Request.hpp"

Request::Request ( void ) : body(""), body_length(0), serv_block(NULL), location_block(NULL), root(""), index(""), redir(""), autoindex("off"), limit_size(100000) {
	error["400"] = "";
	error["404"] = "";
	error["405"] = "";
	error["415"] = "";
	error["500"] = "";
	error["501"] = "";

	std::cout << "Default Request constructor called" << std::endl;
}

Request::~Request ( void ) {
	std::cout << "Destructor Request called" << std::endl;
}

// Methods

int	Request::Parse ( std::string buffer, int clientfd) {
	content = buffer;
	client_socket = clientfd;
	try {
		
		ParseFirstLine();
		ParseHeader();
		IsCGI();
		if (headers["CONTENT-LENGTH"] != "" && static_cast<int>(body_length) != std::atoi(headers["CONTENT-LENGTH"].c_str())) {
			if (static_cast<int>(body_length) > std::atoi(headers["CONTENT-LENGTH"].c_str()))
				throw ErrorHttp("400 Bad Request", error["400"]);
			std::cout << "Content-Length: " << headers["CONTENT-LENGTH"] << std::endl;
			// ChunkedBody(events, ep);
			return 2;
		}
	} catch (std::exception &e) {
		throw ;
	}
	return 1;
}

void	Request::IsCGI() {
	if (uri.find("/cgi-bin/") != std::string::npos)
	{
		is_cgi = 1;
		// size_t pos_point = uri.rfind('.');
		// if (pos_point != std::string::npos)
		// 	cgi_ext = uri.substr(pos_point);
		// else
		// 	throw ErrorHttp("404 Not Found", error["404"]);
	}
	else {
		is_cgi = 0;
		// cgi_ext = "";
	}
}

void	Request::get_args(std::string args) {
	if (args.empty())
		nb_args = 0;
	else {
		std::string arg;
		size_t posstart = 0;
		size_t posend = args.find('&');
		while (posend != std::string::npos) {
			arg = args.substr(posstart, posend - posstart);
			size_t posmid = arg.find('=');
			if (posmid != std::string::npos && posmid != arg.size() - 1 && posmid != 0)
				_marg.insert(std::pair<std::string, std::string>(decodePercentEncoding(arg.substr(0, posmid)), decodePercentEncoding(arg.substr(posmid + 1))));
			posstart = posend + 1;
			posend = args.find('&', posstart);
		}
		arg = args.substr(posstart);
		size_t posmid = arg.find('=');
		if (posmid != std::string::npos && posmid != arg.size() - 1 && posmid != 0)
			_marg.insert(std::pair<std::string, std::string>(decodePercentEncoding(arg.substr(0, posmid)), decodePercentEncoding(arg.substr(posmid + 1))));
		nb_args = _marg.size();
	}
}

void	Request::ParseFirstLine ( void ) {
	//	Parsing the first line of the request
	//	Getting the method
	std::string firstLine = content.substr(0, content.find('\n'));
	if (firstLine.empty()) throw ErrorHttp("400 Bad Request", error["400"]);
	size_t pos = firstLine.find_first_of(" \t\r\v\f");
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	method = firstLine.substr(0, pos);

	//	Getting the URI
	pos = firstLine.find_first_not_of(" \t\r\v\f", firstLine.find_first_of(" \t\r\v\f"));
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	size_t pos2 = firstLine.find_first_of(" \t\r\v\f", pos);
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	uri = firstLine.substr(pos, pos2 - pos);
	//	Looking for arguments
	uri2 = uri;
	if (uri.find('?') != std::string::npos) {
		query_string = uri.substr(uri.find('?') + 1);
		try {
			get_args(uri.substr(uri.find('?') + 1));
		} catch (std::invalid_argument &e) {
			throw ErrorHttp("400 Bad Request", error["400"]);
		}
		uri = uri.substr(0, uri.find('?'));
	}
	else {
		query_string = "";
		nb_args = 0;
	}

	//	Getting the version
	pos = firstLine.find_first_not_of(" \t\r\v\f", pos2);
	if (pos == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	pos2 = firstLine.find_first_of(" \t\r\v\f\n", pos);
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	version = firstLine.substr(pos, pos2 - pos);
}

void Request::ParseHeader( void ) {
	// Parsing the headers of the request
	size_t pos = content.find_first_of("\n") + 1;
	if (!pos || pos == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	std::string header = content.substr(pos, content.size() - pos);
	size_t pos1 = 0;
	size_t pos2 = header.find('\n');
	if (pos2 == std::string::npos) throw ErrorHttp("400 Bad Request", error["400"]);
	std::string headerLine = header.substr(pos1, pos2);

	while (!headerLine.empty() && headerLine.find_first_of(":") != std::string::npos) {
		if (headerLine[0] == ':') throw ErrorHttp("400 Bad Request", error["400"]);
		std::string key = headerLine.substr(0, headerLine.find(':'));
		if (key[key.size() - 1] == ' ') throw ErrorHttp("400 Bad Request", error["400"]);
		for (std::string::iterator it = key.begin(); it != key.end(); ++it) {
			if (*it >= 97 && *it <= 122) *it -= 32;
		}
		if (headerLine.find(':') + 1 == headerLine.size()) throw ErrorHttp("400 Bad Request", error["400"]);
		std::string value = headerLine.substr(headerLine.find(':') + 1, headerLine.size() - headerLine.find(':') - 1);
		
		//	Removing leading and trailing spaces
		// if (value.find_first_not_of(' ') != std::string::npos) {
		// 	size_t start_pos_space = value.find_first_not_of(' ');
		// 	if (start_pos_space != std::string::npos) {
		// 		value.erase(0, start_pos_space);
		// 	}
		// 	size_t end_pos_space = value.find_last_not_of(' ');
		// 	if (end_pos_space != std::string::npos) {
		// 		value.erase(end_pos_space + 1);
		// 	}
		// }

		headers.insert(std::pair<std::string, std::string>(key, value));

		pos1 = pos2 + 1;
		pos2 = header.find('\n', pos1);
		if (pos2 == std::string::npos) break;
		headerLine = header.substr(pos1, pos2 - pos1);
	}
	body = header.substr(pos2 + 1);
	body_length = body.size();
}

//	Overload operator<<
std::ostream& operator<<(std::ostream& os, const Request& request) {
	os << "-FULL URI:" << request.GetFullUri() << std::endl;
	os << "-Method: " << request.GetMethod() << std::endl;
	os << "-URI: " << request.GetUri() << std::endl;
	os << "-Version: " << request.GetVersion() << std::endl;
	os << "nb_args: " << request.GetNbArgs() << std::endl;
	std::map<std::string, std::string> marg = request.GetMarg();
	for (std::map<std::string, std::string>::const_iterator i = marg.begin(); i != marg.end(); ++i) {
		os << "arg: " << i->first << " = " << i->second << std::endl;
	}
	os << "-Headers:" << std::endl;
	std::map<std::string, std::string>	headers = request.GetHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		os << it->first << ": " << it->second << std::endl;
	}
	os << "-Body: " << request.GetBody() << std::endl;
	os << "-Body length: " << request.GetBodyLength() << "-Calculated body length:" << request.GetBody().size() << std::endl;
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
