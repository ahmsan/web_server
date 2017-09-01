/*
 * ws_thread.cpp
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#include <pthread.h>
#include <sys/epoll.h>

#include "ws_thread.h"

namespace ws {


void WSThread::init() {
	init_succ_ = false;
	do {
		if ( pthread_attr_init(&p_attr_) != 0 ) {
			LOG_ERR("pthread_attr_init failed!");
			break;
		}
		if ( pthread_attr_setscope(&p_attr_, PTHREAD_SCOPE_SYSTEM) != 0 ) {
			LOG_ERR("pthread_attr_setscope failed!");
			break;
		}
		if ( pthread_attr_setdetachstate(&p_attr_, PTHREAD_CREATE_JOINABLE) != 0 ) {
			LOG_ERR("pthread_attr_setdetachstate failed!");
			break;
		}
		init_succ_ = true;
	} while ( 0 );
}

void* WSThread::doWorkFunc(void *args) {
	WSThread* thread = static_cast<WSThread *>(args);
	thread->Run();
	return NULL;
}

bool WSThread::Start() {
	bool ret = false;
	if ( this->IsInitSucc() ) {
		false = pthread_create(&pid_, NULL, doWorkFunc, this) == 0;
	}
	return ret;
}

void WSThread::Join() {
	if ( pid_ > 0 ) {
		pthread_join(pid_, NULL);
	}
}


//////////////////////////////////////////////////////////////////


void WSWorkThread::Run() {

	if ( !isConnInfoValid() ) {
		LOG_ERR("conn info doesn't initialize");
		return;
	}

	int pipe_fd[2] = { -1, -1 };
	int ret = ws::create_pipe(pipe_fd, O_NONBLOCK | O_CLOEXEC);
	if ( ret != 0 ) {
		LOG_ERR("create pipe failed!");
		return;
	}

	pipe_read_fd_ = pipe_fd[0];
	pipe_write_fd_ = pipe_fd[1];

	if ( max_socket_fd_num_ > 10000 ) {
		max_socket_fd_num_ = 10000;
	}

	int ret = epoll_.CreateEPoll(max_socket_fd_num_);
	if ( ret < 0 ) {
		LOG_ERR("%s", epoll_.GetLastErrMsg().c_str());
		return;
	}

    // register pipe fd
    // used to receive main_thread's accepted fd.
    ret = epoll_.AddEPollEvent(pipe_read_fd_, EVENT_TYPE_READ);
    if ( ret < 0 ) {
    	LOG_ERR("epoll add(%d) failed, msg: %s", pipe_read_fd_,
    			epoll_.GetLastErrMsg().c_str());
    	return;
    }

    doIOLoop();
}


void WSWorkThread::ShutDown() {
	if ( pipe_read_fd_ >= 0 ) {
		close(pipe_read_fd_);
		pipe_read_fd_ = -1;
	}
	if ( pipe_write_fd_ >= 0 ) {
		close(pipe_write_fd_);
		pipe_write_fd_ = -1;
	}
	epoll_.CloseEPollFd();
}


int WSWorkThread::handleNewConn() {
    int conn_fd = 0;
    // read conn_fd from pipe
    if ( read(pipe_read_fd_, &conn_fd, sizeof(int)) != sizeof(int) ) {
    	LOG_ERR("read conn_id from pipe failed!");
    	return -1;
    }

    if ( conn_fd >= conns_vec_.size() ) {
    	LOG_ERR("conn_fd(%d) is too large", conn_fd);
    	close(conn_fd);
    	return -1;
    }

    conns_vec_[conn_fd].Reset();

    int ret = epoll_.AddEPollEvent(conn_fd, EVENT_TYPE_READ | EVENT_TYPE_ERR);
    if ( ret < 0 ) {
    	LOG_ERR("add event to epoll for conn_fd(%d) failed, err: %s",
    			conn_fd, epoll_.GetLastErrMsg().c_str());
        return -1;
    }

    return 0;
}


void WSWorkThread::doIOLoop() {

	const int max_events = max_socket_fd_num_;

	struct epoll_event *events;
	events = (struct epoll_event *)calloc(max_events * sizeof(struct epoll_event));
	if ( events == NULL ) {
		LOG_ERR("calloc for epoll event failed!");
		return;
	}

	// wait all work theads created done.
	this->barrier_.WaitBarrier();

	while ( true ) {
		// do network_io event loop
		int ret = epoll_.WaitEPollEvents();
		if ( ret < 0 ) {
			usleep(1000);
			continue;
		}

		int fd = 0;
		int flag = 0;
		while ( epoll_.GetNextEventFd(fd, flag) > 0 ) {
			if ( fd  >= conns_vec_.size() ) {
				epoll_.DelEPollEvent(fd);
				close(fd);
				continue;
			}

			if ( fd == pipe_read_fd_ ) {
				// handle new connection: read conn_fd from pipe
				handleNewConn();
				continue;
			}

			WSConnection* conn = &(conns_vec_[fd]);
			conn->SetEPoll(&this->epoll_);

			if ( flag & EVENT_TYPE_ERR ) {
				// handle err event first
				conn->Close();
				continue;
			}
			if ( flag & EVENT_TYPE_READ ) {
				// handle event read
				conn->HandleRead();
			}
			if ( flag & EVENT_TYPE_WRITE ) {
				// handle event write
				conn->HandleWrite();
			}
		}
	}

	ShutDown();
	free(events);
	return;
}


}


