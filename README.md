# Cross-Platform Threads
Cross-platform threads and mutexes without any overhead.

## Installation

Copy this library into a project and include the header files.
```c
#include "thread.h"
#include "mutex.h"
```

## Usage

### Threads

1. Define a thread handler
```c
THREAD_DEF_HANDLER(worker) {
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
```c
thread_destroy(&thread);
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

The following is an example of a small program using this library.
```c
#include <stdlib.h>
#include "thread.h"

DEF_THREAD_HANDLER(worker) {
	// Do work...
	return 0;
}

int main() {
	unsigned int thread_count = thread_get_num_cores();
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