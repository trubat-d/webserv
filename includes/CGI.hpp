#ifndef CGI_HPP
#define CGI_HPP

#include "Includer.hpp"

class CGI {

public:

	typedef std::map<std::string, std::string>::iterator	map_iterator;

	CGI();

	CGI(std::string const & exe, std::string const & cmd, char ** env,\
	std::map<std::string, std::string> const & headers);

	CGI(CGI const & instance);

	~CGI();

	CGI &	operator=(CGI const & instance);

	int		processCGI();

private:

	char **		_env;
	std::string	_exe;
	std::string	_cmd;
	std::map<std::string, std::string>	_headers;
};

#endif