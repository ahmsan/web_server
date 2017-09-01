/*
 * ws_epoll.cpp
 *
 *  Created on: 2016Äê7ÔÂ1ÈÕ
 *      Author: ahmsan
 */

#include "ws_epoll.h"

#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

namespace ws {

void WSEPoll::setErrMsg(const char *err_msg) {
	if ( err_msg != NULL ) {
		err_msg_ = err_msg;
		if ( errno != 0 ) {
			err_msg_.append(strerror(errno));
		}
	}
}


void WSEPoll::setEPollEv(struct epoll_event &ev, int fd, int flag) {
	uint32_t events = 0;

	if ( ( flag & EVENT_TYPE_READ ) != 0 ) {
		events |= EPOLLIN;
	}
	if ( ( flag & EVENT_TYPE_WRITE ) != 0 ) {
		events |= EPOLLOUT;
	}
	if ( ( flag & EVENT_TYPE_ERR ) != 0 ) {
		events |= EPOLLHUP | EPOLLERR;
	}

	ev.data.fd = fd;
	ev.events = epoll_events;
}


void WSEPoll::getEPollEv(struct epoll_event &ev, int &fd, int &flag) {
	int flag_tmp = 0;
	if ( ( ev.events & EPOLLIN ) != 0) {
		flag_tmp |= EVENT_TYPE_READ;
	}
	if ( ( ev.events & EPOLLOUT ) != 0) {
		flag_tmp |= EVENT_TYPE_WRITE;
	}
	if ( ( ev.events & (EPOLLHUP | EPOLLERR) ) != 0) {
		flag_tmp |= EVENT_TYPE_ERR;
	}

	fd = ev.data.fd;
	flag = flag_tmp;
}


int WSEPoll::CreateEPoll(int max_socket_num) {
	if ( max_socket_num <= 0 ) {
		max_socket_num = 1000;
	}

	max_event_ = max_socket_num;

	int epoll_fd = epoll_create(max_socket_num);
	if ( epoll_fd < 0 ) {
		setErrMsg("create epoll failed!");
		return -1;
	}

	epoll_evts_ = (struct epoll_event *)calloc(max_event_ * sizeof(struct epoll_event));
	if ( epoll_evts_ == NULL ) {
		setErrMsg("calloc epoll events failed!");
		return -1;
	}

	epoll_fd_ = epoll_fd;

	return 0;
}


int WSEPoll::AddEPollEvent(int fd, int flag) {
	struct epoll_event ev;
	if ( fd < 0 ) {
		setErrMsg("invalid fd!");
		return -1;
	}

	int ret = 0;
	setEPollEv(ev, fd, flag);
	if ( epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) < 0 ) {
		// check if this fd existed, try modify it
		if ( errno != EEXIST || epoll_ctl(pstMfpMng->epollfd, EPOLL_CTL_MOD, iFd, &ev) < 0 ) {
			ret = -1;
		}
	}
	if ( ret != 0 ) {
		setErrMsg("epoll_ctl failed!");
	}
	return ret;
}


int WSEPoll::ModEPollEvent(int fd, int flag) {
	struct epoll_event ev;
	if ( fd < 0 ) {
		setErrMsg("invalid fd!");
		return -1;
	}

	int ret = 0;
	setEPollEv(ev, fd, flag);
	if ( epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev) < 0 ) {
		// if there is no this entry, try to add it
		if ( errno != ENOENT || epoll_ctl(pstMfpMng->epollfd, EPOLL_CTL_ADD, iFd, &ev) < 0 ) {
			ret = -1;
		}
	}
	if ( ret != 0 ) {
		setErrMsg("epoll_ctl failed!");
	}
	return ret;
}


int WSEPoll::DelEPollEvent(int fd) {
    struct epoll_event ev;
	if ( fd < 0 ) {
		setErrMsg("invalid fd!");
		return -1;
	}

	int ret = 0;
    if ( epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ev) < 0 ) {
    	if ( errno != ENOENT ) {
            setErrMsg("no entry");
        } else {
        	setErrMsg("del failed!");
        }
    	ret = -1;
    }
	return ret;
}


int WSEPoll::WaitEPollEvents() {
	if ( event_timeout_ < 0 ) {
		event_timeout_ = -1;
	}

	int nfds = epoll_wait(epoll_fd_, epoll_evts_, max_event_, event_timeout_);
	if ( nfds < 0 ) {
		if ( errno != EAGAIN && errno != EINTR ) {
			setErrMsg("epoll_wait failed!");
			return -1;
		}
	}
	notified_event_idx_ = 0;
	notified_event_cnt_ = nfds;
	return 0;
}


int WSEPoll::GetNextEventFd(int &fd, int &flag) {
	int fd = -1;
	if ( notified_event_idx_ < 0 ||
		 notified_event_idx_ >= notified_event_cnt_ ) {
		return -1;
	}

	getEPollEv(epoll_evts_[notified_event_idx_], fd, flag);
	if ( flag == 0 || fd < 0 ) {
		return -1;
	}

	++notified_event_idx_;
	return 0;
}


int WSEPoll::CloseEPollFd() {
	if ( epoll_fd_ >= 0 ) {
		close(epoll_fd_);
		epoll_fd_ = -1;
	}
	return 0;
}

}
