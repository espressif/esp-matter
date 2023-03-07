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
 * xf-debug.h
 *
 * Debugging interface for Xtensa Audio DSP codec server
 *******************************************************************************/

#include <stdlib.h>

/*******************************************************************************
 * Auxiliary macros (put into "xf-types.h"?)
 ******************************************************************************/

#ifndef offset_of
#define offset_of(type, member)         \
    ((int)(intptr_t)&(((const type *)(0))->member))
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((void *)(ptr) - offset_of(type, member)))
#endif 

/*******************************************************************************
 * Bug check for constant conditions (file scope)
 ******************************************************************************/

#define __C_BUG(n)      __C_BUG2(n)
#define __C_BUG2(n)     __c_bug_##n
#define C_BUG(expr)     typedef char __C_BUG(__LINE__)[(expr) ? -1 : 1]

/*******************************************************************************
 * Compilation-time types control
 ******************************************************************************/

#if XF_DEBUG
#define __C_TYPE_CONTROL(d, type)       ((void) ((d) != (type*) 0))
#else
#define __C_TYPE_CONTROL(d, type)       ((void) 0)
#endif

/*******************************************************************************
 * Unused variable
 ******************************************************************************/

#define C_UNUSED(v)                     (void)(0 ? (v) = (v), 1 : 0)

/*******************************************************************************
 * Auxiliary macros
 ******************************************************************************/

/* ...define a stub for unused declarator */
#define __xf_stub(tag, line)            __xf_stub2(tag, line)
#define __xf_stub2(tag, line)           typedef int __xf_##tag##_##line

/* ...convert anything into string */
#define __xf_string(x)                  __xf_string2(x)
#define __xf_string2(x)                 #x

/*******************************************************************************
 * Tracing facility
 ******************************************************************************/

#if XF_TRACE

/* ...tracing to communication processor */
extern int  xf_trace(const char *format, ...);

/* ...tracing facility initialization */
extern void xf_trace_init(const char *banner);

/* ...initialize tracing facility */
#define TRACE_INIT(banner)              (xf_trace_init(banner))

/* ...trace tag definition */
#define TRACE_TAG(tag, on)              enum { __xf_trace_##tag = on }

/* ...check if the trace tag is enabled */
#define TRACE_CFG(tag)                  (__xf_trace_##tag)

/* ...tagged tracing primitive */
#define TRACE(tag, fmt, ...)            (void)(__xf_trace_##tag ? __xf_trace(tag, __xf_format##fmt, ## __VA_ARGS__), 1 : 0)

/*******************************************************************************
 * Tagged tracing formats
 ******************************************************************************/

/* ...tracing primitive */
#define __xf_trace(tag, fmt, ...)       \
    ({ __attribute__((unused)) const char *__xf_tag = #tag; xf_trace(fmt, ## __VA_ARGS__); })

/* ...just a format string */
#define __xf_format_n(fmt)              fmt

/* ...module tag and trace tag shown */
#define __xf_format_b(fmt)              "[%s.%s] " fmt, __xf_string(MODULE_TAG), __xf_tag

/* ...module tag, trace tag, file name and line shown */
#define __xf_format_x(fmt)              "[%s.%s] - %s@%d - " fmt,  __xf_string(MODULE_TAG), __xf_tag, __FILE__, __LINE__

/*******************************************************************************
 * Globally defined tags
 ******************************************************************************/

/* ...unconditionally OFF */
TRACE_TAG(0, 0);

/* ...unconditionally ON */
TRACE_TAG(1, 1);

/* ...error output - on by default */
TRACE_TAG(ERROR, 1);

#else

#define TRACE_INIT(banner)              (void)0
#define TRACE_TAG(tag, on)              __xf_stub(trace_##tag, __LINE__)
#define TRACE_CFG(tag)			0
#define TRACE(tag, fmt, ...)            (void)0
#define __xf_trace(tag, fmt, ...)       (void)0

#endif  /* XF_TRACE */

/*******************************************************************************
 * Bugchecks
 ******************************************************************************/

#if XF_DEBUG

/* ...run-time bugcheck */
#define BUG(cond, fmt, ...)                                     \
do                                                              \
{                                                               \
    if (cond)                                                   \
    {                                                           \
        /* ...output message */                                 \
        __xf_trace(BUG, __xf_format##fmt, ## __VA_ARGS__);      \
                                                                \
        /* ...and die  */                                       \
        abort();                                                \
    }                                                           \
}                                                               \
while (0)

#else
#define BUG(cond, fmt, ...)             (void)0
#endif  /* XF_DEBUG */

/*******************************************************************************
 * Run-time error processing
 ******************************************************************************/

/* ...check the API call succeeds */
#define XF_CHK_API(cond)                                \
({                                                      \
    int __ret;                                          \
                                                        \
    if ((__ret = (int)(cond)) < 0)                      \
    {                                                   \
        TRACE(ERROR, _x("API error: %d"), __ret);       \
        return __ret;                                   \
    }                                                   \
    __ret;                                              \
})

/* ...check the condition is true */
#define XF_CHK_ERR(cond, error)                 \
({                                              \
    intptr_t __ret;                             \
                                                \
    if (!(__ret = (intptr_t)(cond)))            \
    {                                           \
        TRACE(ERROR, _x("check failed"));       \
        return (error);                         \
    }                                           \
    (int)__ret;                                 \
})
