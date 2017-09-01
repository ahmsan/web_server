/*
 * ws_barrier.h
 *
 *  Created on: 2016Äê6ÔÂ29ÈÕ
 *      Author: ahmsan
 */

#ifndef WS_BARRIER_H_
#define WS_BARRIER_H_

#include <pthread.h>
#include "uncopyable.h"

namespace ws {

class WSBarrier: private UnCopyable {
public:
	WSBarrier(): is_valid_(false),
		count_total_(0), count_curr_(0) {
	}

	int InitBarrier(int total_cnt);
	int DestoryBarrier();
	int WaitBarrier();

private:
	bool	is_valid_;
	int 	count_total_;
	int		count_curr_;
	pthread_mutex_t	mutex_;
	pthread_cond_t	cond_;
} ;

}


#endif /* WS_BARRIER_H_ */
