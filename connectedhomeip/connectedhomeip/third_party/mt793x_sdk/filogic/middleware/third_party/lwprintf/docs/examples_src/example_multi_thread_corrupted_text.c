#include "lwprintf/lwprintf.h"

/* Assuming LwPRINTF has been initialized before */

void
task_1(void* arg) {
	lwprintf_printf("Hello world\r\n");
}

void
task_2(void* arg) {
	lwprintf_printf("This is Task 2\r\n");
}

/*
 * If thread safety is not enabled,
 * running above example may print:
 *
 * "Hello This is Task 2\r\nworld\r\n"
 */
