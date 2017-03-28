#define MB_THREAD_IMPL
#include "mb_thread.h"

MB_THREAD_FUNC(worker) {
	// do work...
	return 0;
}

int main() {
	MBThread threads[16];

	for (int i = 0; i < 16; i++) {
		mb_thread_spawn(&threads[i], worker, NULL);
	}

	for (int i = 0; i < 16; i++) {
		mb_thread_join(&threads[i]);
		mb_thread_destroy(&threads[i]);
	}

	return 0;
}