#include "Socket.hpp"

Socket::Socket(): _kq(-1), _hint((struct sockaddr_in){}), _configHead(NULL)
{
	this->_socket.push_back(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.at(0) < 0)
		throw(Error::SocketException());
	this->_hint.sin_port = htons(80);
	this->_hint.sin_family = AF_INET;
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->_socket.at(0), reinterpret_cast <struct sockaddr *> (&this->_hint), sizeof(this->_hint)) < 0)
		throw(Error::BindException());
	if (listen(this->_socket.at(0), 1024))
		throw(Error::BindException());
}

Socket::Socket(std::vector<int> port, Parser *config)
{
	this->_kq = -1;
	this->_hint = (struct sockaddr_in){};
	this->_configHead = config;
	this->_hint.sin_family = AF_INET;
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
//    std::cout << "vector port size: " << port.size() << std::endl;
	for (size_t i = 0; i < port.size(); i++)
	{
		this->_socket.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (this->_socket.at(i) < 0)
			throw(Error::SocketException());
		this->_hint.sin_port = htons(port.at(i));
		if (bind(this->_socket.at(i), reinterpret_cast <struct sockaddr *> (&this->_hint),sizeof(this->_hint)) < 0)
			throw(Error::BindException());
		if (listen(this->_socket.at(i), getWorkerConnections()))
			throw(Error::BindException());
//        std::cout << "masterSocket " << this->_socket.at(i) << ": port= " << ntohs(this->_hint.sin_port) << std::endl;
	}
}

int Socket::getWorkerConnections()
{
    if(!this->_configHead)
        return 0;
    std::vector<std::string> event = this->_configHead->getEvent("worker_connections");
    if(event.size() != 1)
        return 0;
    else
        return Utils::vecStoI(event)[0];
}

Socket::Socket(Socket const & instance)
{
    if (this != &instance)
        *this = instance;
}

Socket::~Socket()
{
	for(size_t i = 0; i < this->_socket.size(); i++)
		close (this->_socket.at(i));
}

/////////////////////////////////////////////////////////////

Socket &	Socket::operator=(Socket const & instance)
{
	if (this != &instance)
    {
        this->_socket = instance.getSocket();
        this->_kq = instance.getKqueue();
        this->_hint = instance.getHint();
        this->_rcv = instance.getRcv();
        this->_snd = instance.getSnd();
        this->_configHead = instance.getConfigHead();
    }
	return *this;
}

/////////////////////////////////////////////////////////////

std::vector<int>	Socket::getSocket() const
{
	return this->_socket;
}

int					Socket::getKqueue() const
{
	return this->_kq;
}

std::map<int, std::string>  Socket::getRcv() const
{
    return this->_rcv;
}

std::map<int, std::string>  Socket::getSnd() const
{
    return this->_snd;
}

struct sockaddr_in  Socket::getHint() const
{
    return this->_hint;
}

Parser *  Socket::getConfigHead() const
{
    return this->_configHead;
}

void				Socket::setKqueue()
{
	if ((this->_kq = kqueue()) == -1)
		throw(Error::SocketException());
}

int		Socket::isSocket(uintptr_t socket) const
{
	for (size_t i = 0; i < this->_socket.size(); i++)
		if (static_cast <int> (socket) == this->_socket.at(i))
			return static_cast <int> (i);
	return -1;
}
/////////////////////////////////////////////////////////////

void	Socket::addSocket(int index)
{
	int				newSocket;
	struct kevent	newClient = {};

	newSocket = accept(this->_socket.at(index), NULL, NULL);
	if (newSocket < 0)
		return ;
	if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
		return ;

    ///////////////////////////////////////////////////////
    struct sockaddr_in	sockAddr = {};
    socklen_t len = sizeof(sockAddr);
    getsockname(this->_socket.at(index), reinterpret_cast <struct sockaddr *> (&sockAddr), &len);
    //////////////////////////////////////////////////////

    // DEFINE CONNECTION FOR THIS SOCKET AS FALSE;
    this->_mapConnect[static_cast<uintptr_t>(newSocket)] = false;
	EV_SET(&newClient, newSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    // CHECK IF CORRECTLY PUT IN KQUEUE
    if (kevent(this->getKqueue(), &newClient, 1, NULL, 0, NULL) != -1)
    {
        this->_rcv.insert(std::pair<int, std::string>(newSocket, ""));
	    this->_snd.insert(std::pair<int, std::string>(newSocket, ""));
//        std::cout << "from masterSocket: " << this->_socket.at(index) << "on port: " << ntohs(sockAddr.sin_port) << ", new socket: " << newSocket << std::endl;
    }
}

void	Socket::readSocket(struct kevent & socket)
{
	map_it			itRcv, itSnd;
	ssize_t			length;
	struct kevent	change[2] = {};
	char			buffer[2048] = {};
    // SI ERREUR SUR LE SOCKET
	if (socket.flags & EV_EOF)
        return (void) Utils::removeSocket(this->getKqueue(), socket, 1, (int [1]){EVFILT_READ}, EV_DELETE, this->_rcv, this->_snd);
    // RECUPERER DANS LES 2 MAPS LES STRING CORRESPONDANT AU SOCKET
	if ((itRcv = this->_rcv.find(static_cast <int> (socket.ident))) == this->_rcv.end() || (itSnd = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end())
        return ;

    // LIRE LE SOCKET ET METTRE LE CONTENU DANS MAP RCV
	if ((length = read(static_cast <int> (socket.ident), buffer, 2047)) == -1)
    {
        Utils::eraseMap(this->_rcv, static_cast <int> (socket.ident));
        itSnd->second = Utils::basicError(std::pair<int, std::string> (500, "HTTP/1.1 500 Internal Server Error\r\n"));
        return ;
    }
    // CONCATENE CONTENU LU DANS MAP:RCV
	buffer[length] = 0;
	std::string temp(buffer, length);
	itRcv->second += temp;

    // SI LU ASSEZ D'INFO POUR GENERER UNE REPONSE
    int parseValue = this->processSocket(socket, itRcv->second, itSnd->second);
	if (parseValue != keepReading)
	{
        EV_SET(&change[0], socket.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        // SI PAS DE 2ND REQUEST ET TOUT LU SUR SOCKET ET CONNECTION: CLOSED!
        if (itRcv->second.empty() && !this->_mapConnect[socket.ident])
        {
            // ENLEVE EVENT DE READ DU SOCKET ET RAJOUTE EVENT DE WRITE DU SOCKET
            EV_SET(&change[1], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(this->getKqueue(), change, 2, NULL, 0, NULL);
        }
        // RAJOUTE EVENT WRITE ET KEEP EVENT READ DU SOCKET
        kevent(this->getKqueue(), change, 1, NULL, 0, NULL);
	}
}

int    Socket::processSocket(struct kevent & socket, std::string & request, std::string & response)
{
    std::string                 tmp;
    std::pair<int, std::string> res;
    int                         parseValue = this->parseSocket(request, tmp);
    Http                        client;

    //SI LU ASSEZ D'INFO POUR GENERER UNE REPONSE
    if (parseValue == done)
    {
        client = Http(request,socket);
        request = tmp;
        //DEFINE CTR_DATA & HEADERS & BODY IF OK
        if (client.parseRequest())
        {
            //DEFINE SI REQUEST OK WITH .CONF
            res = client.processRequest(*this->_configHead);
            //SET CGI ENV
            if (res.first == 200)
                res = client.setCGIEnv();
            //IF CONNECTION: KEEP-ALIVE
            if (client.getHeader("Connection") == "keep-alive")
                this->_mapConnect[socket.ident] = true;
            //CREATE RESPONSE
            response = client.generateResponse(res);
            //SET CONNECTION CLOSE IF RESPONSE IS AN ERROR TODO , AMELIORER PAR MODIFIER UNE REF ET RENVOYER LE STATUS CODE
            if (response.find("HTTP/1.1 200") == std::string::npos && response.find("HTTP/1.1 201") && response.find("HTTP/1.1 302") == std::string::npos)
                this->_mapConnect[socket.ident] = false;
        }
        else
            response = client.generateResponse(std::pair<int, std::string> (400, "HTTP/1.1 400 Bad Request\r\n"));
    }
    else if (parseValue == badRequest)
        response = Utils::basicError(std::pair<int, std::string> (400, "HTTP/1.1 400 Bad Request\r\n"));
    else if (parseValue == methodNotAllowed)
        response = Utils::basicError(std::pair<int, std::string> (405, "HTTP/1.1 405 Method Not Allowed\r\n"));
    //SI ERREUR RESET CONNECTION TO CLOSE
    if (parseValue == badRequest || parseValue == methodNotAllowed)
        this->_mapConnect[socket.ident] = false;
    return parseValue;
}

int     Socket::parseSocket(std::string & read, std::string & sndRequest)
{
    size_t          postCase = read.find("POST");
    size_t          getCase = read.find("GET");
    size_t          delCase = read.find("DELETE");

    //si 1er request est GET ou DELETE
    if (!getCase || !delCase)
    {
        size_t  endRequest = read.find("\r\n\r\n");
        //IF NO END MARKER, KEEP READING
        if (endRequest == std::string::npos)
            return  keepReading;
        //IF 2ND REQUEST RIGHT AFTER
        if (endRequest + 3 != read.size())
        {
            sndRequest = read.substr(endRequest + 4);
            read.erase(endRequest + 4);
        }
    }
    //si 1er request est POST
    else if (!postCase)
    {
        size_t cntLen = read.find("Content-Length: ");
        //IF NO CONTENT-LENGTH
        if (cntLen == std::string::npos)
        {
            //IF NO TRANSFER ENCODING
            if (read.find("Transfer-Encoding: chunked\n\r") == std::string::npos)
                return keepReading;
            //IF TRANSFER ENCODING
            size_t  endRequest = read.find("0\r\n\r\n");
            if (endRequest == std::string::npos)
                return keepReading;
            //IF 2ND REQUEST RIGHT AFTER
            if (endRequest + 4 < read.size())
            {
                sndRequest = read.substr(endRequest + 5);
                read.erase(endRequest + 5);
            }
        }
        //IF CONTENT-LENGTH
        else
        {
            size_t  startValue = read.find(':', cntLen);
            size_t  endValue = read.find("\r\n", cntLen);
            if (startValue == std::string::npos)
                return badRequest;
            size_t bodyLen = static_cast<unsigned long> (atol(read.substr(startValue + 2, endValue - (startValue + 2)).c_str()));
            size_t endHeaders = read.find("\r\n\r\n");
            //IF NO END OF HEADERS MARK
            if (endHeaders == std::string::npos)
                return keepReading;
            //IF 2ND REQUEST RIGHT AFTER
            if (endHeaders + 3 + bodyLen < read.size())
            {
                sndRequest = read.substr(endHeaders + 4 + bodyLen);
                read.erase(endHeaders + 4 + bodyLen);
            }
			//IF NOT HAVE ALL BODY YET
			if (endHeaders + 3 + bodyLen > read.size())
				return keepReading;
        }
    }
    // PAS TROUVER METHODE
    else
        return methodNotAllowed;
    return done;
}


void	Socket::writeSocket(struct kevent & socket)
{
	map_it	it;
	ssize_t	length;

    //IF CONNECTION LOST OR DON'T FIND IN MAP
	if (socket.flags & EV_EOF || ((it = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end()))
        return (void) Utils::removeSocket(this->getKqueue(), socket, 2, (int [2]){EVFILT_READ, EVFILT_WRITE}, EV_DELETE, this->_rcv, this->_snd);
    //WRITE INTO SOCKET
	if ((length = write(static_cast <int> (socket.ident), it->second.data(), \
	it->second.length() > 2047 ? 2047 : it->second.length())) != -1)
	    it->second.erase(0, length);

    //WRITE ERROR : DELETE TOUT || WROTE EVERYTHING & CONNECTION: CLOSE
    if (length == -1 || (it->second.empty() && !this->_mapConnect[socket.ident]))
        return (void) Utils::removeSocket(this->getKqueue(), socket, 2, (int [2]){EVFILT_READ, EVFILT_WRITE}, EV_DELETE, this->_rcv, this->_snd);

    //IF WROTE EVERYTHING & CONNECTION: KEEP ALIVE
    if (it->second.empty())
	{
		EV_SET(&socket, socket.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		kevent(this->getKqueue(), &socket, 1, NULL, 0, NULL);
	}
}

/////////////////////////////////////////////////////////////

int	Socket::run()
{
	int	nbrRequests;
	int	indexSocket;
	struct kevent newSocket = {};
	struct kevent events[100];

	this->setKqueue();
	for (size_t j = 0; j < this->_socket.size(); j++)
	{
		fcntl(this->_socket[j], F_SETFL, O_NONBLOCK);
		EV_SET(&newSocket, this->_socket[j], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		if (kevent(this->getKqueue(), &newSocket, 1, NULL, 0, NULL) == -1)
			exit (1);
//        std::cout << "adding masterSocket: " << j << " to kevent" << std::endl;
	}
	while (true)
	{
		nbrRequests = kevent(this->getKqueue(), NULL, 0, events, 100, NULL);
		if (nbrRequests < 0)
			throw(Error::KeventException());
		for (int i = 0; i < nbrRequests; i++)
		{
			indexSocket = this->isSocket(events[i].ident);
            //SI CONNECTION LOST WITH CLIENT
			if (events[i].flags & EV_ERROR)
                (void) Utils::removeSocket(this->getKqueue(), events[i], 2, (int[2]) {EVFILT_WRITE, EVFILT_READ}, EV_DELETE, this->_rcv, this->_snd);
            //CHECK SI ACCEPT READ OR WRITE
            if (indexSocket >= 0)
                this->addSocket(indexSocket);
            else if (indexSocket == -1 && events[i].filter == EVFILT_READ)
				this->readSocket(events[i]);
            else if (indexSocket == -1 && events[i].filter == EVFILT_WRITE)
                this->writeSocket(events[i]);
		}
	}
}
