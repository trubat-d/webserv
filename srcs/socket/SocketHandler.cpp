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

Socket::Socket(std::vector<int> & port, Parser *config)
{
	this->_configHead = config;
	this->_hint.sin_family = AF_INET;
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
	for (size_t i = 0; i < port.size(); i++)
	{
		this->_socket.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (this->_socket.at(i) < 0)
			throw(Error::SocketException());
		this->_hint.sin_port = htons(port.at(i));
		if (bind(this->_socket.at(i), reinterpret_cast <struct sockaddr *> (&this->_hint),sizeof(this->_hint)) < 0)
			throw(Error::BindException());
		if (listen(this->_socket.at(i), 1024))
			throw(Error::BindException());
	}
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
	return (0);
}

int	Socket::readSocket(struct kevent & socket)
{
	map_it			it;
	ssize_t			length;
	struct kevent	change[2];
	char			buffer[2048];

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
	if ((it = this->_rcv.find(static_cast <int> (socket.ident))) == this->_rcv.end())
		return (1);
	if ((length = read(static_cast <int> (socket.ident), buffer, 2047)) == -1)
		throw(Error::ReadException()); //return (1);
	buffer[length] = 0;
	std::string temp(buffer);
	it->second += temp;
	if(length < 2047)
	{
		this->processSocket(socket, it);
		EV_SET(&change[0], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, socket.udata);
		EV_SET(&change[1], socket.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, socket.udata);
		if (kevent(this->getKqueue(), change, 2, NULL, 0, NULL) == -1)
			throw(Error::KeventException()); //return (1);
	}
	//TODO: du code pour gerer separation entre 2 request a la suite
	return (0);
}

int	Socket::processSocket(struct kevent & socket, map_it & it)
{
	HttpRequest		request(it->second);

	this->_rcv.erase(it);
	if (request.parseRequest())
	{
		HttpResponse	response(request, socket);
		if (response.processRequest(*this->_configHead))
		{
			if ((it = this->_snd.find(static_cast <int> (socket.ident))) == this->_snd.end())
				return 1;
			it->second = response.generateResponse();
		}
		//TODO it->second = response.genereateError();
	}
	it->second = "HTTP/1.1 400 Bad Request\r\n";
	return 0;
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
		this->_snd.erase(it);
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
			exit (1);https://code-with-me.global.jetbrains.com/PkkxBUsLWVPorAtNxiVclA#p=CL&fp=7EE4813ED49455294FE29FF95F4D1003581BF67BE42C33C98F9944B9592851BA
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
