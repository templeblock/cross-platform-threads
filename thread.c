#include <assert.h>
#include "thread.h"

#ifdef _WIN32
	unsigned int get_num_cores(void) {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
	}

	void thread_spawn(Thread *thread, ThreadHandler handler, ThreadArg arg) {
		assert(thread != NULL);
		assert(handler != NULL);
		*thread = CreateThread(NULL, 0, handler, arg, 0, NULL);
		assert(*thread != NULL);
	}

	void thread_join(Thread *thread) {
		assert(thread != NULL);
		assert(*thread != NULL);
		DWORD result = WaitForSingleObject(*thread, INFINITE);
		assert(result == WAIT_OBJECT_0);
	}
	
	void thread_join_multi(Thread *threads, unsigned int thread_count) {
		if (thread_count > 0) {
			assert(threads != NULL);
			DWORD result = WaitForMultipleObjects((DWORD)thread_count, threads, TRUE, INFINITE);
			assert(result == WAIT_OBJECT_0);
		}
	}

	void thread_destroy(Thread *thread) {
		assert(thread != NULL);
		assert(*thread != NULL);
		BOOL result = CloseHandle(*thread);
		assert(result != 0);
		
		#ifndef NDEBUG
			*thread = NULL;
		#endif
	}
#elif defined(_POSIX_SOURCE)
	unsigned int get_num_cores(void) {
		return sysconf(_SC_NPROCESSORS_ONLN);
	}

	void thread_spawn(Thread *thread, ThreadHandler handler, ThreadArg arg) {
		assert(handler != NULL);
		int result = pthread_create(thread, NULL, handler, arg);
		assert(result == 0);
	}

	void thread_join(Thread *thread) {
		assert(thread != NULL);
		int result = pthread_join(*thread, NULL);
		assert(result == 0);
	}
	
	void thread_join_multi(Thread *threads, unsigned int thread_count) {
		while (thread_count > 0) {
			assert(threads != NULL);
			thread_join(threads);
			threads++;
			thread_count--;
		}
	}

	void thread_destroy(Thread *thread) {
		assert(thread != NULL);
	}
#else
	unsigned int get_num_cores(void) {
		return 1;
	}

	void thread_spawn(Thread *thread, ThreadHandler handler, ThreadArg arg) {
		assert(handler != NULL);
		handler(arg);
		return NULL;
	}
	
	void thread_join(Thread *thread) {}
	void thread_join_multi(Thread *threads, unsigned int thread_count) {}
	void thread_destroy(Thread *thread) {}
#endif