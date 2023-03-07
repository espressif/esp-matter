/*
 * wpa_supplicant/hostapd - Default include files
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This header file is included into all C files so that commonly used header
 * files can be selected with OS specific ifdef blocks in one place instead of
 * having to have OS/C library specific selection in many files.
 */

#ifndef INCLUDES_H
#define INCLUDES_H

/* FreeRTOS porting */
#include "memory_attribute.h"
/* Include possible build time configuration before including anything else */
#include "build_config.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#ifndef _WIN32_WCE
#include <signal.h>

#if (!defined(__CC_ARM)) && (!defined(__ICCARM__))
#include <sys/types.h>
#endif
#include <errno.h>
#endif /* _WIN32_WCE */
#include <ctype.h>

#ifndef _MSC_VER
#if (!defined(__CC_ARM)) && (!defined(__ICCARM__))
#include <unistd.h>
#endif
#endif /* _MSC_VER */

#ifndef CONFIG_NATIVE_WINDOWS
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include "lwip/sockets.h"
#include "lwip/inet.h"

/* rtos porting */
/* TODO: lwip did not configure LWIP_IPV6 */
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

#undef snprintf
#define snprintf snprintf

#ifndef __vxworks
//#include <sys/uio.h>
//#include <sys/time.h>
#endif /* __vxworks */
#endif /* CONFIG_NATIVE_WINDOWS */

#define abort()

#if defined(__CC_ARM) || defined(__ICCARM__)
typedef unsigned short gid_t;
#define SIGALRM (14)
#define SIGHUP (1)
#if 0
//use arch.h define
#define EINTR (4)
#define ENOENT (2)
#endif
#include <time.h>
#endif
#endif /* INCLUDES_H */
