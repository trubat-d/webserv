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
			return ("SocketException: socket failed");
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

	class FcntlException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("FcntlException: fcntl failed");
		}
	};

	class AcceptException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("AcceptException: accpet failed");
		}
	};

	class ReadException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("ReadException: read failed");
		}
	};

	class WriteException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("WriteException: write failed");
		}
	};

	class TcpException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("TcpException: Unexpected behavior");
		}
	};
};

#endif
