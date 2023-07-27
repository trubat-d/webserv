<<<<<<< HEAD
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
};

=======
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
>>>>>>> f851995d0a32b946f1f84a069868c26b441a3137

#endif
