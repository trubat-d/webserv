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


#endif
