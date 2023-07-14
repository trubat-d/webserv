#include "Socket.hpp"

Socket::Socket()
{
	this->_socket.push_back(socket(AF_INET, SOCK_STREAM, 0));
	if (this->_socket.at(0) < 0)
		throw(Error::SocketException());
	this->_hint.sin_port = htons(80);
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->_socket.at(0), reinterpret_cast <struct sockaddr *> (&this->_hint), static_cast<socklen_t>(sizeof(this->_hint))) < 0)
		throw(Error::BindException());
	if (listen(this->_socket.at(0), 1024))
		throw(Error::BindException());
}

Socket::Socket(std::vector<int> & port)
{
	this->_hint.sin_family = AF_INET;
	this->_hint.sin_addr.s_addr = htonl(INADDR_ANY);
	for (unsigned long i = 0; i < port.size() ; i++)
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
	for(unsigned long i = 0; i < this->_socket.size(); i++)
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

int		Socket::isSocket(uintptr_t const socket) const
{
	for (unsigned long i = 0; i < this->_socket.size(); i++)
		if (static_cast <int> (socket) == this->_socket.at(i))
			return static_cast <int> (i);
	return -1;
}
/////////////////////////////////////////////////////////////

int	Socket::addSocket(int index)
{
	int				newSocket;
	struct kevent	newClient = {};
	unsigned long size = sizeof(this->_hint);

	if (index < 0)
		return (1);
	newSocket = accept(this->_socket.at(index), reinterpret_cast <struct sockaddr *> (&this->_hint),\
				reinterpret_cast <socklen_t * > (&size));
	if (newSocket < 0)
		return (1);
	if (fcntl(newSocket, F_SETFL, O_NONBLOCK) == -1)
		return (1);
	EV_SET(&newClient, newSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(this->_kq, &newClient, 1, NULL, 0, NULL) == -1)
		return (1);
	this->_rcv.insert(std::pair<int, std::string>(newSocket, ""));
	this->_snd.insert(std::pair<int, std::string>(newSocket, ""));
	return (0);
}

int	Socket::readSocket(struct kevent & socket)
{
	ssize_t				length;
	char			buffer[2048];
	struct kevent	change[2];
	map_it			it;

	if (socket.flags & EV_EOF)
	{
		this->_rcv.erase(this->_snd.find(socket.ident));
		this->_snd.erase(this->_snd.find(socket.ident));
		EV_SET(&change[0], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, 0);
		close(socket.ident);
		return (kevent(this->_kq, change, 1, NULL, 0, NULL) == -1);
	}
	if ((it = this->_rcv.find(socket.ident)) == this->_rcv.end())
		return (1);
	if ((length = read(socket.ident, buffer, 2047)) == -1)
		return (1);
	buffer[length] = 0;
	std::string temp(buffer);
	it->second += temp;
	if (temp.find("EOF") != std::string::npos)
	{
		//process what to do with data read;
//		this->_rcv.erase(this->_snd.find(socket.ident));
		this->_snd.find(socket.ident)->second = it->second;;
		EV_SET(&change[0], socket.ident, EVFILT_READ, EV_DELETE, 0, 0, 0);
		EV_SET(&change[1], socket.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
		if (kevent(this->_kq, change, 2, NULL, 0, NULL) == -1)
			return (1);
	}
	return (0);
}


int	Socket::writeSocket(struct kevent & socket)
{
	ssize_t		length;
	map_it	it;

	if (socket.flags & EV_EOF)
	{
		this->_rcv.erase(this->_snd.find(socket.ident));
		this->_snd.erase(this->_snd.find(socket.ident));
		EV_SET(&socket, socket.ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
		close(socket.ident);
		return (kevent(this->_kq, &socket, 1, NULL, 0, 0) == -1);
	}
	it = this->_snd.find(socket.ident);
	if (it == this->_snd.end())
		return (1);
	if ((length = write(socket.ident, it->second.data(), it->second.length() > 2047 ? 2047: it->second.length())) == -1)
		return (1);
	it->second.erase(0, length);
	if (it->second.empty())
	{
		this->_snd.erase(this->_snd.find(socket.ident));
		EV_SET(&socket, socket.ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
		return (kevent(this->_kq, &socket, 1, NULL, 0, NULL) == -1 || \
		close(socket.ident) == -1);
	}
	return (0);
}

/////////////////////////////////////////////////////////////

int	Socket::run()
{
	int	nbrRequests;
	struct kevent newSocket = {};
	struct kevent events[100];

	if ((this->_kq = kqueue()) == -1)
		throw(Error::SocketException());
	for (unsigned long j = 0; j < this->_socket.size(); j++)
	{
		fcntl(this->_socket[j], F_SETFL, O_NONBLOCK);
		EV_SET(&newSocket, this->_socket[j], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
		if (kevent(this->_kq, &newSocket, 1, NULL, 0, NULL) == -1)
			exit (1);
	}
	while(true)
	{
		nbrRequests = kevent(this->_kq, NULL, 0, events, 100, NULL);
		if (nbrRequests < 0)
			throw(Error::KeventException());
		for (int i = 0; i < nbrRequests; i++)
		{
			if ((events[i].flags & EV_ERROR) || \
				(this->addSocket(this->isSocket(events[i].ident))) || \
				(this->isSocket(events[i].ident) == -1 && events[i].filter == EVFILT_READ && this->readSocket(events[i])) || \
				/*(events[i].ident == static_cast<uintptr_t>(this->_socket) && this->addSocket()) || \
				(events[i].ident != static_cast<uintptr_t>(this->_socket) && events[i].filter == EVFILT_READ && this->readSocket(events[i])) || \ */
				(events[i].filter == EVFILT_WRITE && this->writeSocket(events[i])))
			{
				if (events[i].flags & EV_ERROR)
					std::cout << "EV_ERROR" << std::endl;
				exit (1);
			}
		}
	}
}
