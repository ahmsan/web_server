/*
 * ws_thread.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_THREAD_H_
#define WS_THREAD_H_

#include <pthread.h>
#include "ws_barrier.h"

#include <vector>
using std::vector;

namespace ws {

class WSThread {
public:
	WSThread(): init_succ_(false), pid_(0),
			p_attr_(PTHREAD_MUTEX_INITIALIZER) {
		init();
	}

	WSThread(pthread_attr_t attr): init_succ_(false),
			pid_(0), p_attr_(attr) {
		init();
	}

	virtual ~WSThread() {
		pthread_attr_destroy(&p_attr_);
	}

	bool Start();

	virtual void Run() = 0;

	void Join();

	inline pthread_t GetThreadID() {
		return this->pid_;
	}

	inline bool IsInitSucc() {
		return init_succ_;
	}

private:
	void init();

private:
	bool			init_succ_;
	pthread_t 		pid_;
	pthread_attr_t 	p_attr_;
    static void *doWorkFunc(void * args);
};


///////////////////////////////////////////////////////////////////////////


class WSWorkThread: public WSThread {
public:
	WSWorkThread(int max_socket_fd_num, vector<WSConnection> &conns_vec,
			uint16_t conns_cnt, WSBarrier &barrier):
		max_socket_fd_num_(max_socket_fd_num), barrier_(barrier),
		conns_vec_(conns_ptr), conns_cnt_(conns_cnt) {
		pipe_read_fd_ = pipe_write_fd_ = -1;
	}

	WSWorkThread(int max_socket_fd_num, vector<WSConnection> &conns_vec,
			uint16_t conns_cnt, WSBarrier &barrier, pthread_attr_t attr):
		max_socket_fd_num_(max_socket_fd_num), barrier_(barrier),
		conns_vec_(conns_vec), conns_cnt_(conns_cnt), WSThread(attr) {
		pipe_read_fd_ = pipe_write_fd_ = -1;
	}

	inline void SetConnInfo(WSConnection *conns_ptr, int conns_cnt) {
		if ( conns_ptr != NULL || conns_cnt > 0 ) {
			conns_ptr_ = conns_ptr;
			conns_cnt_ = conns_cnt;
		}
	}

	void Run();
	void ShutDown();

	inline int GetReadPipeFd() {
		return pipe_fd_[0];
	}

	inline int GetWritePipeFd() {
		return pipe_fd_[1];
	}

private:
	void doIOLoop();
	int  handleNewConn();

	inline bool isConnInfoValid() {
		return conns_vec_.size() > 0;
	}

private:
    int 		pipe_read_fd_;
    int 		pipe_write_fd_;
    int 		max_socket_fd_num_;
    WSEPoll		epoll_;
    WSBarrier&	barrier_;

    vector<WSConnection>& conns_vec_;
};


}


#endif /* WS_THREAD_H_ */
