#ifndef THREAD_H
#define THREAD_H

#ifdef _WIN32
	#include <windows.h>

	typedef HANDLE Thread;
	typedef LPVOID ThreadArg;
	typedef LPTHREAD_START_ROUTINE ThreadHandler;
	
	#define THREAD_DEF_HANDLER(name) DWORD __stdcall name(ThreadArg arg)
#elif defined(_POSIX_SOURCE)
	#include <pthread.h>

	typedef pthread_t Thread;
	typedef void *ThreadArg;
	typedef void *(*ThreadHandler)(ThreadArg);
	
	#define THREAD_DEF_HANDLER(name) void *name(ThreadArg arg)
#else
	#warning Threads are not implemented for the target platform so the code will be run sequentially

	typedef void *Thread;
	typedef void *ThreadArg;
	typedef void *(*ThreadHandler)(ThreadArg);
	
	#define THREAD_DEF_HANDLER(name) void *name(ThreadArg arg)
#endif

unsigned int thread_get_num_cores(void);
void thread_spawn(Thread *thread, ThreadHandler handler, ThreadArg arg);
void thread_join(Thread *thread);
void thread_join_multi(Thread *threads, unsigned int thread_count);
void thread_destroy(Thread *thread);

#endif