/*
mb_thread.h - Miles Barr

-----
Setup
-----
*One* source file must contain:
	
	#define MB_THREAD_IMPL
	#include "mb_thread.h"

-----------------
Compiler Switches
-----------------

Disable inlining:
	
	#define MB_THREAD_NO_INLINE

Make all functions static:
	
	#define MB_THREAD_USE_STATIC

Disable mutexes:

	#define MB_THREAD_NO_MUTEX

-------
License
-------

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef MB_THREAD_H
#define MB_THREAD_H
	#ifndef MB_THREAD_NO_INLINE
		#if _MSC_VER && !__INTEL_COMPILER
			#define MB_THREAD_INLINE __inline
		#else
			#define MB_THREAD_INLINE inline
		#endif
	#else
		#define MB_THREAD_INLINE
	#endif

	#ifndef MB_THREAD_USE_STATIC
		#define MB_THREAD_STATIC static
	#else
		#define MB_THREAD_STATIC
	#endif

	#ifdef _WIN32
		#include <windows.h>

		typedef HANDLE MBThread;
		typedef LPVOID MBThreadArg;
		typedef LPTHREAD_START_ROUTINE MBThreadFunc;
		
		#define MB_THREAD_DEF_FUNC(name) DWORD __stdcall name(MBThreadArg arg)

		typedef HANDLE MBMutex;
	#elif defined(_POSIX_SOURCE)
		#include <pthread.h>

		typedef pthread_t MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_DEF_FUNC(name) void *name(MBThreadArg arg)

		typedef pthread_mutex_t MBMutex;
	#else
		#warning "threads are not implemented for the target platform so the code will run sequentially"

		typedef void *MBThread;
		typedef void *MBThreadArg;
		typedef void *(*MBThreadFunc)(MBThreadArg);
		
		#define MB_THREAD_DEF_FUNC(name) void *name(MBThreadArg arg)

		typedef int MBMutex;
	#endif

	MB_THREAD_INLINE MB_THREAD_STATIC unsigned int mb_get_num_cores(void);
	
	MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg);
	MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join(MBThread *thread);
	MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join_multi(size_t n, MBThread *threads);
	MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_destroy(MBThread *thread);

	#ifndef MB_THREAD_NO_MUTEX
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_init(MBMutex *mutex);
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_destroy(MBMutex *mutex);
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock(MBMutex *mutex);
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock_multi(size_t n, MBMutex *mutex);
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_unlock(MBMutex *mutex);
	#endif
#endif

#ifdef MB_THREAD_IMPL
	#ifndef MB_THREAD_ASSERT
		#include <assert.h>
		#define MB_THREAD_ASSERT(cond) assert(cond)
	#endif

	#ifdef _WIN32
		MB_THREAD_INLINE MB_THREAD_STATIC unsigned int mb_get_num_cores(void) {
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			return sysinfo.dwNumberOfProcessors;
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(func != NULL);
			*thread = CreateThread(NULL, 0, func, arg, 0, NULL);
			MB_THREAD_ASSERT(*thread != NULL);
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			DWORD result = WaitForSingleObject(*thread, INFINITE);
			MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
		}
		
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join_multi(size_t n, MBThread *threads) {
			if (n > 0) {
				MB_THREAD_ASSERT(threads != NULL);
				DWORD result = WaitForMultipleObjects((DWORD)n, threads, TRUE, INFINITE);
				MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
			}
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_destroy(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			MB_THREAD_ASSERT(*thread != NULL);
			BOOL result = CloseHandle(*thread);
			MB_THREAD_ASSERT(result != 0);
			#ifndef NDEBUG
				*thread = NULL;
			#endif
		}

		#ifndef MB_THREAD_NO_MUTEX
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_init(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				*mutex = CreateMutex(NULL, FALSE, NULL);
				MB_THREAD_ASSERT(*mutex != NULL);
			}
			
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_destroy(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				MB_THREAD_ASSERT(*mutex != NULL);
				BOOL result = CloseHandle(mutex);
				MB_THREAD_ASSERT(result != 0);
				#ifndef NDEBUG
					*mutex = NULL;
				#endif
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				MB_THREAD_ASSERT(*mutex != NULL);
				DWORD result = WaitForSingleObject(*mutex, INFINITE);
				MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
			}
			
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock_multi(size_t n, MBThread *mutexes) {
				MB_THREAD_ASSERT(mutexes != NULL);
				if (n > 0) {
					DWORD result = WaitForMultipleObjects((DWORD)n, mutexes, TRUE, INFINITE);
					MB_THREAD_ASSERT(result == WAIT_OBJECT_0);
				}
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_unlock(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				MB_THREAD_ASSERT(*mutex != NULL);
				BOOL result = ReleaseMutex(*mutex);
				MB_THREAD_ASSERT(result);
			}
		#endif
	#elif defined(_POSIX_SOURCE)
		MB_THREAD_INLINE MB_THREAD_STATIC unsigned int mb_get_num_cores(void) {
			return sysconf(_SC_NPROCESSORS_ONLN);
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			int result = pthread_create(thread, NULL, func, arg);
			MB_THREAD_ASSERT(result == 0);
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join(MBThread *thread) {
			MB_THREAD_ASSERT(thread != NULL);
			int result = pthread_join(*thread, NULL);
			MB_THREAD_ASSERT(result == 0);
		}
		
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join_multi(size_t n, MBThread *threads) {
			while (n > 0) {
				mb_thread_join(threads);
				threads++;
				n--;
			}
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_destroy(MBThread *thread) {
		}

		#ifndef MB_THREAD_NO_MUTEX
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_init(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				int result = pthread_mutex_init(mutex, NULL);
				MB_THREAD_ASSERT(result == 0);
			}
			
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_destroy(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				int result = pthread_mutex_destroy(mutex);
				MB_THREAD_ASSERT(result == 0);
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				int result = pthread_mutex_lock(mutex);
				MB_THREAD_ASSERT(result == 0);
			}
			
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock_multi(size_t n, MBMutex *mutexes) {
				MB_THREAD_ASSERT(mutexes != NULL);
				while (n > 0) {
					mb_mutex_lock(mutexes);
					mutexes++;
					n--;
				}
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_unlock(MBMutex *mutex) {
				MB_THREAD_ASSERT(mutex != NULL);
				int result = pthread_mutex_unlock(mutex);
				MB_THREAD_ASSERT(result == 0);
			}
		#endif
	#else
		MB_THREAD_INLINE MB_THREAD_STATIC unsigned int mb_get_num_cores(void) {
			return 1;
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_spawn(MBThread *thread, MBThreadFunc func, MBThreadArg arg) {
			MB_THREAD_ASSERT(func != NULL);
			func(arg);
			return NULL;
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join(MBThread *thread) {
		}

		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_join_multi(size_t n, MBThread *threads) {
		}
		
		MB_THREAD_INLINE MB_THREAD_STATIC void mb_thread_destroy(MBThread *thread) {
		}


		#ifndef MB_THREAD_NO_MUTEX
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_init(MBMutex *mutex) {
			}
			
			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_destroy(MBMutex *mutex) {
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock(MBMutex *mutex) {
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_lock_multi(MBMutex *mutex) {
			}

			MB_THREAD_INLINE MB_THREAD_STATIC void mb_mutex_unlock(MBMutex *mutex) {
			}
		#endif
	#endif
#endif