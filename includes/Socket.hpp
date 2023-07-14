#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <vector>

#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "Includer.hpp"

class Socket {

public:

	Socket();
	Socket(std::vector<int> & port);
	Socket(Socket const & instance);
	~Socket();

	Socket &	operator=(Socket const & instance);

	std::vector<int>	getSocket() const;
	int					getKqueue() const;
	int					isSocket(uintptr_t socket) const;

	int run();
	int	addSocket(int index);
	int	readSocket(struct kevent & socket);
	int	writeSocket(struct kevent & socket);

	typedef std::map<int, std::string>::iterator 	map_it;
//	typedef std::vector<int>::iterator 				vec_it;

private:

	int 					_kq;
	std::vector<int>		_socket;
	struct sockaddr_in		_hint;
	std::map<int, std::string>	_rcv;
	std::map<int, std::string>	_snd;

};

#endif