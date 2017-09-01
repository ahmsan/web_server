/*
 * ws_mutex_lock.h
 *
 *  Created on: 2016Äê6ÔÂ28ÈÕ
 *      Author: mingsanwang
 */

#ifndef WS_MUTEX_LOCK_H_
#define WS_MUTEX_LOCK_H_

#include <pthread.h>

#include "uncopyable.h"

namespace ws {

class MutexLock: private UnCopyable {
public:
    MutexLock() {
        pthread_mutex_init(&mutex_, NULL);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&mutex_);
    }

    void Lock() {
        pthread_mutex_lock(&mutex_);
    }

    void Unlock() {
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* GetMutex() {
    	return &mutex_;
    }

private:
    pthread_mutex_t mutex_;
};



class MutexLockGuard: private UnCopyable {
public:
    explicit MutexLockGuard(MutexLock &mutex): mutex_lock_(mutex) {
    	mutex_lock_.Lock();
    }

    ~MutexLockGuard() {
    	mutex_lock_.Unlock();
    }

private:
    MutexLock& mutex_lock_;
};


class Condition: private UnCopyable {
public:
	Condition(MutexLock& mutex): mutex_lock_(mutex) {
		pthread_cond_init(&pcond_, NULL);
	}

	~Condition() {
		pthread_cond_destroy(&pcond_);
	}

	void Wait() {
		pthread_cond_wait(&pcond_, mutex_lock_.GetMutex());
	}

	void Notify() {
		pthread_cond_signal(&pcond_);
	}

	void NotifyAll() {
		pthread_cond_broadcast(&pcond_);
	}

private:
	MutexLock& mutex_lock_;
	pthread_cond_t pcond_;
};




//¡¡prevent misuse like£º
//¡¡MutexLockGuard(mutex)
//¡¡a tmp object doesn't hold mutex for long
#define MutexLockGuard(x) error "missing guard object name"

}

#endif /* WS_MUTEX_LOCK_H_ */
