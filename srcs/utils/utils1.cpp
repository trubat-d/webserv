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

std::string  Utils::fileToString(std::string & fullPath, std::pair<int, std::string> & infos)
{
    if (fullPath.empty())
        return basicHTML(infos);
    int fd = open(fullPath.c_str(), O_RDONLY);
    if (fd == -1)
        return basicHTML(infos);
    char buffer[1024];
    ssize_t size = read(fd, buffer, 1023);
    if (size == -1)
        return internalServerError(infos);
    std::string body;
    while (size > 0)
    {
        buffer[size] = 0;
        body += buffer;
        size = read(fd, buffer, 1023);
        if (size == -1)
            return internalServerError(infos);
    }
    if (close(fd) == -1)
        return internalServerError(infos);
    return body;
}

std::string Utils::internalServerError(std::pair<int, std::string> & infos)
{
    infos.first = 500;
    infos.second = "HTTP/1.1 500 Internal Server Error";
    return "";
}

std::string Utils::basicHTML(std::pair<int, std::string> const & infos)
{
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + Utils::itos<int>(infos.first) + "</title>\n</head>\n<body>\n<p>" + infos.second+ "</p>\n</body>\n</html>";
}
