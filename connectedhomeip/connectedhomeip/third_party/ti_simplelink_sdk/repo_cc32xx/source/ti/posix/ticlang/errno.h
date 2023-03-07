/*
 * Copyright (c) 2017-2021 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== errno.h ========
 */

#ifndef ti_posix_ticlang_errno__include
#define ti_posix_ticlang_errno__include

/* compiler vendor check */
#if !defined(__ti_version__) && !defined(__clang__)
#error Incompatible compiler: use this include path (.../ti/posix/ticlang) \
only with a Texas Instruments clang compiler. You appear to be using a \
different compiler.
#endif

/* include toolchain's header file */
#include <../../include/c/errno.h>

/* These are in errno.h but commented out. Values match GNU ARM compiler. */

#ifndef EADDRINUSE
#define EADDRINUSE              112
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL           125
#endif

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT            106
#endif

#ifndef EALREADY
#define EALREADY                120
#endif

#ifndef ECONNABORTED
#define ECONNABORTED            113
#endif

#ifndef ECONNREFUSED
#define ECONNREFUSED            111
#endif

#ifndef ECONNRESET
#define ECONNRESET              104
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ            121
#endif

#ifndef EHOSTUNREACH
#define EHOSTUNREACH            118
#endif

#ifndef EISCONN
#define EISCONN                 127
#endif

#ifndef ENETDOWN
#define ENETDOWN                115
#endif

#ifndef ENETRESET
#define ENETRESET               102
#endif

#ifndef ENETUNREACH
#define ENETUNREACH             114
#endif

#ifndef ENOBUFS
#define ENOBUFS                 105
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT             109
#endif

#ifndef ENOTCONN
#define ENOTCONN                128
#endif

#ifndef ENOTSOCK
#define ENOTSOCK                108
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP              95
#endif

#ifndef EOVERFLOW
#define EOVERFLOW               139
#endif

#ifndef EPROTO
#define EPROTO                  71
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT         123
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE              107
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK             EAGAIN
#endif

/* custom error codes */
#define EFREERTOS    2001       /* FreeRTOS function failure */

#endif /* ti_posix_ticlang_errno__include */
