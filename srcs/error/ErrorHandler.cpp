#include "ErrorHandler.hpp"

const char *Error::ConfigParsingException::what() const throw()
{
		return _message.c_str();
}

Error::ConfigParsingException::ConfigParsingException(std::string &message)
{
	this->_message = "Error while parsing token -> " + message;
}