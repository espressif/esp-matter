/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/
/*******************************************************************************
 * xf-trace.c
 *
 * Tracing facility
 ******************************************************************************/

#include "xos-msgq-if.h"
#include "xf.h"
#include <sys/time.h>

#if XF_TRACE
/*******************************************************************************
 * Local data definitions
 ******************************************************************************/

/* ...tracing lock */
static xf_lock_t  xf_trace_mutex;

/*******************************************************************************
 * Tracing facility
 ******************************************************************************/

/* ...timestamp function */
static UWORD32 xf_timenow(void)
{
    struct timeval          tv;

    /* ...get current time value */
    gettimeofday(&tv, NULL);

    /* ...wrap over every 100 seconds */
    return (UWORD32)((tv.tv_sec % 100) * 1000000 + tv.tv_usec);
}

/* ...tracing initialization */
void xf_trace_init(const char *banner)
{
    /* ...initialize tracing lock */
    __xf_lock_init(&xf_trace_mutex);

    /* ...output banner */
    xf_trace(banner);
}

/* ...tracing primitive */
int xf_trace(const char *format, ...)
{
    va_list     args;
    static char buf[256];
    char       *b = buf;
    
    /* ...get global tracing lock */
    __xf_lock(&xf_trace_mutex);

    /* ...output timestamp */
    b += sprintf(b, "[%08u] ", xf_timenow());

    /* ...output format string */
    va_start(args, format);
    b += vsprintf(b, format, args);
    va_end(args);

    /* ...put terminator */
    *b = '\0';

    /* ...output prepared string */
    __xf_puts(buf);
    
    /* ...release tracing lock */
    __xf_unlock(&xf_trace_mutex);

    return 0;
}

#endif  /* XF_TRACE */
