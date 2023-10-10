#ifndef HTTP_HPP
#define HTTP_HPP

#include "Includer.hpp"

#define NaV "NoValueFindInThisHeader"

class Http {

    typedef std::vector<std::string>::iterator 	                vec_it;
    typedef std::map<std::string, std::string>::const_iterator  map_it;

public:
	Http();
	Http(std::string & request, struct kevent & socket);
	Http(const Http & instance);
	~Http();

	Http &	operator=(Http const & instance);

    //	typedef	std::string::iterator str_it;

//REQUEST
///////////////////////////////////////////////////////////////////////
	std::string const &	getRequest() const;
	std::string const & getBody() const;
	std::string *       getCtrlData();
	std::map<std::string, std::string> const & getHeaders() const;
	void				setControlData(std::string const & data);
	void				setBody(std::string const & body);

	bool	parseRequest();

	void	showData();

///////////////////////////////////////////////////////////////////////


//RESPONSE
///////////////////////////////////////////////////////////////////////
    std::string getHeader(std::string const & key) const;
    //char 		**setEnv(struct kevent & socket);

    std::pair<int, std::string>	processRequest(Parser &config);
    bool						validateBodySize(std::string &bodySize);
    std::pair<int, std::string> setCGIEnv(struct kevent & socket);
    std::string					generateResponse(std::pair<int, std::string> res);

    std::string const methodGetHandler();
    std::string const notCorrectMethodHandler();
    std::string const fullResponse(std::string const & path, std::string const & body, std::pair<int, std::string> & infos);
    std::string cgiHandler();
    std::string const getMimeType(std::string path);

///////////////////////////////////////////////////////////////////////

//ATTRIBUTE
///////////////////////////////////////////////////////////////////////
private:

	std::string						    _request;
    std::string                         _response;

    std::string 						_ctrlData[3]; //[0] = GET ; [1] = /path ; [2] = HTTP/1.1
	std::map<std::string, std::string>	_headers;
	std::string 						_body;

    t_conf_map		                    _config;
    std::vector<std::string>		    _cgiEnv;
    uDada				                _masterSocketInfo;
};
///////////////////////////////////////////////////////////////////////

#endif