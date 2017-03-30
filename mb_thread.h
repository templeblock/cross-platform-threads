#ifndef MB_THREAD_H
#define MB_THREAD_H
	#if _MSC_VER && !__INTEL_COMPILER
		#define MB_THREAD_INLINE __inline
	#else
		#define MB_THREAD_INLINE inline
	#endif

	#ifdef _WIN32
		#include <windows.h>

		typedef HANDLE MBThread;
		typedef LPVOID MBThreadArg;
		typedef LPTHREAD_START_ROUTINE MBThreadFunc;
		
		#define MB_THREAD_FUNC(name) DWORD __stdcall name(MBThreadArg arg)

		typedef HANDLE MBMutex;
	#elif defined(_POSIX_SOURCE)
		#include <pthread.h>

		typedef pthread_t MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_FUNC(name) void *name(MBThreadArg arg)

		typedef pthread_mutex_t MBMutex;
	#else
		#warning "threads are not implemented for the target platform so the code will run sequentially"

		typedef void *MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_FUNC(name) void *name(MBThreadArg arg)

		typedef int MBMutex;
	#endif

	unsigned int mb_get_num_cores(void);
	
	void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg);
	void mb_thread_join(MBThread *thread);
	void mb_thread_join_multi(size_t n, MBThread *threads);
	void mb_thread_destroy(MBThread *thread);

	void mb_mutex_init(MBMutex *mutex);
	void mb_mutex_destroy(MBMutex *mutex);
	void mb_mutex_lock(MBMutex *mutex);
	void mb_mutex_lock_multi(size_t n, MBMutex *mutex);
	void mb_mutex_unlock(MBMutex *mutex);
#endif

#ifdef MB_THREAD_IMPL
	#ifndef MB_THREAD_ASSERT
		#include <assert.h>
		#define MB_THREAD_ASSERT(cond) assert(cond)
	#endif

	#ifdef _WIN32
		MB_THREAD_INLINE unsigned int mb_get_num_cores(void) {
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			return sysinfo.dwNumberOfProcessors;
		}

		MB_THREAD_INLINE void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(func != NULL);
			*thread = CreateThread(NULL, 0, func, arg, 0, NULL);
			MB_THREAD_ASSERT(*thread != NULL);
		}

		MB_THREAD_INLINE void mb_thread_join(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			DWORD result = WaitForSingleObject(*thread, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
		
		MB_THREAD_INLINE void mb_thread_join_multi(size_t n, MBThread *threads) {
			if (n > 0) {
				MB_THREAD_ASSERT(threads != NULL);
				DWORD result = WaitForMultipleObjects((DWORD)n, threads, TRUE, INFINITE);
				MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
			}
		}

		MB_THREAD_INLINE void mb_thread_destroy(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			BOOL result = CloseHandle(*thread);
			MB_THREAD_ASSERT(result != 0);
			#ifndef NDEBUG
				*thread = NULL;
			#endif
		}

		MB_THREAD_INLINE void mb_mutex_init(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			*mutex = CreateMutex(NULL, FALSE, NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
		}
		
		MB_THREAD_INLINE void mb_mutex_destroy(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			BOOL result = CloseHandle(mutex);
			MB_THREAD_ASSERT(result != 0);
			#ifndef NDEBUG
				*mutex = NULL;
			#endif
		}

		MB_THREAD_INLINE void mb_mutex_lock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			DWORD result = WaitForSingleObject(*mutex, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
		
		MB_THREAD_INLINE void mb_mutex_lock_multi(size_t n, MBThread *mutexes) {
			MB_THREAD_ASSERT(mutexes != NULL);
			if (n > 0) {
				DWORD result = WaitForMultipleObjects((DWORD)n, mutexes, TRUE, INFINITE);
				MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
			}
		}

		MB_THREAD_INLINE void mb_mutex_unlock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			MB_THREAD_ASSERT(*mutex != NULL);
			BOOL result = ReleaseMutex(*mutex);
			MB_THREAD_ASSERT(result);
		}
	#elif defined(_POSIX_SOURCE)
		MB_THREAD_INLINE unsigned int mb_get_num_cores(void) {
			return sysconf(_SC_NPROCESSORS_ONLN);
		}

		MB_THREAD_INLINE void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			int result = pthread_create(thread, NULL, func, arg);
			MB_THREAD_ASSERT(result == 0);
		}

		MB_THREAD_INLINE	void mb_thread_join(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			int result = pthread_join(*thread, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
		
		MB_THREAD_INLINE void mb_thread_join_multi(size_t n, MBThread *threads) {
			while (n > 0) {
				mb_thread_join(threads);
				threads++;
				n--;
			}
		}

		MB_THREAD_INLINE void mb_thread_destroy(MBThread *thread) {
		}

		MB_THREAD_INLINE void mb_mutex_init(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_init(mutex, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
		
		MB_THREAD_INLINE void mb_mutex_destroy(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_destroy(mutex);
			MB_THREAD_ASSERT(result == 0);
		}

		MB_THREAD_INLINE void mb_mutex_lock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_lock(mutex);
			MB_THREAD_ASSERT(result == 0);
		}
		
		MB_THREAD_INLINE void mb_mutex_lock_multi(size_t n, MBMutex *mutexes) {
			MB_THREAD_ASSERT(mutexes != NULL);
			while (n > 0) {
				mb_mutex_lock(mutexes);
				mutexes++;
				n--;
			}
		}

		MB_THREAD_INLINE void mb_mutex_unlock(MBMutex *mutex) {
			MB_THREAD_ASSERT(mutex != NULL);
			int result = pthread_mutex_unlock(mutex);
			MB_THREAD_ASSERT(result == 0);
		}
	#else
		MB_THREAD_INLINE unsigned int mb_get_num_cores(void) {
			return 1;
		}

		MB_THREAD_INLINE void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			func(arg);
			return NULL;
		}

		MB_THREAD_INLINE void mb_thread_join(MBThread *thread) {
		}

		MB_THREAD_INLINE void mb_thread_join_multi(size_t n, MBThread *threads) {
		}
		
		MB_THREAD_INLINE void mb_thread_destroy(MBThread *thread) {
		}

		MB_THREAD_INLINE void mb_mutex_init(MBMutex *mutex) {
		}
		
		MB_THREAD_INLINE void mb_mutex_destroy(MBMutex *mutex) {
		}

		MB_THREAD_INLINE void mb_mutex_lock(MBMutex *mutex) {
		}

		MB_THREAD_INLINE void mb_mutex_lock_multi(MBMutex *mutex) {
		}

		MB_THREAD_INLINE void mb_mutex_unlock(MBMutex *mutex) {
		}
	#endif
#endif