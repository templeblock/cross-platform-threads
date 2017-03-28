#ifndef MB_THREAD_H
#define MB_THREAD_H
	#ifdef _WIN32
		#include <windows.h>

		typedef HANDLE MBThread;
		typedef LPVOID MBThreadArg;
		typedef LPTHREAD_START_ROUTINE MBThreadFunc;
		
		#define MB_THREAD_FUNC(name) DWORD __stdcall name(MBThreadArg arg)
		#define MB_THREAD_RETURN do return 0; while (0)
	#elif defined(_POSIX_SOURCE)
		#include <pthread.h>

		typedef pthread_t MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_FUNC(name) void *name(MBThreadArg arg)
		#define MB_THREAD_RETURN do return NULL; while (0)
	#else
		#warning "threads are not implemented for the target platform so the code will run synchronously"

		typedef void *MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_FUNC(name) void *name(MBThreadArg arg)
		#define MB_THREAD_RETURN do return NULL; while (0)
	#endif

	void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg);
	void mb_thread_join(MBThread *thread);
#endif

#ifdef MB_THREAD_IMPL
	#ifndef MB_THREAD_ASSERT
		#include <assert.h>
		#define MB_THREAD_ASSERT(cond) assert(cond)
	#endif

	#ifdef _WIN32
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			*thread = CreateThread(NULL, 0, func, arg, 0, NULL);
		}

		inline void mb_thread_join(MBThread *thread) {
			DWORD result = WaitForSingleObject(*thread, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
	#elif defined(_POSIX_SOURCE)
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			int result = pthread_create(thread, NULL, func, arg);
			MB_THREAD_ASSERT(result == 0);
		}

		inline	void mb_thread_join(MBThread *thread) {
			int result = pthread_join(*thread, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
	#else
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			func(arg);
			return NULL;
		}

		inline void mb_thread_join(MBThread *thread) {
		}
	#endif
#endif