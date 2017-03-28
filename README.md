# Single-file, cross-platform threads
This library provides a simple interface for cross-platform threads and mutexes without any overhead over using the native operating system APIs.

## Threads

1. Define a function
```
MB_THREAD_HANDLER(handler) {
  // code here...
  return 0;
}
```

2. Spawn a thread
```
MBThread thread;
mb_thread_spawn(&thread, handler, NULL);
```

3. Join with the thread
```
mb_thread_join(&thread);
```

4. Destroy the thread
```
mb_thread_destroy(&thread);
```

## Mutexes

1. Initialize a mutex
```
MBMutex mutex;
mb_mutex_init(&mutex);
```

2. Lock the mutex
```
mb_mutex_lock(&mutex);
```

3. Unlock the mutex
```
mb_mutex_unlock(&mutex);
```

4. Destroy the mutex
```
mb_mutex_destroy(&mutex);
```
