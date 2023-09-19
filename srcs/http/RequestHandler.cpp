#include "Http.hpp"

HttpRequest::HttpRequest() {}
HttpRequest::HttpRequest(const std::string & request): _request(request) {}
HttpRequest::HttpRequest(const HttpRequest & instance): _headers(instance._headers) {}
HttpRequest::~HttpRequest() {}

HttpRequest &	HttpRequest::operator=(const HttpRequest &instance)
{
	if (this != &instance)
		this->_headers = instance._headers;
	return *this;
}

///////////////////////////////////GETTERS/////////////////////////////////////////////

std::string const &	HttpRequest::getRequest() const
{
	return this->_request;
}

std::string const &	HttpRequest::getCtrlData() const
{
	return this->_ctrlData;
}

std::string const &	HttpRequest::getBody() const
{
	return this->_body;
}

std::map<std::string, std::string> const & HttpRequest::getHeaders() const
{
	return this->_headers;
}

///////////////////////////////////SETTERS//////////////////////////////////////////////

void	HttpRequest::setControlData(std::string const & data)
{
	this->_ctrlData = data;
}

void	HttpRequest::setBody(std::string const & body)
{
	this->_body = body;
}

/////////////////////////////////PARSING//////////////////////////////////////////////////

bool	HttpRequest::parseRequest()
{
	std::string pre;
	std::string::size_type pos;

    std::cout << "request content: " << std::endl;
	std::cout << this->getRequest() << std::endl;
    if (this->getRequest().empty())
		return false;
	if ((pos = this->getRequest().find("\r\n\r\n")) == std::string::npos)
		return false;
	if (pos + 3 != this->getRequest().size())
	{
		this->setBody(this->getRequest().substr(pos + 4, this->getRequest().size() - pos - 4));
		this->_request.erase(pos + 4, this->getRequest().size() - pos - 3);
	}
	pos = this->getRequest().find("\r\n");
	this->setControlData(this->getRequest().substr(0, pos));
	this->_request.erase(0, pos + 2);
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
			this->_ctrlData.clear();
			this->_body.clear();
			return false;
		}
	}
	return true;
}

//void	HttpRequest::showData()
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