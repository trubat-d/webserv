#include "Http.hpp"

HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(HttpRequest const & instance)
{
	this->_ctrlData = instance.getCtrlData();
	this->_headers = instance.getHeaders();
	this->_body = instance.getBody();
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
	this->_ctrlData = instance._ctrlData;
	this->_body = instance._body;
	this->_headers = instance._headers;
	return *this;
}

bool	HttpResponse::processRequest(int masterSocket) const
{
	//TODO:	 comparer contenu headers et .conf si all ok, ex: si method de request sur uri allow sur par .conf, etc....
	return true;//1) checker le control data
}

std::string HttpResponse::generateResponse()
{
	//TODO: checker si cgi needed or not, si oui fork ses grand morts;
	return "hello";
}