#include "Utils.hpp"

char *Utils::concatToCharset(std::string &str1, std::string &str2)
{
	str1 += str2;
	return const_cast<char *>(str1.c_str());
}

std::vector<int> Utils::vecStoI(std::vector<std::string> input)
{
	std::vector<int> result;
	for(size_t i = 0; i < input.size(); i++)
	{
		std::stringstream strm(input[i]);
		int x = 0;
		strm >> x;
		result.push_back(x);
	}
	return result;
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