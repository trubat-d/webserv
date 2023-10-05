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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

typedef struct uData {
    int     masterSocket;
    int     masterPort;
    bool    connection;
} uDada;

#include "Utils.hpp"
#include "ErrorHandler.hpp"
#include "Parser.hpp"
#include "Socket.hpp"
#include "Http.hpp"
#include "CGI.hpp"

class Utils;
class Error;
class Parser;


#endif
