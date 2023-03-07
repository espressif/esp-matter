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
 * xf-io.h
 *
 * Input/output data ports
 *******************************************************************************/

#ifndef __XF_H
#error "xf-io.h mustn't be included directly"
#endif

/*******************************************************************************
 * Types definitions
 ******************************************************************************/

/* ...input port with interim buffer */
typedef struct xf_input_port
{
    /* ...message queue */
    xf_msg_queue_t          queue;
    
    /* ...internal contiguous buffer to store incoming data */
    void                   *buffer;

    /* ...size of internal buffer */
    UWORD32                     length;
    
    /* ...current writing position in the buffer */
    UWORD32                     filled;
    
    /* ...interim pointer to input message buffer */
    void                   *access;
    
    /* ...remaining length of current input message */
    UWORD32                     remaining;
    
    /* ...execution flags */
    UWORD32                     flags;

}   xf_input_port_t;

/*******************************************************************************
 * Input port flags
 ******************************************************************************/

/* ...data enabled */
#define XF_INPUT_FLAG_CREATED           (1 << 0)

/* ...data enabled */
#define XF_INPUT_FLAG_ENABLED           (1 << 1)

/* ...end-of-stream condition */
#define XF_INPUT_FLAG_EOS               (1 << 2)

/* ...stream completed */
#define XF_INPUT_FLAG_DONE              (1 << 3)

/* ...stream purging sequence */
#define XF_INPUT_FLAG_PURGING           (1 << 4)

/* ...base input port flags mask */
#define __XF_INPUT_FLAGS(flags)         ((flags) & ((1 << 5) - 1))

/* ...custom input port flag */
#define __XF_INPUT_FLAG(f)              ((f) << 5)

/*******************************************************************************
 * Helpers
 ******************************************************************************/

/* ...test if input port is created */
static inline int xf_input_port_created(xf_input_port_t *port)
{
    return (port->flags & XF_INPUT_FLAG_CREATED);
}

/* ...check if input port is ready (has pending message) */
static inline int xf_input_port_ready(xf_input_port_t *port)
{
    return (xf_msg_queue_head(&port->queue) != NULL);
}

/* ...test if input port entered end-of-stream condition */
static inline int xf_input_port_done(xf_input_port_t *port)
{
    return (port->flags & XF_INPUT_FLAG_DONE);
}

/* ...check if port is in bypass mode */
static inline int xf_input_port_bypass(xf_input_port_t *port)
{
    return port->buffer == NULL;
}

/* ...bypass port only: check if there is a data available */
static inline void * xf_input_port_data(xf_input_port_t *port)
{
    return port->access;
}

/* ...bypass port only: get remaining length of current message */
static inline UWORD32 xf_input_port_length(xf_input_port_t *port)
{
    return port->remaining;
}

/* ...non-bypass port only: get current fill level */
static inline UWORD32 xf_input_port_level(xf_input_port_t *port)
{
    return port->filled;
}

/*******************************************************************************
 * Output port data
 ******************************************************************************/

typedef struct xf_output_port
{
    /* ...pending message queue */
    xf_msg_queue_t          queue;

    /* ...message pool */
    xf_msg_pool_t           pool;

    /* ...saved port unrouting message */
    xf_message_t           *unroute;

    /* ...length of output buffer */
    UWORD32                     length;

    /* ...output port flags */
    UWORD32                     flags;

}   xf_output_port_t;

/*******************************************************************************
 * Output port flags
 ******************************************************************************/

/* ...port is created */
#define XF_OUTPUT_FLAG_CREATED          (1 << 0)

/* ...port is routed */
#define XF_OUTPUT_FLAG_ROUTED           (1 << 1)

/* ...data enabled */
#define XF_OUTPUT_FLAG_ENABLED          (1 << 2)

/* ...stream completed */
#define XF_OUTPUT_FLAG_DONE             (1 << 3)

/* ...flushing sequence is on-going */
#define XF_OUTPUT_FLAG_FLUSHING         (1 << 4)

/* ...port is idle - no outstanding messages */
#define XF_OUTPUT_FLAG_IDLE             (1 << 5)

/* ...port is being unrouted */
#define XF_OUTPUT_FLAG_UNROUTING        (1 << 6)

/* ...base output port flags accessor */
#define __XF_OUTPUT_FLAGS(flags)        ((flags) & ((1 << 7) - 1))

/* ...custom output port flag */
#define __XF_OUTPUT_FLAG(f)             ((f) << 7)

/*******************************************************************************
 * Helpers
 ******************************************************************************/

/* ...test if input port is created */
static inline int xf_output_port_created(xf_output_port_t *port)
{
    return (port->flags & XF_OUTPUT_FLAG_CREATED);
}

/* ...check if port is routed */
static inline int xf_output_port_routed(xf_output_port_t *port)
{
    return ((port->flags & XF_OUTPUT_FLAG_ROUTED) != 0);
}

/* ...check if port unrouting sequence is ongoing */
static inline int xf_output_port_unrouting(xf_output_port_t *port)
{
    return ((port->flags & XF_OUTPUT_FLAG_UNROUTING) != 0);
}

/* ...check if port is idle (owns all data buffers) */
static inline int xf_output_port_idle(xf_output_port_t *port)
{
    return ((port->flags & XF_OUTPUT_FLAG_IDLE) != 0);
}

/* ...check if port is ready (has output buffers - better use flags - tbd) */
static inline int xf_output_port_ready(xf_output_port_t *port)
{
    return (xf_msg_queue_head(&port->queue) != NULL && !xf_output_port_unrouting(port));
}

/* ...output port flow-control message accessor */
static inline xf_message_t * xf_output_port_control_msg(xf_output_port_t *port)
{
    return xf_msg_pool_item(&port->pool, 0);
}

/*******************************************************************************
 * Input port API
 ******************************************************************************/

/* ...initialize input port structure */
extern int  xf_input_port_init(xf_input_port_t *port, UWORD32 size, UWORD32 align, UWORD32 core);

/* ...put message into input port queue */
extern int  xf_input_port_put(xf_input_port_t *port, xf_message_t *m);

/* ...fill-in required amount of data into input port buffer */
extern int  xf_input_port_fill(xf_input_port_t *port);

/* ...consume bytes from input buffer */
extern void xf_input_port_consume(xf_input_port_t *port, UWORD32 n);

/* ...purge input port queue */
extern void xf_input_port_purge(xf_input_port_t *port);

/* ...save flow-control message for propagated input port purging sequence */
extern void xf_input_port_control_save(xf_input_port_t *port, xf_message_t *m);

/* ...complete input port purging sequence */
extern void xf_input_port_purge_done(xf_input_port_t *port);

/* ...destroy input port data */
extern void xf_input_port_destroy(xf_input_port_t *port, UWORD32 core);

/*******************************************************************************
 * Output port API
 ******************************************************************************/

/* ...initialize output port structure */
extern int  xf_output_port_init(xf_output_port_t *port, UWORD32 size);

/* ...put next message to the output port */
extern int  xf_output_port_put(xf_output_port_t *port, xf_message_t *m);

/* ...get data buffer of output message */
extern void * xf_output_port_data(xf_output_port_t *port);

/* ...route output port */
extern int xf_output_port_route(xf_output_port_t *port, UWORD32 id, UWORD32 n, UWORD32 length, UWORD32 align);

/* ...unroute output port */
extern void xf_output_port_unroute(xf_output_port_t *port);

/* ...start output port unrouting sequence */
extern void xf_output_port_unroute_start(xf_output_port_t *port, xf_message_t *m);

/* ...complete port unrouting sequence */
extern void xf_output_port_unroute_done(xf_output_port_t *port);

/* ...produce output message marking amount of bytes produced */
extern int  xf_output_port_produce(xf_output_port_t *port, UWORD32 n);

/* ...purge output port */
extern void xf_output_port_purge(xf_output_port_t *port);

/* ...flush output port and return non-zero result if sequence is over */
extern int xf_output_port_flush(xf_output_port_t *port, UWORD32 opcode);

/* ...complete flushing sequence */
extern void xf_output_port_flush_done(xf_output_port_t *port);

/* ...destroy output port data */
extern void xf_output_port_destroy(xf_output_port_t *port, UWORD32 core);
