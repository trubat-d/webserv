#ifndef WEB_GIT_INCLUDER_HPP
#define WEB_GIT_INCLUDER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <sstream>
#include <exception>
#include <algorithm>

#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <csignal>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>


typedef struct uData {
    //int     masterSocket;
    //int     masterPort;
    std::string   masterPort;
    std::string   client_host;
    std::string   client_addr;
} uDada;

#include "Utils.hpp"
#include "ErrorHandler.hpp"
#include "Parser.hpp"
#include "Socket.hpp"
#include "Http.hpp"
#include "CGI.hpp"

enum isWhat {
    none = -1,
    file = 0,
    dir = 1,
};

class Utils;
class Error;
class Parser;


#endif
