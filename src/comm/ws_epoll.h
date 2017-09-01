/*
 * ws_epoll.h
 *
 *  Created on: 2016年7月1日
 *      Author: ahmsan
 */

#ifndef WS_EPOLL_H_
#define WS_EPOLL_H_

#include "uncopyable.h"

#include <string>
#include <sys/epoll.h>
using std::string;

namespace ws {

enum event_type {
	EVENT_TYPE_READ		= 1 << 0,
	EVENT_TYPE_WRITE	= 1 << 1,
	EVENT_TYPE_ERR		= 1 << 2,
}

// 对linux epoll做封装
class WSEPoll: private UnCopyable {
public:
	WSEPoll(int timeout = -1): epoll_fd_(-1), max_event_(),
		event_timeout_(timeout), notified_event_idx_(0),
		notified_event_cnt_(0), epoll_evs_(NULL) {
	}

	int CreateEPoll(int max_socket_num);

	int AddEPollEvent(int fd, int flag);
	int ModEPollEvent(int fd, int flag);
	int DelEPollEvent(int fd);

	int WaitEPollEvents();
	int GetNextEventFd(int &fd, int &flag);

	int CloseEPollFd();

	inline int GetEPollFd() {
		return epoll_fd_;
	}

	inline string GetLastErrMsg() {
		return err_msg_;
	}

private:
	void setEPollEv(struct epoll_event &ev, int  fd, int  flag);
	void getEPollEv(struct epoll_event &ev, int &fd, int &flag);
	void setErrMsg(const char *err_msg);

private:
	int 	epoll_fd_;
	int 	max_event_;
	int 	event_timeout_;
	int 	notified_event_idx_;
	int 	notified_event_cnt_;

	string	err_msg_;
	struct epoll_event* epoll_evts_;
};

}


#endif /* WS_EPOLL_H_ */
