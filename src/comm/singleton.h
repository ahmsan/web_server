/*
 * singleton.h
 *
 *  Created on: 2016Äê6ÔÂ30ÈÕ
 *      Author: ahmsan
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <pthread.h>
#include "uncopyable.h"


namespace ws {


template <typename T>
class Singleton: private UnCopyable {
public:
	static T Instance() {
		pthread_once(&ponce_, &Singleton::Init);
		return value_;
	}

	static void Init() {
		value_ = new T();
		 ::atexit(Destroy);
	}

	static void Destroy() {
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_be_complete_type dummy; (void) dummy;
		delete value_;
	}

private:
	static pthread_once_t ponce_;
	static T* value_;
};

template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = NULL;

}


#endif /* SINGLETON_H_ */
