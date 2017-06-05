#include <assert.h>
#include "mutex.h"

#ifdef _WIN32
	void mutex_init(Mutex *mutex) {
		assert(mutex != NULL);
		*mutex = CreateMutex(NULL, FALSE, NULL);
		assert(*mutex != NULL);
	}
	
	void mutex_destroy(Mutex *mutex) {
		assert(mutex != NULL);
		assert(*mutex != NULL);
		BOOL result = CloseHandle(mutex);
		assert(result != 0);
		
		#ifndef NDEBUG
			*mutex = NULL;
		#endif
	}

	void mutex_lock(Mutex *mutex) {
		assert(mutex != NULL);
		assert(*mutex != NULL);
		DWORD result = WaitForSingleObject(*mutex, INFINITE);
		assert(result == WAIT_OBJECT_0);
	}
	
	void mutex_lock_multi(Mutex *mutexes, unsigned int mutex_count) {
		if (mutex_count > 0) {
			assert(mutexes != NULL);
			DWORD result = WaitForMultipleObjects((DWORD)mutex_count, mutexes, TRUE, INFINITE);
			assert(result == WAIT_OBJECT_0);
		}
	}

	void mutex_unlock(Mutex *mutex) {
		assert(mutex != NULL);
		assert(*mutex != NULL);
		BOOL result = ReleaseMutex(*mutex);
		assert(result);
	}
#elif defined(_POSIX_SOURCE)
	void mutex_init(Mutex *mutex) {
		assert(mutex != NULL);
		int result = pthread_mutex_init(mutex, NULL);
		assert(result == 0);
	}
	
	void mutex_destroy(Mutex *mutex) {
		assert(mutex != NULL);
		int result = pthread_mutex_destroy(mutex);
		assert(result == 0);
	}

	void mutex_lock(Mutex *mutex) {
		assert(mutex != NULL);
		int result = pthread_mutex_lock(mutex);
		assert(result == 0);
	}
	
	void mutex_lock_multi(Mutex *mutexes, unsigned int mutex_count) {
		while (n > 0) {
			assert(mutexes != NULL);
			mutex_lock(mutexes);
			mutexes++;
			n--;
		}
	}

	void mutex_unlock(Mutex *mutex) {
		assert(mutex != NULL);
		int result = pthread_mutex_unlock(mutex);
		assert(result == 0);
	}
#else
	void mutex_init(Mutex *mutex) {}
	void mutex_destroy(Mutex *mutex) {}
	void mutex_lock(Mutex *mutex) {}
	void mutex_lock_multi(Mutex *mutexes, unsigned int mutex_count) {}
	void mutex_unlock(Mutex *mutex) {}
#endif