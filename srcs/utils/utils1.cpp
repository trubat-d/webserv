#include "Utils.hpp"

char *Utils::concatToCharset(std::string &str1, std::string &str2)
{
	str1 += str2;
	return const_cast<char *>(str1.c_str());
}

char *      Utils::stoa(std::string const & str)
{
    return (const_cast<char *>(str.c_str()));
}

std::string Utils::getTime(time_t time)
{
    std::time_t now = std::time(NULL);
    std::tm *   ptm = std::gmtime(&now);
    if (time)
        ptm = std::gmtime(&time);
    char buffer[30] = {};
    (void) strftime(buffer, 29, "%a, %d %b %Y %T GMT", ptm);
    std::string tmp(buffer);
    return tmp;
}