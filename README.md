# Single-file, cross-platform threads
This library provides a simple interface for cross-platform threads and mutexes without any overhead over using the native operating system APIs.

1. Define a handler
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
