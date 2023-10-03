#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "Includer.hpp"

typedef std::map<std::string, std::vector<std::string> > t_conf_map;
class Parser;

class Socket {

	typedef std::map<int, std::string>::iterator 	map_it;

public:

    enum parseCase {
        badRequest = 0,
        methodNotAllowed = 1,
        done = 2,
        keepReading = 3,
    };

	Socket();
	Socket(std::vector<int> port, Parser *config);
	Socket(Socket const & instance);
	~Socket();

	Socket &	operator=(Socket const & instance);

	std::vector<int>	getSocket() const;
	int					getKqueue() const;
	void				setKqueue();
	int					isSocket(uintptr_t socket) const;

	int     run();
	int     addSocket(int index);
	int     readSocket(struct kevent & socket);
    int     parseSocket(std::string & socket, std::string & sndRequest);
	void    processSocket(struct kevent & socket, map_it & it);
	int     writeSocket(struct kevent & socket);
    int     getWorkerConnections();

private:

	int 						_kq;
	std::vector<int>			_socket;
	struct sockaddr_in			_hint;
	std::map<int, std::string>	_rcv;
	std::map<int, std::string>	_snd;
	Parser						*_configHead;
};

#endif