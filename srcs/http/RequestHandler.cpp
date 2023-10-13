#include "Http.hpp"

Http::Http() {} /*_request((std::string &) "")*/

Http::Http(std::string & request, struct kevent & socket): _request(request)
{
    if (socket.udata)
        this->_masterSocketInfo = * reinterpret_cast<uDada *>(socket.udata);
    else
        bzero(&this->_masterSocketInfo, sizeof(uDada));
}

Http::Http(const Http & instance)
{
    *this = instance;
}

Http::~Http() {}

Http &	Http::operator=(const Http &instance)
{
    if (this == &instance)
        return *this;
    this->_request = instance._request;
    this->_response = instance._response;
    for (int i = 0; i < 3; i++)
        this->_ctrlData[i] = instance._ctrlData[i];
    this->_headers = instance._headers;
    this->_body = instance._body;
    this->_cgiEnv = instance._cgiEnv;
    this->_config = instance._config;
    this->_masterSocketInfo = instance._masterSocketInfo;
    return *this;
}

///////////////////////////////////GETTERS/////////////////////////////////////////////

std::string const &	Http::getRequest() const
{
	return this->_request;
}

std::string *	Http::getCtrlData()
{
	return this->_ctrlData;
}

std::string const &	Http::getBody() const
{
	return this->_body;
}

std::map<std::string, std::string> const & Http::getHeaders() const
{
	return this->_headers;
}

///////////////////////////////////SETTERS//////////////////////////////////////////////

void	Http::setControlData(std::string const & data)
{
    std::stringstream ss;
    ss << data;
    for (int i = 0; i < 3; i++)
        ss >> this->_ctrlData[i];
}

void	Http::setBody(std::string const & body)
{
	this->_body = body;
}

/////////////////////////////////PARSING//////////////////////////////////////////////////

bool	Http::parseRequest()
{
	std::string pre;
	std::string::size_type pos;

    //CHECK ERREUR
    if (this->getRequest().empty())
		return false;
	if ((pos = this->getRequest().find("\r\n\r\n")) == std::string::npos)
		return false;

    //RECUPERE LE BODY
	if (pos + 3 != this->getRequest().size())
	{
		this->setBody(this->getRequest().substr(pos + 4, this->getRequest().size() - pos - 4));
		this->_request.erase(pos + 4, this->getRequest().size() - pos - 3);
	}
    //RECUPERE LA CONTROL DATA
	pos = this->getRequest().find("\r\n");
	this->setControlData(this->getRequest().substr(0, pos));
	this->_request.erase(0, pos + 2);

    //RECUPERE LES HEADERS
	while ((pos = this->getRequest().find("\r\n", 0)) != std::string::npos)
	{
		if (this->getRequest() == "\r\n\r\n" || this->getRequest() == "\r\n")
			return true;
		pre = this->getRequest().substr(0, pos);
		this->_request.erase(0, pos + 2);
		if ((pos = pre.find(": ")) != std::string::npos)
			this->_headers[pre.substr(0, pos)] += (this->_headers[pre.substr(0, pos)].empty() ? "" : ",") \
			+ pre.substr(pos + 2, pre.size() - pos - 1);
		else
		{
			this->_headers.clear();
            for (int i = 0; i < 3; i++)
			    this->_ctrlData[i].clear();
			this->_body.clear();
			return false;
		}
	}
	return true;
}

//void	Http::showData()
//{
//	std::cout << "test ctrl data: " << "[" << this->_ctrlData << "]" << std::endl;
//	typedef std::map<std::string, std::string>::iterator it;
//	it ite;
//
//	std::cout << "test map" << std::endl;
//	for (ite = this->_headers.begin(); ite != this->_headers.end(); ite++)
//		std::cout << "[" << ite->first << "]" << " " << "[" << ite->second << "]" << std::endl;
//
//	std::cout << "test body :" << "[" << this->_body << "]" << std::endl;
//}