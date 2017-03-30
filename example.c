#include <stdlib.h>
#define MB_THREAD_USE_STATIC
#define MB_THREAD_IMPL
#include "mb_thread.h"

MB_THREAD_DEF_FUNC(worker) {
	// do work...
	return 0;
}

int main() {
	unsigned int thread_count = mb_get_num_cores();
	MBThread *threads = malloc(thread_count * sizeof(MBThread));

	for (int i = 0; i < thread_count; i++) {
		mb_thread_spawn(&threads[i], worker, NULL);
	}

	mb_thread_join_multi(thread_count, threads);

	for (int i = 0; i < thread_count; i++) {
		mb_thread_destroy(&threads[i]);
	}

	free(threads);
	return EXIT_SUCCESS;
}