#ifndef HTTP_HPP
#define HTTP_HPP

#include "Includer.hpp"

#define NaV "NoValueFindInThisHeader"

class HttpRequest {

public:

	HttpRequest();
	HttpRequest(std::string const & request);
	HttpRequest(HttpRequest const & instance);
	~HttpRequest();

	HttpRequest &	operator=(HttpRequest const & instance);


	std::string const &	getRequest() const;
	std::string const & getBody() const;
	std::string const & getCtrlData() const;
	std::map<std::string, std::string> const & getHeaders() const;
	void				setControlData(std::string const & data);
	void				setBody(std::string const & body);

	bool	parseRequest();

	void	showData();

//	typedef	std::string::iterator str_it;

private:

	std::string 						_request;

	std::string 						_ctrlData;
	std::map<std::string, std::string>	_headers;
	std::string 						_body;
};

///////////////////////////////////////////////////////////////////////

class HttpResponse {

public:

	typedef std::map<std::string, std::string>::const_iterator	map_it;

	HttpResponse();
	HttpResponse(HttpRequest const & instance, struct kevent & socket);
	HttpResponse(HttpResponse const & instance);
	~HttpResponse();

	HttpResponse & operator=(HttpResponse const & instance);

	std::string getHeader(std::string const & key) const;
	//char 		**setEnv(struct kevent & socket);

	std::string generateResponse(struct kevent & socket);
	bool		processRequest(int masterSocket) const;

private:

	std::string 										_ctrlData[3]; //[0] = GET ; [1] = /path ; [2] = HTTP 1.1
	std::map<std::string, std::string>					_headers;
	std::string											_body;
	std::map< std::string, std::vector<std::string> >	_config;
	std::vector<std::string>							_cgiEnv;
};

#endif