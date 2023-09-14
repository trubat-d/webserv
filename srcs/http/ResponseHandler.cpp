#include "Http.hpp"

HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(HttpRequest const & instance, struct kevent & socket)
{
	std::string	tmp;
	std::stringstream ss;
	struct sockaddr_in	sockAddr = {};
	socklen_t len = sizeof(sockAddr);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	this->_masterSocketInfo = * reinterpret_cast<uDada *>(socket.udata);
	ss << instance.getCtrlData();
	for (int i = 0; i < 3; i++)
		ss >> this->_ctrlData[i];
	this->_headers = instance.getHeaders();
	this->_body = instance.getBody();
	this->_cgiEnv.push_back("AUTH_TYPE=" + getHeader("Authorization"));
	this->_cgiEnv.push_back("CONTENT_LENGTH=" + getHeader("Content-Length"));
	this->_cgiEnv.push_back("CONTENT_TYPE=" + getHeader("Content-Type"));
	this->_cgiEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_cgiEnv.push_back("HTTP_USER_AGENT=" + getHeader("User-Agent"));
	this->_cgiEnv.push_back("HTTP_HOST=" + getHeader("Host"));
	this->_cgiEnv.push_back("HTTP_ACCEPT=" + getHeader("Accept"));
	this->_cgiEnv.push_back("HTTP_ACCEPT_CHARSET=" + getHeader("Accept-Charset"));
	this->_cgiEnv.push_back("HTTP_ACCEPT_LANGUAGE=" + getHeader("Accept-Language"));
	this->_cgiEnv.push_back("HTTP_CONNECTION=" + getHeader("Connection"));
	this->_cgiEnv.push_back("HTTP_REFERER=" + getHeader("Referer"));
	this->_cgiEnv.push_back("HTTP_USER_AGENT=" + getHeader("User-Agent"));
	this->_cgiEnv.push_back("SERVER_SOFTWARE=WebserverDeSesGrandsMorts/4.20.69");
	this->_cgiEnv.push_back("HTTP_COOKIE=" + getHeader("Cookie"));
	this->_cgiEnv.push_back("REMOTE_IDENT=" + getHeader("Authorization"));
	this->_cgiEnv.push_back("REMOTE_USER=" + getHeader("Authorization"));
//////////////////////////////////////////////////////////////////
//	this->_cgiEnv.push_back("PATH_INFO="); //bonus path
//	this->_cgiEnv.push_back("PATH_TRANSLATED="); // bonus path translation
//	this->_cgiEnv.push_back("QUERY_STRING="); // after the ?
//////////////////////////////////////////////////////////////////
	this->_cgiEnv.push_back("REQUEST_METHOD=" + this->_ctrlData[0]); // method in meta data
	this->_cgiEnv.push_back("REQUEST_URI=" + getHeader("Host") + this->_ctrlData[1]); // full path, meta data + host
	this->_cgiEnv.push_back("SERVER_PROTOCOL=" + this->_ctrlData[2]);
//////////////////////////////////////////////////////////////////
//	this->_cgiEnv.push_back("DOCUMENT_ROOT=" + /*TODO*/); // path where all cgi docs are
//	this->_cgiEnv.push_back("SCRIPT_NAME=" + /*TODO*/); // path relative to DOCUMENT_ROOT
//	this->_cgiEnv.push_back("SCRIPT_FILENAME=" + /*TODO*/); // full path
	//find in .conf
//////////////////////////////////////////////////////////////////
	if (getsockname(static_cast <int> (socket.ident), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
		throw(Error::getSockNameException());
	//inet_ntop(sockAddr.sin_family, &sockAddr.sin_addr, buffer, INET_ADDRSTRLEN);
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
		throw(Error::getNameInfoException());
	tmp = host;
	//tmp += " ";
	//tmp += service;
	this->_cgiEnv.push_back("REMOTE_ADDR=" + tmp); // IP address client
	this->_cgiEnv.push_back("REMOTE_HOST=" + tmp); // IP adress client again
	if (getsockname(*reinterpret_cast <int *> (this->_masterSocketInfo.masterSocket), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
		throw(Error::getSockNameException());
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
		throw(Error::getNameInfoException());
	tmp = host;
	this->_cgiEnv.push_back("SERVER_NAME=" + tmp); // server hostname or IP address
	ss.clear();
	ss << this->_masterSocketInfo.masterPort;
	ss >> tmp;
	this->_cgiEnv.push_back("SERVER_PORT=" + tmp); // port on host running
}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(const HttpResponse &instance)
{
	*this = instance;
}

HttpResponse &	HttpResponse::operator=(const HttpResponse &instance)
{
	if (this == &instance)
		return *this;
	for (int i = 0; i < 3; i++)
		this->_ctrlData[i] = instance._ctrlData[i];
	this->_body = instance._body;
	this->_headers = instance._headers;
	this->_cgiEnv = instance._cgiEnv;
	return *this;
}

std::string HttpResponse::getHeader(std::string const & key) const
{
	map_it it = _headers.find( key);
	if (it != _headers.end())
		return it->second;
	else
		return NaV;
}

//char **		HttpResponse::setEnv(struct kevent & socket)
//{
//	//cgiEnv.data() fait pareil ....
//	size_t	size = _cgiEnv.size();
//	size_t 	tot;
//	char **env = new char * [size];
//
//	if (!env)
//		return NULL;
//	for (size_t i = 0; i < size; i++)
//	{
//		env[i] = new char [_cgiEnv[i].size() + 1];
//		if (!env[i])
//		{
//			//free_tab(env, i);
//			return NULL;
//		}
//		tot = _cgiEnv[i].copy(env[i], _cgiEnv[i].size(), 0);
//		env[i][tot] = '\0';
//	}
//	return (env);
//}

bool	HttpResponse::processRequest(int masterSocket) const
{
	(void) masterSocket;
	//TODO:	 comparer contenu headers avec .conf si all ok, ex: si method de request sur uri allow sur par .conf, etc....

	return true; //1) checker le control data
}

std::string HttpResponse::methodGetHandler()
{

}

std::string HttpResponse::methodPostHandler()
{

}

std::string HttpResponse::methodDeleteHandler()
{

}

std::string HttpResponse::cgiHandler(std::string cgiPath)
{
	int 		fd[2];
	int			status;
	char		buffer[1024];
	std::string	response;

	pipe(fd);
	int pid = fork();
	if (!pid)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		//jamais arguments car tout dans env, donc file, argv are same)
		char *const args[2];
		args[0] = cgiPath.c_str();
		args[1] = NULL;
		if (execve(cgiPath.c_str(), args, this->_cgiEnv.data()) == -1)
			exit(-1);
		exit(0);
	}
	close (fd[0]);
	close (fd[1]);
	waitpid(pid, &status, 0);
	if (status == -1)
		return "error cgi";
	ssize_t size = read(fd[0], buffer, 1023);
	if (size == -1)
		return "errror cgi";
	while (size > 0)
	{
		buffer[size] = 0;
		response += buffer;
		size = read(fd[0], buffer, 1023);
		if (size == -1)
			return "errror cgi";
	}
	close (fd[0]);
	return response;
}

std::string HttpResponse::generateResponse(struct kevent & socket)
{
	//TODO: checker si cgi needed or not, si oui fork ses grand morts;
	std::string	response;
	size_t 		pos = this->_ctrlData[1].find(".cgi");

	(void) socket;
	if (this->_ctrlData[0] == "GET")
	{
		if (pos != std::string::npos)
		{
			if (pos + 4 != this->_ctrlData[1].size())
			{
				if ((pos = this->_ctrlData[1].find('?')) != std::string::npos)
				{
					this->_cgiEnv.push_back("QUERY_STRING=" + this->_ctrlData[1].substr(pos + 1, _ctrlData[1].size() - pos - 1));
					_ctrlData[1].erase(pos, _ctrlData[1].size() - pos);
				}
				if ((pos = this->_ctrlData[1].find(".cgi/")) != std::string::npos)
				{
					this->_cgiEnv.push_back("PATH_INFO=" + this->_ctrlData[1].substr(pos + 4, _ctrlData[1].size() - pos - 4));
					this->_ctrlData[1].erase(pos + 4, _ctrlData[1].size() - pos - 4);
				}
			}
		}
		//basic case
		{
			//TODO define le num de retour, les headers pour la reponse
		}
		//TODO define les headers a mettre dans reponses de base en fonction des headers de request
	}
	else if (this->_ctrlData[0] == "POST")
	{
		// utiliser CGI
	}
	else
	{
		// ? utiliser CGI
	}
	return response;
}
