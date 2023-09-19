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
	this->_cgiEnv.push_back(Utils::stoa("AUTH_TYPE=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(Utils::stoa("CONTENT_LENGTH=" + getHeader("Content-Length")));
	this->_cgiEnv.push_back(Utils::stoa("CONTENT_TYPE=" + getHeader("Content-Type")));
	this->_cgiEnv.push_back(Utils::stoa("GATEWAY_INTERFACE=CGI/1.1"));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_USER_AGENT=" + getHeader("User-Agent")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_HOST=" + getHeader("Host")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_ACCEPT=" + getHeader("Accept")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_ACCEPT_CHARSET=" + getHeader("Accept-Charset")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_ACCEPT_LANGUAGE=" + getHeader("Accept-Language")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_CONNECTION=" + getHeader("Connection")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_REFERER=" + getHeader("Referer")));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_USER_AGENT=" + getHeader("User-Agent")));
	this->_cgiEnv.push_back(Utils::stoa("SERVER_SOFTWARE=WebserverDeSesGrandsMorts/4.20.69"));
	this->_cgiEnv.push_back(Utils::stoa("HTTP_COOKIE=" + getHeader("Cookie")));
	this->_cgiEnv.push_back(Utils::stoa("REMOTE_IDENT=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(Utils::stoa("REMOTE_USER=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(Utils::stoa("REQUEST_METHOD=" + this->_ctrlData[0])); // method in meta data
	this->_cgiEnv.push_back(Utils::stoa("REQUEST_URI=" + getHeader("Host") + this->_ctrlData[1])); // full path, meta data + host
	this->_cgiEnv.push_back(Utils::stoa("SERVER_PROTOCOL=" + this->_ctrlData[2]));
	this->_cgiEnv.push_back(Utils::stoa("DOCUMENT_ROOT=" + this->_config["root"][0])); // path where all cgi docs are
	this->_cgiEnv.push_back(Utils::stoa("SCRIPT_NAME=" + this->_ctrlData[1])); // path relative to DOCUMENT_ROOT
	this->_cgiEnv.push_back(Utils::stoa("SCRIPT_FILENAME=" + _config["root"][0] + this->_ctrlData[1])); // full path
	if (getsockname(static_cast <int> (socket.ident), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
		throw(Error::getSockNameException());
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
		throw(Error::getNameInfoException());
	tmp = host;
	this->_cgiEnv.push_back(Utils::stoa("REMOTE_ADDR=" + tmp)); // IP address client
	this->_cgiEnv.push_back(Utils::stoa("REMOTE_HOST=" + tmp)); // IP adress client again
	if (getsockname(*reinterpret_cast <int *> (this->_masterSocketInfo.masterSocket), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
		throw(Error::getSockNameException());
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
		throw(Error::getNameInfoException());
	tmp = host;
	this->_cgiEnv.push_back(Utils::stoa("SERVER_NAME=" + tmp)); // server hostname or IP address
	ss.clear();
	ss << this->_masterSocketInfo.masterPort;
	ss >> tmp;
	this->_cgiEnv.push_back(Utils::stoa("SERVER_PORT=" + tmp)); // port on host running
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
    this->_masterSocketInfo = instance._masterSocketInfo;
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

bool	HttpResponse::processRequest(Parser &config)
{
	//TODO:	 comparer contenu headers avec .conf si all ok, ex: si method de request sur uri allow sur par .conf, etc....
	//TODO: REMOVE THE TESTS
	std::cout << "Process request info"<< std::endl;
	std::cout << this->_masterSocketInfo.masterPort << std::endl;
	std::cout << getHeader("Host") << std::endl;
	std::cout << this->_ctrlData[1] << std::endl;
	//TODO: ----------------------------------------------------------------------------------------------------------------
	this->_config = new t_conf_map(config.getServerConfig(getHeader("Host"), std::to_string(this->_masterSocketInfo.masterPort), this->_ctrlData[1]));
	return true;
}

bool HttpResponse::validateBodySize(std::string &bodySize)
{
	if (bodySize.empty())
		return false;
	if(bodySize.find('M') != std::string::npos || bodySize.find('m') != std::string::npos)
	{
		bodySize.pop_back();
		if(bodySize.empty() ||
			bodySize.find_first_not_of("0123456789", 0) != std::string::npos)
			return false;
		return this->_body.size() < static_cast<unsigned long>(std::atol(bodySize.c_str())) * 1000000;
	}
	else if (bodySize.find('K') != std::string::npos || bodySize.find('k') != std::string ::npos)
	{
		bodySize.pop_back();
		if(bodySize.empty() ||
		   bodySize.find_first_not_of("0123456789", 0) != std::string::npos)
			return false;
		return this->_body.size() < static_cast<unsigned long>(std::atol(bodySize.c_str())) * 1000;
	}
	return this->_body.size() < static_cast<unsigned long>(std::atol(bodySize.c_str()));
}

std::string const HttpResponse::fullResponse(char *path, std::string const & body, std::pair<int, std::string> infos)
{
    std::string         response;
    struct stat         fileInfos = {};

    (void) stat(path, &fileInfos);
    response ="HTTP/1.1 " + Utils::itos(infos.first) + " " + infos.second + "\r\n";
    response +="Data: " + Utils::getTime(0) + "\r\n";
    response += "Content-Type: text/html; charset=UTF-8\r\n";
    response += "Content-Length: " + Utils::itos(static_cast<int>(fileInfos.st_size)) + "\r\n";
    response += "Last-Modified: " + Utils::getTime(fileInfos.st_mtime) + "\r\n";
    response += "Server: WebserverDeSesGrandsMorts/4.20.69\r\n";
    response += "Accept-Ranges: bytes\r\n";
    response += "Connection: " + getHeader("Connection") == "Keep-Alive" ? "Keep-Alive\r\n" : "close\r\n";
    if (infos.first == 405) // 405 Method not allowed
    {
        response += "Allow: ";
        for (vec_it it = this->_config["allow"].begin(); it != this->_config["allow"].end(); it++)
            response += *it + " ";
        response += "\r\n";
    }
    response += "\r\n";
    response += body + "\r\n";
    return response;
}

std::string const HttpResponse::methodGetHandler()
{
	size_t 		pos = this->_ctrlData[1].find(".cgi");

	//cig case
	if (pos != std::string::npos)
	{
		if (pos + 4 != this->_ctrlData[1].size())
		{
			if ((pos = this->_ctrlData[1].find('?')) != std::string::npos)
			{
				this->_cgiEnv.push_back(Utils::stoa("QUERY_STRING=" + this->_ctrlData[1].substr(pos + 1, _ctrlData[1].size() - pos - 1)));
				_ctrlData[1].erase(pos, _ctrlData[1].size() - pos);
			}
			if ((pos = this->_ctrlData[1].find(".cgi/")) != std::string::npos)
			{
				this->_cgiEnv.push_back(Utils::stoa("PATH_INFO=" + this->_ctrlData[1].substr(pos + 4, _ctrlData[1].size() - pos - 4)));
				this->_ctrlData[1].erase(pos + 4, _ctrlData[1].size() - pos - 4);
			}
			return this->cgiHandler();
		}
	}
	//basic case
    if (this->_ctrlData[1].back() == '/')
        this->_ctrlData[1] += "index.html";
    std::string	fullPath = this->_config["root"][0] + this->_ctrlData[1];
    fullPath.insert(fullPath.begin(), '.');
    int fd = open(fullPath.c_str(), O_RDONLY);
    if (fd == -1)
        return ("file not find");
    char buffer[1024];
    ssize_t size = read(fd, buffer, 1023);
    if (size == -1)
        return ("error read");
    std::string body;
    while (size > 0)
    {
        buffer[size] = 0;
        body += buffer;
        size = read(fd, buffer, 1023);
        if (size == -1)
            return ("error read");
    }
    return this->fullResponse(Utils::stoa(fullPath), body, std::make_pair<int, std::string>(0, "OK"));
}

std::string const HttpResponse::methodPostHandler()
{
	return "merde";
}

std::string const HttpResponse::methodDeleteHandler()
{
	return "merde";
}

std::string const HttpResponse::notCorrectMethodHandler()
{
    std::string         response;
    struct stat         fileInfos = {};

    response ="HTTP/1.1 405 Method not allowed\r\n";
    response +="Data: " + Utils::getTime(0) + "\r\n";
    response += "Server: WebserverDeSesGrandsMorts/4.20.69\r\n";
    response += "Connection: " + getHeader("Connection") == "Keep-Alive" ? "Keep-Alive\r\n" : "close\r\n";
    if (infos.first == 405) // 405 Method not allowed
    {
        response += "Allow: ";
        for (vec_it it = this->_config["allow"].begin(); it != this->_config["allow"].end(); it++)
            response += *it + " ";
        response += "\r\n";
    }
    response += "\r\n";
    return response;
}

std::string const HttpResponse::cgiHandler()
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
		std::string tmp("tmp");
		char * args[2] = {const_cast<char *>(tmp.c_str()), NULL};
		if (execve("TODO", args, reinterpret_cast<char *const *>(this->_cgiEnv.data())) == -1)
			exit(-1);
		exit(0);
	}
	close (fd[1]);
	waitpid(pid, &status, 0);
	if (status == -1)
		return "error cgi";
	ssize_t size = read(fd[0], buffer, 1023);
	if (size == -1)
		return "error cgi";
	while (size > 0)
	{
		buffer[size] = 0;
		response += buffer;
		size = read(fd[0], buffer, 1023);
		if (size == -1)
			return "error cgi";
	}
	close (fd[0]);
	return response;
}

std::string HttpResponse::generateResponse()
{
	std::string	response;

	if (this->_ctrlData[0] == "GET")
		response = this->methodGetHandler();
	else if (this->_ctrlData[0] == "POST")
		response = this->methodPostHandler();
	else if (this->_ctrlData[0] == "DELETE")
		response = this->methodDeleteHandler();
    else
        response = this->notCorrectMethodHandler();
	return response;
}
