#ifndef WEB_GIT_UTILS_HPP
#define WEB_GIT_UTILS_HPP

#include "Includer.hpp"

class Utils
{
public:
	static char *       concatToCharset(std::string &str1, std::string &str2);
    static char *       stoa(std::string const & str);
    static std::string  getTime(time_t time);

    template<typename T>
    static std::string itos(T value)
    {
        std::stringstream ss;

        ss << value;
        return ss.str();
    }
};

#endif