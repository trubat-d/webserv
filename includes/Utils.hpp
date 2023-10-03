#ifndef WEB_GIT_UTILS_HPP
#define WEB_GIT_UTILS_HPP

#include "Includer.hpp"

class Utils
{
public:
	static std::vector<int> vecStoI(std::vector<std::string> input);
	static char *       concatToCharset(std::string &str1, std::string &str2);
    static char *       stoa(std::string const & str);
    static std::string  getTime(time_t time);

	static void fillMime();
	static std::map<std::string, std::string> mimeTypes;
    static std::string        fileToString(std::string & fullPath, std::pair<int, std::string> & infos);
    static std::string         internalServerError(std::pair<int, std::string> & infos);
    static std::string          basicHTML(std::pair<int, std::string> const & infos);
    template<typename T>
    static std::string itos(T value)
    {
        std::stringstream ss;

        ss << value;
        return ss.str();
    }
};

#endif