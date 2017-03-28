#ifndef MB_THREAD_H
#define MB_THREAD_H
	#ifdef _WIN32
		#include <windows.h>

		typedef HANDLE MBThread;
		typedef LPVOID MBThreadArg;
		typedef LPTHREAD_START_ROUTINE MBThreadFunc;
		
		#define MB_THREAD_HANDLER(name) DWORD __stdcall name(MBThreadArg arg)
	#elif defined(_POSIX_SOURCE)
		#include <pthread.h>

		typedef pthread_t MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_HANDLER(name) void *name(MBThreadArg arg)
	#else
		#warning "threads are not implemented for the target platform so the code will run synchronously"

		typedef void *MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_HANDLER(name) void *name(MBThreadArg arg)
	#endif

	void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg);
	void mb_thread_join(MBThread *thread);
#endif

#ifdef MB_THREAD_IMPL
	#ifndef MB_THREAD_ASSERT
		#include <assert.h>
		#define MB_THREAD_ASSERT(cond) assert(cond)
	#endif

	#ifdef _WIN32
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg) {
			MB_THREAD_ASSERT(handler != NULL);
			*thread = CreateThread(NULL, 0, handler, arg, 0, NULL);
		}

		inline void mb_thread_join(MBThread *thread) {
			DWORD result = WaitForSingleObject(*thread, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
	#elif defined(_POSIX_SOURCE)
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg) {
			MB_THREAD_ASSERT(handler != NULL);
			int result = pthread_create(thread, NULL, handler, arg);
			MB_THREAD_ASSERT(result == 0);
		}

		inline	void mb_thread_join(MBThread *thread) {
			int result = pthread_join(*thread, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
	#else
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg) {
			MB_THREAD_ASSERT(handler != NULL);
			handler(arg);
			return NULL;
		}

		inline void mb_thread_join(MBThread *thread) {
		}
	#endif
#endif