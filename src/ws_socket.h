/*
 * ws_socket.h
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: ahmsan
 */

#ifndef WS_SOCKET_H_
#define WS_SOCKET_H_

#include "ws_comm.h"

namespace ws {

enum socket_type {
	SOCKET_TCP,
	SOCKET_UDP,
};

class WSSocket {
public:
	explicit WSSocket(socket_type type):
		type_(type), socket_fd_(-1) {
	}

	virtual ~WSSocket();

	int  Create(const string &ip, uint16_t port);
	int  Close();

	int  GetOpt();
	void SetOpt(int opt);

	inline int GetFd() {
		return socket_fd_;
	}

	inline string GetLastErrMsg() {
		return err_msg_;
	}

protected:
	inline bool IsValidFd() {
		return socket_fd_ > 0;
	}

protected:
	int 	socket_fd_;
	string 	err_msg_;
	socket_type	type_;
	sockaddr_in addr_;
};


////////////////////////////////////////////////////////


class WSTcpSocket: public WSSocket {
public:
	WSTcpSocket(): WSSocket(SOCKET_TCP) {
	}
	~WSTcpSocket();

	int  Bind();
	int  Listen(int backlog);
	int  Accept(sockaddr_in &client_addr, int flags);
};

}


#endif /* WS_SOCKET_H_ */
