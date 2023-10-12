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

std::map<std::string, std::string> Utils::mimeTypes;

void Utils::fillMime()
{
	mimeTypes[".aac"] = "audio/aac";
	mimeTypes[".abw"] = "application/x-abiword";
	mimeTypes[".arc"] = "application/x-freearc";
	mimeTypes[".avif"] = "image/avif";
	mimeTypes[".avi"] = "video/x-msvideo";
	mimeTypes[".azw"] = "application/vnd.amazon.ebook";
	mimeTypes[".bin"] = "application/octet-stream";
	mimeTypes[".bmp"] = "image/bmp";
	mimeTypes[".bz"] = "application/x-bzip";
	mimeTypes[".bz2"] = "application/x-bzip2";
	mimeTypes[".cda"] = "application/x-cdf";
	mimeTypes[".csh"] = "application/x-csh";
	mimeTypes[".css"] = "text/css";
	mimeTypes[".csv"] = "text/csv";
	mimeTypes[".doc"] = "application/msword";
	mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mimeTypes[".eot"] = "application/vnd.ms-fontobject";
	mimeTypes[".epub"] = "application/epub+zip";
	mimeTypes[".gz"] = "application/gzip";
	mimeTypes[".gif"] = "image/gif";
	mimeTypes[".htm"] = "text/html";
	mimeTypes[".html"] = "text/html";
	mimeTypes[".ico"] = "image/vnd.microsoft.icon";
	mimeTypes[".ics"] = "text/calendar";
	mimeTypes[".jar"] = "application/java-archive";
	mimeTypes[".jpg"] = "image/jpeg";
	mimeTypes[".jpeg"] = "image/jpeg";
	mimeTypes[".js"] = "text/javascript";
	mimeTypes[".json"] = "application/json";
	mimeTypes[".jsonld"] = "application/ld+json";
	mimeTypes[".mid"] = "audio/midi, audio/x-midi";
	mimeTypes[".midi"] = "audio/midi, audio/x-midi";
	mimeTypes[".mjs"] = "text/javascript";
	mimeTypes[".mp3"] = "audio/mpeg";
	mimeTypes[".mp4"] = "video/mp4";
	mimeTypes[".mpeg"] = "video/mpeg";
	mimeTypes[".mpkg"] = "application/vnd.apple.installer+xml";
	mimeTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
	mimeTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mimeTypes[".odt"] = "application/vnd.oasis.opendocument.text";
	mimeTypes[".oga"] = "audio/ogg";
	mimeTypes[".ogv"] = "video/ogg";
	mimeTypes[".ogx"] = "application/ogg";
	mimeTypes[".opus"] = "audio/opus";
	mimeTypes[".otf"] = "font/otf";
	mimeTypes[".png"] = "image/png";
	mimeTypes[".pdf"] = "application/pdf";
	mimeTypes[".php"] = "application/x-httpd-php";
	mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
	mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mimeTypes[".rar"] = "application/vnd.rar";
	mimeTypes[".rtf"] = "application/rtf";
	mimeTypes[".sh"] = "application/x-sh";
	mimeTypes[".svg"] = "image/svg+xml";
	mimeTypes[".tar"] = "application/x-tar";
	mimeTypes[".tiff"] = "image/tiff";
	mimeTypes[".tif"] = "image/tiff";
	mimeTypes[".ts"] = "video/mp2t";
	mimeTypes[".ttf"] = "font/ttf";
	mimeTypes[".txt"] = "text/plain";
	mimeTypes[".vsd"] = "application/vnd.visio";
	mimeTypes[".wav"] = "audio/wav";
	mimeTypes[".weba"] = "audio/webm";
	mimeTypes[".webm"] = "video/webm";
	mimeTypes[".webp"] = "image/webp";
	mimeTypes[".woff"] = "font/woff";
	mimeTypes[".woff2"] = "font/woff2";
	mimeTypes[".xhtml"] = "application/xhtml+xml";
	mimeTypes[".xls"] = "application/vnd.ms-excel";
	mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mimeTypes[".xml"] = "application/xml";
	mimeTypes[".xul"] = "application/vnd.mozilla.xul+xml";
	mimeTypes[".zip"] = "application/zip";
	mimeTypes[".3gp"] = "video/3gpp";
	mimeTypes[".3g2"] = "video/3gpp2";
	mimeTypes[".7z"] = "application/x-7z-compressed";
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
		std::string temp(buffer, size);
        body += temp;
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
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + Utils::itos<int>(infos.first) + "</title>\n</head>\n<body>\n<p>" + infos.second.substr(9) + "</p>\n</body>\n</html>";
}

int        Utils::removeSocket(int kq, struct kevent * socket, int nfilter, int * filter, int flags, std::map<int, std::string>& receive, std::map<int, std::string>& send)
{
    int             tmp;
    struct kevent   sockets[nfilter];

    for (int i = 0; i < nfilter; i++)
        EV_SET(&sockets[i], static_cast <int> (socket->ident), filter[i], flags, 0, 0, socket->udata);
//    if (socket->udata != NULL)
//    {
//        delete reinterpret_cast<uDada *> (socket->udata);
//        socket->udata = NULL;
//    }
    tmp = kevent(kq, sockets, nfilter, NULL, 0, NULL);
    close(static_cast <int> (socket->ident));
    Utils::eraseMap(receive, static_cast <int> (socket->ident));
    Utils::eraseMap(send, static_cast <int> (socket->ident));
    return tmp;
}

bool    Utils::timer(const std::clock_t start)
{
    static int i = 1;
    std::clock_t end = std::clock();
    if (static_cast<int>((end - start)) >= (i * CLOCKS_PER_SEC))
        std::cout << i++ << "sc passed\n" << std::endl;
    if (end - start >= 5 * CLOCKS_PER_SEC)
        return true;
    return false;
}

std::string   Utils::basicError(std::pair<int, std::string> const & infos)
{
    std::string body(basicHTML(infos));
    std::string tmp;

    tmp += infos.second.substr(10);
    tmp += "Data: " + Utils::getTime(0) + "\r\n";
    tmp += "Server: WebserverDeSesGrandsMorts/4.20.69\r\n";
    tmp += "Connection: close\r\n";
    tmp += "Content-Type: text/html; charset=utf-8\r\n";
    tmp += "Content-Length: " + itos(body.size()) + "\r\n\r\n";
    tmp += body;
    return tmp;
}

bool    Utils::canAccessfile(std::string const & path)
{
    int     fd = open(path.c_str(), O_RDONLY);
    bool    tmp = fd != -1;
    close(fd);
    return tmp;
}