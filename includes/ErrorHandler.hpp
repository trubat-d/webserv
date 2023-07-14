#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#include <exception>

class Error {

public:

	class SocketException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("SocketException: cannot create socket");
		}
	};

	class BindException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("BindException: bind failed");
		}
	};

	class KeventException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("KeventException: kevent failed");
		}
	};

};

#endif
