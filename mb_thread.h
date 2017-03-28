#ifndef MB_THREAD_H
#define MB_THREAD_H
	#ifdef _WIN32
		#include <windows.h>

		typedef HANDLE MBThread;
		typedef LPVOID MBThreadArg;
		typedef LPTHREAD_START_ROUTINE MBThreadFunc;
		
		#define MB_THREAD_HANDLER(name) DWORD __stdcall name(MBThreadArg arg)

		typedef HANDLE MBMutex;
	#elif defined(_POSIX_SOURCE)
		#include <pthread.h>

		typedef pthread_t MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_HANDLER(name) void *name(MBThreadArg arg)

		typedef pthread_mutex_t MBMutex;
	#else
		#warning "threads are not implemented for the target platform so the code will run sequentially"

		typedef void *MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_HANDLER(name) void *name(MBThreadArg arg)

		typedef int MBMutex;
	#endif

	void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg);
	void mb_thread_join(MBThread *thread);
	void mb_thread_destroy(MBThread *thread);

	void mb_mutex_init(MBMutex *mutex);
	void mb_mutex_destroy(MBMutex *mutex);
	void mb_mutex_lock(MBMutex *mutex);
	void mb_mutex_unlock(MBMutex *mutex);
#endif

#ifdef MB_THREAD_IMPL
	#ifndef MB_THREAD_ASSERT
		#include <assert.h>
		#define MB_THREAD_ASSERT(cond) assert(cond)
	#endif

	#ifdef _WIN32
		inline void mb_thread_spawn(MBThread *thread, MBThreadFunc handler, MBThreadArg arg) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(handler != NULL);
			*thread = CreateThread(NULL, 0, handler, arg, 0, NULL);
			MB_THREAD_ASSERT(*thread != NULL);
		}

		inline void mb_thread_join(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			DWORD result = WaitForSingleObject(*thread, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
		
		inline void mb_thread_destroy(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			BOOL result = CloseHandle(*thread);
			MB_THREAD_ASSERT(result != 0);
			#ifndef NDEBUG
				*thread = NULL;
			#endif
		}

		inline void mb_mutex_init(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			*mutex = CreateMutex(NULL, FALSE, NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
		}
		
		inline void mb_mutex_destroy(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			BOOL result = CloseHandle(mutex);
			MB_THREAD_ASSERT(result != 0);
			#ifndef NDEBUG
				*mutex = NULL;
			#endif
		}

		inline void mb_mutex_lock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			DWORD result = WaitForSingleObject(*mutex, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}

		inline void mb_mutex_unlock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			BOOL result = ReleaseMutex(*mutex);
			MB_THREAD_ASSERT(result);
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

		inline void mb_thread_destroy(MBThread *thread) {
		}

		inline void mb_mutex_init(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_init(mutex, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
		
		inline void mb_mutex_destroy(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_destroy(mutex);
			MB_THREAD_ASSERT(result == 0);
		}

		inline void mb_mutex_lock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_lock(mutex);
			MB_THREAD_ASSERT(result == 0);
		}

		inline void mb_mutex_unlock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_unlock(mutex);
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
		
		inline void mb_thread_destroy(MBThread *thread) {
		}

		inline void mb_mutex_init(MBMutex *mutex) {
		}
		
		inline void mb_mutex_destroy(MBMutex *mutex) {
		}

		inline void mb_mutex_lock(MBMutex *mutex) {
		}

		inline void mb_mutex_unlock(MBMutex *mutex) {
		}
	#endif
#endif