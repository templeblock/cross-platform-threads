#ifndef MUTEX_H
#define MUTEX_H

#ifdef _WIN32
	#include <windows.h>
	typedef HANDLE Mutex;
#elif defined(_POSIX_SOURCE)
	#include <pthread.h>
	typedef pthread_mutex_t Mutex;
#else
	#warning Mutexes are not implemented for the target platform
	typedef int Mutex;
#endif

void mutex_init(Mutex *mutex);
void mutex_destroy(Mutex *mutex);
void mutex_lock(Mutex *mutex);
void mutex_lock_multi(Mutex *mutexes, unsigned int mutex_count);
void mutex_unlock(Mutex *mutex);

#endif