#include "CGI.hpp"

CGI::CGI(): _env(NULL), _exe(""), _cmd("") {}

CGI::CGI(std::string const & exe, std::string const & cmd, char ** env,\
std::map<std::string, std::string> const & headers): _env(env), _exe(exe), _cmd(cmd), _headers(headers) {}

CGI::CGI(CGI const & instance): _env(instance._env), _exe(instance._exe), _cmd(instance._cmd) {}

CGI::~CGI() {}

CGI &	CGI::operator=(CGI const & instance)
{
	if (this != &instance)
	{
		_env = instance._env;
		_exe = instance._exe;
		_cmd = instance._cmd;
	}
	return *this;
}

int		CGI::processCGI()
{
	int	pid;
	int status;
	int	fd[2];


	if (access(_cmd.data(), X_OK) == -1)
		throw(Error::AccessException());
	if (pipe(fd) == -1)
		throw (Error::PipeException());
	if ((pid = fork()) == -1)
		throw (Error::ForkException());
	else if (!pid)
	{
		//char **	tab;

		//if (_cmd == )

		//if (execve(this->_exe.c_str(), tab, _env) == -1)
			//exit(-1);
		exit(0);
	}
	waitpid(pid, &status, 0);
	return (status);
}

/* all cgi env
AUTH_TYPE = Authorization
CONTENT_LENGTH = Content-Length
CONTENT_TYPE = Content-type
GATEWAY_INTERFACE = CGI/1.1
HTTP_HOST = Host
HTTP_ACCEPT = Accept
HTTP_USER_AGENT = User-Agent
HTTP =
PATH_INFO =
PATH_TRANSLATED
QUERY_STRING
REMOTE_ADDR
REMOTE_HOST
REMOTE_IDENT = Authorization
REMOTE_USER = Authorization
REQUEST_METHOD
SCRIPT_NAME
SERVER_NAME
SERVER_PORT
SERVER_PROTOCOL
SERVER_SOFTWARE
*/