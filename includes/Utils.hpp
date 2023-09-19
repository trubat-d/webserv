#ifndef WEB_GIT_UTILS_HPP
#define WEB_GIT_UTILS_HPP
#include "Includer.hpp"

class Utils
{
public:
	static char *concatToCharset(std::string &str1, std::string &str2);
	static std::vector<int> vecStoI(std::vector<std::string> input);
};


#endif