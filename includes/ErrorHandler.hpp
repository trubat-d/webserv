#ifndef WEB_GIT_ERRORHANDLER_HPP
#define WEB_GIT_ERRORHANDLER_HPP

#include "Includer.hpp"

class Error
{
public:
	class MemoryAllocationException : public std::exception {
		public:
			virtual const char * what () const throw () { return "Cannot allocate Memory !";}
	};

	class ConfigParsingException : public std::exception {
	private:
		std::string _message;
	public:
		explicit ConfigParsingException(std::string& message);
		~ConfigParsingException() throw() {};
		const char * what () const throw();
	};

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
	class getSockNameException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("getSockName: Unexpected behavior");
		}
	};
	class getNameInfoException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("getnameinfo: Unexpected behavior");
		}
	};
	class AccessException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("access: Unexpected behavior");
		}
	};
	class PipeException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("pipe: Unexpected behavior");
		}
	};
	class ForkException: public std::exception
	{
	public:
		virtual const char* what() const throw()
		{
			return ("fork: Unexpected behavior");
		}
	};
};

#endif
