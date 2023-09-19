#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Includer.hpp"

typedef std::map<std::string, std::vector<std::string> > t_conf_map;
class Parser;

typedef struct uData {
	int masterSocket;
	int masterPort;
} uDada;

class Socket {

	typedef std::map<int, std::string>::iterator 	map_it;

public:

	Socket();
	Socket(std::vector<int> & port, Parser *config);
	Socket(Socket const & instance);
	~Socket();

	Socket &	operator=(Socket const & instance);

	std::vector<int>	getSocket() const;
	int					getKqueue() const;
	void				setKqueue();
	int					isSocket(uintptr_t socket) const;

	int run();
	int	addSocket(int index);
	int	readSocket(struct kevent & socket);
	int processSocket(struct kevent & socket, map_it & it);
	int	writeSocket(struct kevent & socket);

private:

	int 						_kq;
	std::vector<int>			_socket;
	struct sockaddr_in			_hint;
	std::map<int, std::string>	_rcv;
	std::map<int, std::string>	_snd;
	Parser						*_configHead;
};

#endif