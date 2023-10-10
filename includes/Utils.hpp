#ifndef WEB_GIT_UTILS_HPP
#define WEB_GIT_UTILS_HPP

#include "Includer.hpp"

class Utils {
public:
    static std::vector<int> vecStoI(std::vector<std::string> input);

    static char *concatToCharset(std::string &str1, std::string &str2);

    static std::string getTime(time_t time);

    static void fillMime();

    static std::map<std::string, std::string> mimeTypes;

    static std::string fileToString(std::string &fullPath, std::pair<int, std::string> &infos);

    static std::string internalServerError(std::pair<int, std::string> &infos);

    static std::string basicHTML(std::pair<int, std::string> const &infos);

    static int         removeSocket(int kq, struct kevent * socket, int nfilter, int * filter, int flags, std::map<int, std::string>& receive, std::map<int, std::string>& send);

    static bool    timer(const std::clock_t start);

    static std::string   basicError(std::pair<int, std::string> const & infos);

    template<typename T>
    static std::string itos(T value)
    {
        std::stringstream ss;

        ss << value;
        return ss.str();
    }

    template<typename T, typename U>
    static void eraseMap(std::map<T, U> & map, T key)
    {
        typename std::map<T, std::string>::iterator it;
        it = map.find(key);
        if (it != map.end())
            map.erase(it);
    }
};

#endif