#include "Socket.hpp"

Socket::Socket()
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
	this->_configHead = config;
	this->_hint.sin_family = AF_INET;
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
    std::cout << "vector port size: " << port.size() << std::endl;
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
        std::cout << "masterSocket " << this->_socket.at(i) << ": port= " << ntohs(this->_hint.sin_port) << std::endl;
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

Socket::Socket(Socket const & instance): _socket(instance.getSocket()) {}

Socket::~Socket()
{
	for(size_t i = 0; i < this->_socket.size(); i++)
		close (this->_socket.at(i));
}

/////////////////////////////////////////////////////////////

Socket &	Socket::operator=(Socket const & instance)
{
	if (this != &instance)
		this->_socket = instance.getSocket();
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

//TODO DOIT RETURN PLUS RIEN CAR DOIT CONTINUER DE TOURNER MALGRÉ LES ERREURS DES SYS CALLS OU ERREURS VENANT DU CLIENT
int	Socket::addSocket(int index)
{
	int				newSocket;
	uDada *			info;
	struct kevent	newClient = {};
	struct sockaddr_in	sockAddr = {};
	socklen_t len = sizeof(sockAddr);


	newSocket = accept(this->_socket.at(index), NULL, NULL);
	if (newSocket < 0)
		throw(Error::AcceptException()); //return (1);
	if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
		throw(Error::FcntlException()); //return (1);
	info = new uDada;
	info->masterSocket = this->_socket.at(index);
	if (getsockname(this->_socket.at(index), reinterpret_cast <struct sockaddr *> (&sockAddr), &len) == -1)
		throw(Error::getSockNameException());
	info->masterPort = ntohs(sockAddr.sin_port);
	EV_SET(&newClient, newSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, info);
	if (kevent(this->getKqueue(), &newClient, 1, NULL, 0, NULL) == -1)
		throw(Error::KeventException()); //return (1);
	this->_rcv.insert(std::pair<int, std::string>(newSocket, ""));
	this->_snd.insert(std::pair<int, std::string>(newSocket, ""));
    std::cout << "from masterSocket: " << this->_socket.at(index) << "on port: " << info->masterPort << ", new socket: " << newSocket << std::endl;
	return (0);
}

//TODO DOIT RETURN PLUS RIEN CAR DOIT CONTINUER DE TOURNER MALGRÉ LES ERREURS DES SYS CALLS OU ERREURS VENANT DU CLIENT
int	Socket::readSocket(struct kevent & socket)
{
	map_it			itRcv;
	ssize_t			length;
	struct kevent	change[2] = {};
	char			buffer[2048] = {};

    //SI ERREUR SUR LE SOCKET
	if (socket.flags & EV_EOF)
	{
		this->_rcv.erase(this->_snd.find(static_cast <int> (socket.ident)));
		this->_snd.erase(this->_snd.find(static_cast <int> (socket.ident)));
		EV_SET(&change[0], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, socket.udata);
		socket.flags = EV_DELETE;
		if (socket.udata)
			delete reinterpret_cast<uData *> (socket.udata);
		close(static_cast <int> (socket.ident));
		return (kevent(this->getKqueue(), &socket, 1, NULL, 0, NULL) == -1);
	}

    //RECUPERER DANS LES 2 MAPS LES STRING CORRESPONDANT AU SOCKET
	if ((itRcv = this->_rcv.find(static_cast <int> (socket.ident))) == this->_rcv.end())
		return (1);

    //LIRE LE SOCKET ET METTRE LE CONTENU DANS MAP RCV
	if ((length = read(static_cast <int> (socket.ident), buffer, 2047)) == -1)
		throw(Error::ReadException()); //return (1);
	buffer[length] = 0;
	std::string temp(buffer);
	itRcv->second += temp;
    std::string tmp;

    //SI LU ASSEZ D'INFO POUR GENERER UNE REPONSE
    int parseValue = parseSocket(itRcv->second, tmp);
	if (parseValue == done)
	{
        this->processSocket(socket, itRcv);
        itRcv->second = tmp;
        EV_SET(&change[0], socket.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, socket.udata);
        // SI PAS DE 2ND REQUEST ET TOUT LU SUR SOCKET
        if (itRcv->second.empty() && length < 2047)
        {
            //ENLEVE EVENT DE READ DU SOCKET ET RAJOUTE EVENT DE WRITE DU SOCKET
            EV_SET(&change[1], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, socket.udata);
            return (kevent(this->getKqueue(), change, 2, NULL, 0, NULL) == -1);
        }
        //RAJOUTE EVENT WRITE ET KEEP EVENT READ DU SOCKET
        return (kevent(this->getKqueue(), change, 1, NULL, 0, NULL) == -1);
	}
    else if (parseValue == badRequest || parseValue == methodNotAllowed)
    {
        //TODO GENERER UNE 400 BAD REQUEST
        //TODO GENERER UNE 405 METHOD NOT ALLOWED
    }
    //FAIT RIEN POUR KEEP READING CAR ALL GOOD ?
    //TODO CREER UN TIMEOUT SI KEEP WAITING FOR INFO BUT NOTHING COMES
	return (0);
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
            if (read.find("Transfer-Encoding: chunked\n\r")== std::string::npos)
                return badRequest;
            //IF TRANSFER ENCODING
            size_t  endRequest = read.find("0\r\n\r\n");
            if (endRequest == std::string::npos)
                return keepReading;
            //IF 2ND REQUEST RIGHT AFTER
            if (endRequest + 4 != read.size())
            {
                sndRequest = read.substr(endRequest + 5);
                read.erase(endRequest + 5);
            }
        }
        //IF CONTENT-LENGTH
        else
        {
            size_t  startValue = read.find(":", cntLen);
            size_t  endValue = read.find("\r\n", cntLen);
            if (startValue == std::string::npos)
                return badRequest;
            size_t bodyLen = static_cast<unsigned long> (atol(read.substr(startValue + 2, endValue - (startValue + 2)).c_str()));
            size_t endHeaders = read.find("\r\n\r\n");
            //IF NO END OF HEADERS MARK
            if (endHeaders == std::string::npos)
                return badRequest;
            //IF 2ND REQUEST RIGHT AFTER
            if (endHeaders + 3 + bodyLen != read.size())
            {
                sndRequest = read.substr(endHeaders + 4 + bodyLen);
                read.erase(endHeaders + 4 + bodyLen);
            }
        }
    }
    // PAS TROUVER METHODE
    else
        return methodNotAllowed;
    return done;
}

//int	Socket::processSocket(struct kevent & socket, map_it & it)
//{
//	HttpRequest		request(it->second);
//
//	if (request.parseRequest())
//	{
//		HttpResponse	response(request, socket);
//		std::pair<int, std::string> res = response.processRequest(*this->_configHead);
//		if (res.first == 200)
//		{
//			if ((it = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end())
//				return false;
//			it->second = response.generateResponse();
//            if (response.getHeader("Connection") != "keep-alive")
//                this->_rcv.erase(it);
//		}
//		else
//			it->second = res.second;
//	}
//	else
//        it->second = "HTTP/1.1 400 Bad Request\r\n";
//	return 0;
//}

void    Socket::processSocket(struct kevent & socket, map_it & it)
{
    HttpRequest		            request(it->second);
    bool                        parseValue = request.parseRequest();
    HttpResponse	            response(request, socket);
    std::pair<int, std::string> res;

    //RECUPERER DANS MAP SEND LE STRING CORRESPONDANT AU SOCKET
    if ((it = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end())
    {
        it->second = response.generateResponse(std::pair<int, std::string> (500, "HTTP/1.1 500 Internal Server Error\r\n"));
        return ;
    }
    if (parseValue)
    {
        it->second = response.generateResponse(response.processRequest(*this->_configHead));
        if (response.getHeader("Connection") != "keep-alive")
            this->_rcv.erase(it);
        return ;
    }
    else
        it->second = response.generateResponse(std::pair<int, std::string> (400, "HTTP/1.1 400 Bad Request\r\n"));
    return ;
}


int	Socket::writeSocket(struct kevent & socket)
{
	map_it	it;
	ssize_t	length;

	if (socket.flags & EV_EOF)
	{
		//    ?     this->_rcv.erase(this->_snd.find(static_cast <int> (socket.ident)));
		this->_snd.erase(this->_snd.find(static_cast <int> (socket.ident)));
		socket.flags = EV_DELETE;
		if (socket.udata)
			delete reinterpret_cast<uData *> (socket.udata);
		close(static_cast <int> (socket.ident));
		return (kevent(this->getKqueue(), &socket, 1, NULL, 0, NULL) == -1);
	}
	if ((it = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end())
		return 1;
	if ((length = write(static_cast <int> (socket.ident), it->second.data(), \
	it->second.length() > 2047 ? 2047 : it->second.length())) == -1)
		throw(Error::WriteException()); //return (1);
	it->second.erase(0, length);
	if (it->second.empty())
	{
		// TODO checker si keep-alive
        this->_snd.erase(it); // && TODO close(static_cast <int> (socket.ident))
		EV_SET(&socket, static_cast <int> (socket.ident), EVFILT_WRITE, EV_DELETE, 0, 0, socket.udata);
		if (socket.udata)
			delete reinterpret_cast<uDada *> (socket.udata);
		return (kevent(this->getKqueue(), &socket, 1, NULL, 0, NULL) == -1 || \
		close(static_cast <int> (socket.ident)) == -1);
	}
	return (0);
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
        std::cout << "adding masterSocket: " << j << " to kevent" << std::endl;
	}
	while(true)
	{
		nbrRequests = kevent(this->getKqueue(), NULL, 0, events, 100, NULL);
		if (nbrRequests < 0)
			throw(Error::KeventException());
		for (int i = 0; i < nbrRequests; i++)
		{
			indexSocket = this->isSocket(events[i].ident);
			if ((events[i].flags & EV_ERROR) || \
				(indexSocket >= 0 && this->addSocket(indexSocket)) || \
				(indexSocket == -1 && events[i].filter == EVFILT_READ && this->readSocket(events[i])) || \
				(events[i].filter == EVFILT_WRITE && this->writeSocket(events[i])))
				throw(Error::TcpException());
		}
		//TODO: arreter exception quand une erreur de read ou write, laisser tourner le serveur
	}
}
