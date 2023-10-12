#include "Http.hpp"

std::pair<int, std::string> Http::setCGIEnv(struct kevent & socket)
{
	std::string	tmp;
	std::stringstream ss;
	struct sockaddr_in	sockAddr = {};
	socklen_t len = sizeof(sockAddr);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	this->_cgiEnv.push_back(("AUTH_TYPE=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(("CONTENT_LENGTH=" + getHeader("Content-Length")));
	this->_cgiEnv.push_back(("CONTENT_TYPE=" + getHeader("Content-Type")));
	this->_cgiEnv.push_back("REDIRECT_STATUS=CGI");
	this->_cgiEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_cgiEnv.push_back(("HTTP_USER_AGENT=" + getHeader("User-Agent")));
	this->_cgiEnv.push_back(("HTTP_HOST=" + getHeader("Host")));
	this->_cgiEnv.push_back(("HTTP_ACCEPT=" + getHeader("Accept")));
	this->_cgiEnv.push_back(("HTTP_ACCEPT_CHARSET=" + getHeader("Accept-Charset")));
	this->_cgiEnv.push_back(("HTTP_ACCEPT_LANGUAGE=" + getHeader("Accept-Language")));
	this->_cgiEnv.push_back(("HTTP_CONNECTION=" + getHeader("Connection")));
	this->_cgiEnv.push_back(("HTTP_REFERER=" + getHeader("Referer")));
	this->_cgiEnv.push_back(("HTTP_USER_AGENT=" + getHeader("User-Agent")));
	this->_cgiEnv.push_back("SERVER_SOFTWARE=WebserverDeSesGrandsMorts/4.20.69");
	this->_cgiEnv.push_back(("HTTP_COOKIE=" + getHeader("Cookie")));
	this->_cgiEnv.push_back(("REMOTE_IDENT=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(("REMOTE_USER=" + getHeader("Authorization")));
	this->_cgiEnv.push_back(("REQUEST_METHOD=" + this->_ctrlData[0])); // method in meta data
	this->_cgiEnv.push_back(("REQUEST_URI=" + getHeader("Host") + this->_ctrlData[1])); // full path, meta data + host
	this->_cgiEnv.push_back(("SERVER_PROTOCOL=" + this->_ctrlData[2]));
	this->_cgiEnv.push_back(("DOCUMENT_ROOT=" + this->_config["root"][0])); // path where all cgi docs are
	this->_cgiEnv.push_back(("SCRIPT_NAME=" + this->_ctrlData[1])); // path relative to DOCUMENT_ROOT
	this->_cgiEnv.push_back(("SCRIPT_FILENAME=" + this->_config["root"][0] + this->_ctrlData[1])); // full path
	if (getsockname(static_cast <int> (socket.ident), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
        return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
        return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
	tmp = host;
	this->_cgiEnv.push_back(("REMOTE_ADDR=" + tmp)); // IP address client
	this->_cgiEnv.push_back(("REMOTE_HOST=" + tmp)); // IP adress client again
	if (getsockname(this->_masterSocketInfo.masterSocket, reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
        return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
	if (getnameinfo(reinterpret_cast <struct sockaddr *> (&sockAddr), sizeof(sockAddr), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV))
        return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
	tmp = host;
	this->_cgiEnv.push_back(("SERVER_NAME=" + tmp)); // server hostname or IP address
	ss.clear();
	ss << this->_masterSocketInfo.masterPort;
	ss >> tmp;
	this->_cgiEnv.push_back(("SERVER_PORT=" + tmp)); // port on host running
	tmp = getHeader("X-Filename");
	if(tmp != NaV)
		this->_cgiEnv.push_back("HTTP_X_FILENAME=" + tmp);
    return std::pair<int, std::string>(200, "HTTP/1.1 200 OK\r\n");
}

std::string Http::getHeader(std::string const & key) const
{
	map_it it = _headers.find( key);
	if (it != _headers.end())
		return it->second;
	else
		return NaV;
}

std::pair<int, std::string>	Http::processRequest(Parser &config)
{
	this->_config = config.getServerConfig(getHeader("Host"), std::to_string(this->_masterSocketInfo.masterPort), this->_ctrlData[1]);
    if(this->_config.find("deny") != this->_config.end() && !this->_config.at("deny").empty())
    {
        if(std::find(this->_config.at("deny").begin(), this->_config.at("deny").end(),this->_ctrlData[0]) != this->_config.at("deny").end())
            return std::pair<int, std::string>(405, "HTTP/1.1 405 Method Not Allowed\r\n");
    }
	if(this->_ctrlData[1].size() > static_cast<size_t>(std::atol(this->_config.at("uri_max_size")[0].c_str())))
		return std::pair<int, std::string>(414, "HTTP/1.1 414 URI Too Long\r\n");
	if(this->_ctrlData[2] != "HTTP/1.1")
		return std::pair<int, std::string>(505, "HTTP/1.1 505 HTTP Version Not Supported\r\n");
	if(this->_ctrlData[0] == "POST")
	{
		if(getHeader("Content-Length") != NaV && this->_body.empty())
			return std::pair<int, std::string>(100, "HTTP/1.1 100 Continue\r\n");
		else if(getHeader("Content-Length") == NaV && !this->_body.empty())
			return std::pair<int, std::string>(411, "HTTP/1.1 411 Length Required\r\n");
	}
	if(this->_ctrlData[0] != "DELETE" && this->_ctrlData[0] != "POST" && this->_ctrlData[0] != "GET")
		return std::pair<int, std::string>(501, "HTTP/1.1 501 Not Implemented\r\n");
//  ROOT IS MANDATORY0
    if(this->_config.find("root") == this->_config.end())
        return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
    else
    {
        char dir[256] = {};
        if (!getcwd(dir, 256) || this->_config.at("root").size() != 1)
            return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
		std::string temproot(this->_config.at("root")[0]);
		if(!this->_config.at("root")[0].empty() && !this->_config.at("root")[0].compare(0,2,"./"))
			temproot.erase(0,1);
        char * temp = strcat(dir, temproot.c_str());
        struct stat statbuf = {};
        stat(temp, &statbuf);
        if(!S_ISDIR(statbuf.st_mode))
            return std::pair<int, std::string>(500, "HTTP/1.1 500 Internal Server Error\r\n");
    }

//  check the size of the body size versus the allowed size
    if(this->_config.find("client_max_body_size") != this->_config.end() && this->_config.at("client_max_body_size").size() == 1)
    {
        if(!validateBodySize(this->_config.at("client_max_body_size")[0]))
            return std::pair<int, std::string>(413, "HTTP/1.1 413 Payload Too Large\r\n");
    }
	return std::pair<int, std::string>(200, "HTTP/1.1 200 OK\r\n");
}

bool Http::validateBodySize(std::string &bodySize)
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
		return std::atol(getHeader("Content-Length").c_str()) < std::atol(bodySize.c_str()) * 1000;
	}
	return std::atol(getHeader("Content-Length").c_str()) < std::atol(bodySize.c_str());
}

std::string const Http::fullResponse(std::string const & path, std::string const & body, std::pair<int, std::string> & infos)
{
    std::string         response;
    struct stat         fileInfos = {};

    if (path.empty())
        return Utils::basicError(infos);
    (void) stat(path.c_str(), &fileInfos);
	response += infos.second;
    response += "Date: " + Utils::getTime(0) + "\r\n";
    response += "Content-Type: "  + getMimeType(path) + "\r\n";
    response += "Content-Length: " + Utils::itos(body.size()) + "\r\n";
    response += "Last-Modified: " + Utils::getTime(fileInfos.st_mtime) + "\r\n";
    response += "Server: WebserverDeSesGrandsMorts/4.20.69\r\n";
    response += "Accept-Ranges: bytes\r\n";
    response += "Connection: ";
    response += getHeader("Connection") == "keep-alive" ? "keep-alive\r\n" : "close\r\n";
    if (infos.first == 405) // 405 Method not allowed
    {
        response += "Allow: ";
        for (vec_it it = this->_config["allow"].begin(); it != this->_config["allow"].end(); it++)
            response += *it + " ";
        response += "\r\n";
    }
    response += "\r\n";
    response += body + "\r\n";
//	std::cout << response << std::endl;
    return response;
}

std::string const Http::methodGetHandler()
{
    std::pair<int, std::string> status(200, "HTTP/1.1 200 OK\r\n");
	size_t 		pos = this->_ctrlData[1].find(".cgi");

	//cgi case
	if (pos != std::string::npos)
	{
		if (pos + 4 != this->_ctrlData[1].size())
		{
			if ((pos = this->_ctrlData[1].find('?')) != std::string::npos)
			{
				this->_cgiEnv.push_back(("QUERY_STRING=" + this->_ctrlData[1].substr(pos + 1, _ctrlData[1].size() - pos - 1)));
				_ctrlData[1].erase(pos, _ctrlData[1].size() - pos);
			}
			if ((pos = this->_ctrlData[1].find(".cgi/")) != std::string::npos)
			{
				this->_cgiEnv.push_back(("PAT H_INFO=" + this->_ctrlData[1].substr(pos + 4, _ctrlData[1].size() - pos - 4)));
				this->_ctrlData[1].erase(pos + 4, _ctrlData[1].size() - pos - 4);
			}
		}
		return this->cgiHandler();
	}
	//basic case
    std::string	fullPath = this->_config["root"][0] + this->_ctrlData[1];
    if (fullPath.back() == '/')
	{
        //TODO : FIND GOOD INDEX : this->_config.find("index")
        if (Utils::canAccessfile(this->_ctrlData[1] + "index.html"))
            this->_ctrlData[1] += "index.html";
		else if(this->_config.find("autoindex") != this->_config.end() && this->_config.at("autoindex")[0] == "on")
		{
            DIR * dir = opendir(fullPath.c_str());
            if (!dir) //TODO: changer type error
                return generateResponse(std::pair<int, std::string> (404, "HTTP/1.1 404 File Not Found\r\n"));
            return this->generateAutoIndex(dir, this->_ctrlData[1]);
		}
        else //TODO: changer type error
            return generateResponse(std::pair<int, std::string> (404, "HTTP/1.1 404 File Not Found\r\n"));
	}
    int fd = open(fullPath.c_str(), O_RDONLY);
    if (fd != -1)
    {
        close (fd);
        return this->fullResponse(fullPath.c_str(), Utils::fileToString(fullPath, status), status);
    }
    else
        return generateResponse(std::pair<int, std::string> (404, "HTTP/1.1 404 File Not Found\r\n"));
}



std::string const Http::getMimeType(std::string path)
{
	std::string temp_path;
	if(path.length() >= 4 && path.substr(path.length()-4, 4) == ".cgi")
	{
		temp_path = path.substr(0, path.length()-4);
	}
	else
		temp_path = path;
    if(temp_path.find('.') == std::string::npos)
        return "";
	std::string tmp_ext = std::string(temp_path.begin() + temp_path.find_last_of(".", temp_path.size()), temp_path.end());
	if(Utils::mimeTypes.find(tmp_ext) != Utils::mimeTypes.end())
	{
    	std::string const temp = Utils::mimeTypes.at(tmp_ext);
    	return temp;
	}
	//TODO GENERATE ERROR
	std::cerr << "Mime type doesn't appear to be supported" << std::endl;
	return "";
}

//std::string const Http::notCorrectMethodHandler()
//{
//    std::string         response;
//    //struct stat         fileInfos = {};
//
//    response ="HTTP/1.1 405 Method not allowed\r\n";
//    response +="Data: " + Utils::getTime(0) + "\r\n";
//    response += "Server: WebserverDeSesGrandsMorts/4.20.69\r\n";
//    response += "Connection: ";
//    response += getHeader("Connection") == "Keep-Alive" ? "Keep-Alive\r\n" : "close\r\n";
//    response += "Allow: ";
//    for (vec_it it = this->_config["allow"].begin(); it != this->_config["allow"].end(); it++)
//        response += *it + " ";
//    response += "\r\n";
//    response += "\r\n";
//    return response;
//}

std::string Http::generateAutoIndex(DIR * dir, std::string const & path) const
{
    struct dirent *ent;
    std::vector<std::string> filesName;

    while ((ent = readdir(dir)) != NULL)
        filesName.push_back(ent->d_name);
    closedir (dir);
    std::string tmp;
    //TODO rajouter headers
    tmp += "<!DOCTYPE html>\n<html>\n<body>\n<h1>Auto-Index</h1>\n<p>";
    for (std::vector<std::string>::iterator it = filesName.begin(); it != filesName.end(); it++)
    {
        tmp += "<a href=\"http://" + this->_masterSocketInfo.host + ":" + Utils::itos(this->_masterSocketInfo.masterPort) + "/" + path + *it + ">";
        tmp += *it + "</a></p>\n</body>\n</html>";
    }
    tmp += "</body>\n</html>\n";
    return tmp;
}

std::string Http::cgiHandler()
{
	int 		fd[2];
	int			status;
	char		buffer[1024];
	std::string	response;
	int bodyPipe[2];


	std::cout << "ENTERED CGI HANDLER" << std::endl;
	pipe(bodyPipe);
	pipe(fd);
	int pid = fork();
	if (!pid)
	{
		close(fd[0]);
		close(bodyPipe[1]);
		dup2(fd[1], STDOUT_FILENO);
		dup2(bodyPipe[0], STDIN_FILENO);
		close(fd[1]);
		close(bodyPipe[0]);
		size_t envSize = this->_cgiEnv.size();
		char * envi[envSize + 1];
		for(size_t i = 0; i < envSize; i++)
			envi[i] = const_cast<char *>(this->_cgiEnv[i].c_str());
		envi[envSize] = nullptr;
		std::string filePath = this->_config["root"][0].substr(0, this->_config["root"][0].size()-1) + this->_ctrlData[1];

        /////////////////////////////////////////////////TODO: CHECK JOB
        std::string script2;
        size_t merde;
        std::string bouh = ".php.cgi";
        merde = this->_ctrlData[1].find(bouh);
        if (merde == std::string::npos)
        {
            bouh = ".py.";
            merde = this->_ctrlData[1].find(".py.cgi");
        }
        if (merde == std::string::npos)
            exit (-1);
        bouh.erase(0, 1);
        for (int i = 0; i < 4; i++)
            bouh.pop_back();
        std::vector<std::string>::iterator where = std::find(this->_config.at("cgi").begin(), this->_config.at("cgi").end(), bouh);
        if (where != this->_config.at("cgi").end())
            script2 = *(where+1);
        else
            exit(-1);
        /////////////////////////////////////////////////

		//path php cgi /System/Volumes/Data/mnt/sgoinfre/php-cgi
        //path python: python3
        char * args[3] = { const_cast<char*>(script2.c_str()), const_cast<char *>(filePath.c_str()), nullptr};
		if (execve(const_cast<char *>(script2.c_str()), args, envi) == -1)
		{
			std::cerr << "exited on execve with filepath = " << args[0] << " + " << args[1] << std::endl;
			exit(-1);
		}
		exit(0);
	}
	if (!this->_body.empty())
	{
		std::cerr << "Write START \n" << std::endl;
		if (this->_body.size() > 65000)
		{
			std::string tmp(this->_body);
			int merde;
			while (!tmp.empty() && (merde = write(bodyPipe[1], tmp.c_str(), tmp.size() > 65000 ? 65000 : tmp.size())) > 0)
			{
				if (merde == -1)
					return "502 Bad Gateway\r\n";
				tmp.erase(0, tmp.size() > 65000 ? 65000 : tmp.size());
			}
		}
		else
		{
			if (write(bodyPipe[1], this->_body.c_str(), this->_body.size()) == -1)
				return "502 Bad Gateway\r\n";
		}
		std::cerr << "Write END \n" << std::endl;
	}
	close (fd[1]);
	close(bodyPipe[0]);
	close(bodyPipe[1]);
    const std::clock_t start = std::clock();
    while (waitpid(pid, &status, WNOHANG) != pid)
    {
        if (Utils::timer(start))
            kill(pid, SIGINT);
    }
	if (status)
		return "502 Bad Gateway\r\n";
	ssize_t size = read(fd[0], buffer, 1023);
	std::string temp_s(buffer, size);
	response += temp_s;
	if (size == -1)
		return "502 Bad Gateway\r\n";
	while (size == 1023)
	{
		buffer[size] = 0;
		std::string temp(buffer, size);
		response += temp;
		size = read(fd[0], buffer, 1023);
		if (size == -1)
			return "502 Bad Gateway\r\n";
	}
	close (fd[0]);
	return "HTTP/1.1 200 OK\r\n" + response;
}

std::string Http::generateResponse(std::pair<int, std::string> res)
{
    std::string path;

	if (res.first == 200)
	{
        if (this->_ctrlData[0] == "GET")
            return this->methodGetHandler();
        else if (this->_ctrlData[0] == "POST" || this->_ctrlData[0] == "DELETE")
            return this->cgiHandler();
    }
    else
    {
        if(!this->_config.empty())
        {
            std::string translate = Utils::itos(res.first);
            if(this->_config.find("error_page") != this->_config.end())
            {
                std::vector<std::string>::iterator where = std::find(this->_config.at("error_page").begin(), this->_config.at("error_page").end(),translate);
                if (where != this->_config.at("error_page").end())
                    path = *(where+1);
            }
        }
        return this->fullResponse(path, Utils::fileToString(path, res), res);
    }
    return "";
}

