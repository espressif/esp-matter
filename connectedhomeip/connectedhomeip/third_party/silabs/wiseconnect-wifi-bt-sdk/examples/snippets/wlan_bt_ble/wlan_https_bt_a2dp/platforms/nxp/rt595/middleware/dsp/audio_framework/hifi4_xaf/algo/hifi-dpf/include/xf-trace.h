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

#ifndef __XF_H
#error  "xf-debug.h mustn't be included directly"
#endif

/*******************************************************************************
 * Types definitions
 ******************************************************************************/

#if XF_TRACE

/* ...trace data definition */
typedef struct xf_trace_data
{
    /* ...current write position in tracing buffer */
    char               *p;
    
    /* ...threshold position for buffer submission */
    char               *end;

#if XF_TRACE_REMOTE
    /* ...beginning of non-commited internal tracing buffer */
    char               *start;    

    /* ...message queue */
    xf_msg_queue_t      queue;
#endif
    
}   xf_trace_data_t;

#else

/* ...stub for disabled tracing */
typedef UWORD32     xf_trace_data_t[0];

#endif  /* XF_TRACE */

/*******************************************************************************
 * Internal API functions
 ******************************************************************************/

#if XF_TRACE_REMOTE
/* ...submit buffer for tracing */
extern void xf_trace_submit(UWORD32 core, xf_message_t *m);

/* ...flush current buffer */
extern void xf_trace_flush(UWORD32 core, xf_message_t *m);

#else

#define xf_trace_submit(core, m)       (void)0
#define xf_trace_flush(core, m)        (void)0

#endif  /* XF_TRACE_REMOTE */
