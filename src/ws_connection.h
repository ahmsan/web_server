/*
 * ws_connection.h
 *
 *  Created on: 2016Äê7ÔÂ1ÈÕ
 *      Author: ahmsan
 */

#ifndef WS_CONNECTION_H_
#define WS_CONNECTION_H_

#include "ws_comm.h"
#include "ws_http_request.h"

#include <time.h>

namespace ws {

class WSConnection: private UnCopyable {
public:
	WSConnection(int fd, int keep_alive_timeout);

	int  HandleRead();
	int  HandleWrite();
	int  WriteToBuffer(const char *data, int data_len);

	int  Close();

	void Reset();

	inline bool IsTimeout() {
		return time(NULL) - WSConnection::keep_alive_timeout_ > t_last_wakeup_;
	}

	inline void SetEPoll(WSEPoll *epoll) {
		epoll_ = epoll;
	}

private:
	int  handleHttpReq();
	void respErrDirectly(http_status stat, bool shutdown = true);

private:
	int 		fd_;
	bool		shutdown_fd_;
	time_t		t_last_wakeup_;
	static int 	keep_alive_timeout_;

	int		read_len_;
	char 	read_buffer_[CONN_READ_BUFFER_MAX];
	int 	write_len_;
	char	write_buffer_[CONN_WRITE_BUFFER_MAX];

	WSEPoll*	  epoll_;
	WSHttpRequest http_request_;
};

}

#endif /* WS_CONNECTION_H_ */
