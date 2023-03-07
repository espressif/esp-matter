/*
 * memset.c
 */

#include <string.h>
#include <stdint.h>

void *memset(void *dst, int c, size_t n)
{
	char *q = dst;
	while (n--) {
		*q++ = c;
	}
	return dst;
}
