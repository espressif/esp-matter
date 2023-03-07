/*
 * Derived from:
 * http://www.kernel.org/pub/linux/libs/klibc/
 */
/*
 * vsprintf.c
 */

#include <stdio.h>
#include <stdarg.h>

int vsprintf(char *buffer, const char *format, va_list ap)
{
	return vsnprintf(buffer, (~(size_t) 0)>>1, format, ap);
}
