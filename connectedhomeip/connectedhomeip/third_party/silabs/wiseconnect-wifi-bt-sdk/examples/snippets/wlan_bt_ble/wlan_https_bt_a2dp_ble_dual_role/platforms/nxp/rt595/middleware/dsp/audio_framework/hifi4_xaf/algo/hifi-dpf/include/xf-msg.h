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
 * xf-msg.h
 *
 * Internal messages, and message queues.
 *******************************************************************************/

#ifndef __XF_H
#error "xf-msg.h mustn't be included directly"
#endif

/*******************************************************************************
 * Types definitions
 ******************************************************************************/

/* ...forward declaration */
typedef struct xf_message   xf_message_t;

/* ...audio command/response message (internal to DSP processing framework) */
struct xf_message
{
    /* ...pointer to next item in the list */
    xf_message_t       *next;

    /* ...shmem session_id */
    UWORD32                 id;

    /* ...operation code */
    UWORD32                 opcode;

    /* ...length of attached message buffer */
    UWORD32                 length;

    /* ...message buffer (translated virtual address) */
    void               *buffer;
};

/* ...cache-line aligned message buffer */
XF_ALIGNED_TYPEDEF(xf_message_t, __xf_message_t); //dependent on cache line width, variable depending on hifi core.
//XF_MAX_CACHE_ALIGNED_TYPEDEF(xf_message_t, __xf_message_t); ////independent of cache line width.

/* ...message pool definition */
typedef struct xf_msg_pool
{
    /* ...array of aligned messages */
    __xf_message_t     *p;

    /* ...pointer to first free item in the pool */
    __xf_message_t     *head;

    /* ...total size of the pool */
    UWORD32                 n;

}   xf_msg_pool_t;

/* ...message accessor */
static inline xf_message_t * xf_msg_pool_item(xf_msg_pool_t *pool, UWORD32 i)
{
    return (xf_message_t *) &pool->p[i];
}

/*******************************************************************************
 * Message queue data
 ******************************************************************************/

/* ...message queue (single-linked FIFO list) */
typedef struct  xf_msg_queue
{
    /* ...head of the queue */
    xf_message_t       *head;

    /* ...tail pointer */
    xf_message_t       *tail;

}   xf_msg_queue_t;

/*******************************************************************************
 * Message queue API
 ******************************************************************************/

/* ...initialize message queue */
static inline void  xf_msg_queue_init(xf_msg_queue_t *queue)
{
    queue->head = queue->tail = NULL;
}

/* ...push message in FIFO queue */
static inline int xf_msg_enqueue(xf_msg_queue_t *queue, xf_message_t *m)
{
    int     empty = (queue->head == NULL);

    /* ...set list terminating pointer */
    m->next = NULL;

    if (empty)
        queue->head = m;
    else
        queue->tail->next = m;

    /* ...advance tail pointer */
    queue->tail = m;

    /* ...return emptiness status */
    return empty;
}

#define xf_msg_enqueue(queue, m)                                        \
({                                                                      \
    BUG((m)->next != NULL, _x("message is active: %p"), (m));           \
    (xf_msg_enqueue)((queue), (m));                                     \
})

/* ...retrieve (pop) next message from FIFO queue */
static inline xf_message_t * xf_msg_dequeue(xf_msg_queue_t *queue)
{
    xf_message_t   *m = queue->head;

    /* ...check if there is anything in the queue and dequeue it */
    if (m != NULL)
    {
        /* ...advance head to the next entry in the queue */
        if ((queue->head = m->next) == NULL)
            queue->tail = NULL;

        /* ...debug - wipe out next pointer */
        m->next = NULL;
    }

    return m;
}

/* ...test if message queue is empty */
static inline int xf_msg_queue_empty(xf_msg_queue_t *queue)
{
    return (queue->head == NULL);
}

/* ...get message queue head pointer */
static inline xf_message_t * xf_msg_queue_head(xf_msg_queue_t *queue)
{
    return queue->head;
}

/* ...check if message belongs to a pool */
static inline int xf_msg_from_pool(xf_msg_pool_t *pool, xf_message_t *m)
{
    return (UWORD32)((__xf_message_t*)m - pool->p) < pool->n;   
}

/*******************************************************************************
 * Global message pool API
 ******************************************************************************/

/* ...submit message execution on local DSP core */
extern void xf_msg_schedule(xf_message_t *m, UWORD32 ts);

/* ...schedule message execution from ISR context */
extern void xf_msg_schedule_isr(xf_message_t *m);

/* ...submit message for execution on some DSP */
extern void xf_msg_submit(xf_message_t *m);

/* ...cancel local (scheduled on current core) message execution */
extern void xf_msg_cancel(xf_message_t *m);

/* ...complete message processing */
extern void xf_msg_complete(xf_message_t *m);

/* ...complete message from ISR context */
extern void xf_msg_complete_isr(xf_message_t *m);

/* ...allocate message pool on specific core */
extern int  xf_msg_pool_init(xf_msg_pool_t *pool, UWORD32 n, UWORD32 core);

/* ...allocate message from a pool (no concurrent access from other cores) */
extern xf_message_t * xf_msg_pool_get(xf_msg_pool_t *pool);

/* ...return message back to the pool (no concurrent access from other cores) */
extern void xf_msg_pool_put(xf_msg_pool_t *pool, xf_message_t *m);

/* ...destroy message pool */
extern void xf_msg_pool_destroy(xf_msg_pool_t *pool, UWORD32 core);

/* ...indicate whether pool of free messages is empty */
extern int  xf_message_pool_empty(void);

/* ...initialize global pool of messages */
extern void xf_message_pool_init(void);

/*******************************************************************************
 * Auxiliary helpers
 ******************************************************************************/

/* ...send response message to caller */
static inline void xf_response(xf_message_t *m)
{
    xf_msg_complete(m);
}

/* ...send response message with output buffer */
static inline void xf_response_data(xf_message_t *m, UWORD32 length)
{
    /* ...adjust message output buffer */
    m->length = length;

    /* ...return message to originator */
    xf_msg_complete(m);
}

/* ...send generic "ok" message (no data buffer) */
static inline void xf_response_ok(xf_message_t *m)
{
    /* ...adjust message output buffer */
    m->length = 0;

    /* ...return message to originator */
    xf_msg_complete(m);
}

/* ...send error-response message */
static inline void xf_response_err(xf_message_t *m)
{
    /* ...set generic error message */
    m->opcode = XF_UNREGISTER, m->length = 0;

    /* ...return message to originator */
    xf_msg_complete(m);
}
