/*
 * stdio.h
 */

#ifndef _STDIO_H
#define _STDIO_H

#include <extern.h>
#include <stdarg.h>
#include <stddef.h>

/* The actual IO functions are not included. */

__extern int sprintf(char *, const char *, ...);
__extern int vsprintf(char *, const char *, va_list);
__extern int snprintf(char *, size_t n, const char *, ...);
__extern int vsnprintf(char *, size_t n, const char *, va_list);
__extern int sscanf(const char *, const char *, ...);
__extern int vsscanf(const char *, const char *, va_list);

#endif				/* _STDIO_H */
