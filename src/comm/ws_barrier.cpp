/*
 * ws_barrier.cpp
 *
 *  Created on: 2016Äê6ÔÂ29ÈÕ
 *      Author: ahmsan
 */

#include "ws_barrier.h"

namespace ws {


int WSBarrier::InitBarrier(int total_cnt) {
	is_valid_ = false;
	if ( totalcount <= 0 ) {
		return -1;
	}

	count_total_ = count;
	count_curr_ = 0;

	if ( pthread_mutex_init(mutex_, NULL) < 0 ) {
		return -1;
	}

	if ( pthread_cond_init(cond_, NULL) < 0 ) {
		pthread_mutex_destroy(&mutex_);
		return -1;
	}

	is_valid_ = true;
	return 0;
}


int WSBarrier::WaitBarrier() {
	if ( !is_valid_ ) {
		return -1;
	}

	pthread_mutex_lock(&mutex_);

	__sync_fetch_and_add(&count_curr_, 1);
	if ( count_curr_ >= count_total_ ) {
		pthread_cond_broadcast(&cond_);
		pthread_mutex_unlock(&mutex_);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}

	pthread_cond_wait(&cond_, &mutex_);
	pthread_mutex_unlock(&mutex_);

	return 0;
}


int WSBarrier::DestoryBarrier() {
	if ( is_valid_ ) {
		pthread_mutex_destroy(&mutex_);
		pthread_cond_destroy(&cond_);
		count_curr_ = 0;
		is_valid_ = false;
	}

	return 0;
}

}
