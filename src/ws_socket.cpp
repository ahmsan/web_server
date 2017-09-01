/*
 * ws_socket.cpp
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: ahmsan
 */

#include "ws_socket.h"

namespace ws {

WSSocket::~WSSocket() {
	Close();
}

int WSSocket::Close() {
	if ( IsValidFd() ) {
		close(socket_fd_);
		socket_fd_ = 0;
	}
}

int WSSocket::Create(const string &ip, uint16_t port) {
	Close();

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port	= htons(port);

	if ( ip.empty() || ip[0] == '*' ) {
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
	}

	if ( INADDR_NONE == addr.sin_addr.s_addr) {
		err_msg_ = "init socket failed, invalid ip!";
		return -1;
	}

	addr_ = addr;

	socket_fd_ = socket(AF_INET, type == SOCKET_TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
	if ( !IsValidFd() ) {
		err_msg_ = "create socket failed!";
		return -1;
	}

	int flag = fcntl(socket_fd_, F_GETFL, 0);
	flag |= O_NDELAY;
	fcntl(socket_fd_, F_SETFL, flag);

	return 0;
}

void WSSocket::SetOpt(int opt) {
	// TODO
}

int WSSocket::GetOpt() {
	// TODO
}

////////////////////////////////////////////////////////////

int WSTcpSocket::Bind() {
	if ( -1 == bind(socket_fd_, (struct sockaddr *)&addr_, sizeof(addr_)) ) {
		Close();
		err_mgs_ = "bind network address to socket_fd failed!";
		return -1;
	}
	return 0;
}


int WSTcpSocket::Listen(int backlog) {
	if ( backlog < 100 ) {
		backlog = 100;
	}
	if ( -1 == listen(socket_fd_, backlog) ) {
		Close();
		err_mgs_ = "listen socket_fd_ failed!";
		return -1;
	}
	return 0;
}


int WSTcpSocket::Accept(sockaddr_in &client_addr, int flags) {
	int conn_fd = accept(socket_fd_, (struct sockaddr *)&client_addr, sizeof(client_addr));
	if ( conn_fd < 0 ) {
		Close();
		err_mgs_ = "accept socket_fd_ failed!";
		return -1;
	}

	int new_flags = 0;
	if ( flags & SOCK_NONBLOCK ) {
		new_flags |= O_NONBLOCK;
	}
	if (flags & SOCK_CLOEXEC) {
		new_flags |= O_CLOEXEC;
	}

	if ( fcntl(conn_fd, F_SETFL, new_flags) < 0) {
		int saved_errno = errno;
		close(conn_fd);
		err_mgs_ = "set conn_fd_ flag failed!";
		errno = saved_errno;
		return -1;
	}

	return conn_fd;
}

}
