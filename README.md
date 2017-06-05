# Cross-Platform Threads
Cross-platform threads and mutexes without any overhead.

## Installation

Copy this library into a project and include the header files.
```c
#include "cross-platform-threads/thread.h"
#include "cross-platform-threads/mutex.h"
```

## Usage

### Threads

1. Define a thread handler
```c
DEF_THREAD_HANDLER(worker) {
  // Code here...
  return 0;
}
```

2. Spawn a thread
```c
Thread thread;
thread_spawn(&thread, worker, NULL);
```

3. Join with the thread
```c
thread_join(&thread);
```

4. Destroy the thread
```
threadc_destroy(&thread);
```

### Mutexes

1. Initialize a mutex
```c
Mutex mutex;
mutex_init(&mutex);
```

2. Lock the mutex
```c
mutex_lock(&mutex);
```

3. Unlock the mutex
```c
mutex_unlock(&mutex);
```

4. Destroy the mutex
```c
mutex_destroy(&mutex);
```

## Example

The following is an example of how this library could be used in a small program.
```c
#include <stdlib.h>
#include "cross-platform-threads/thread.h"

DEF_THREAD_HANDLER(worker) {
	// Do work...
	return 0;
}

int main() {
	unsigned int thread_count = get_num_cores();
	Thread *threads = malloc(thread_count * sizeof(Thread));

	for (unsigned int i = 0; i < thread_count; i++) {
		thread_spawn(&threads[i], worker, NULL);
	}

	thread_join_multi(threads, thread_count);

	for (unsigned int i = 0; i < thread_count; i++) {
		thread_destroy(&threads[i]);
	}

	free(threads);
	return EXIT_SUCCESS;
}
```