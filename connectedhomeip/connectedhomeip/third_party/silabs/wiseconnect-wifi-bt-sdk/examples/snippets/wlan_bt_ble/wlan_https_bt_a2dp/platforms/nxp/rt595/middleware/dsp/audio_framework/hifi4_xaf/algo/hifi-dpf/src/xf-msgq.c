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
 * xf-shmem.c
 *
 * DSP shared memory interface implementation
 ******************************************************************************/

#define MODULE_TAG                      SHMEM

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"
#ifdef __TOOLS_RF2__
#include <xtensa/xos/xos.h>
#else   // #ifdef __TOOLS_RF2__
#include <xtensa/xos.h>
#endif  // #ifdef __TOOLS_RF2__

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

/* ...general initialization sequence */
TRACE_TAG(INIT, 1);

/* ...interface status change */
TRACE_TAG(EXEC, 0);

/* ...command reception */
TRACE_TAG(CMD, 1);

/* ...response generation */
TRACE_TAG(RSP, 1);

/*******************************************************************************
 * Local constants definitions
 ******************************************************************************/

/* ...local interface status change flag */
#define XF_PROXY_STATUS_LOCAL           (1 << 0)

/* ...remote status change notification flag */
#define XF_PROXY_STATUS_REMOTE          (1 << 1)

/*******************************************************************************
 * Internal helpers
 ******************************************************************************/

/* ...put message into proxy queue */
static inline void xf_msg_proxy_put(xf_message_t *m)
{
    UWORD32                 dst = XF_MSG_DST_CORE(m->id);
    UWORD32                 src = XF_MSG_SRC_CORE(m->id);
    xf_core_rw_data_t  *rw = XF_CORE_RW_DATA(dst);
    int                 first;

    /* ...get an access to shared rw-memory (we are running on "source" core) */
    xf_mutex_lock(src);
    
    /* ...assure memory coherency if needed */
    if (XF_REMOTE_IPC_NON_COHERENT)
    {
        /* ...invalidate rw-shared memory region */
        XF_PROXY_INVALIDATE(rw, sizeof(*rw));
        
        /* ...put message into shared queue */
        first = xf_msg_enqueue(&rw->remote, m);

        /* ...flush both message and shared queue data */
        XF_PROXY_FLUSH(rw, sizeof(*rw)), XF_PROXY_FLUSH(m, sizeof(*m));
    }
    else
    {
        /* ...no memory coherency concerns; just place a message in the queue */
        first = xf_msg_enqueue(&rw->remote, m);
    }

    /* ...release rw-memory region lock */
    xf_mutex_unlock(src);

    /* ...assert IPI interrupt on target ("destination") core if needed */
    if (first && (dst ^ src))
    {
        xf_ipi_assert(dst);
    }
}

/* ...retrieve message from proxy queue */
static inline xf_message_t * xf_msg_proxy_get(UWORD32 core)
{
    xf_core_rw_data_t  *rw = XF_CORE_RW_DATA(core);
    xf_message_t       *m;
    
    /* ...retrieve message from queue in atomic fashion */
    xf_mutex_lock(core);

    /* ...assure memory coherency if needed */
    if (XF_REMOTE_IPC_NON_COHERENT)
    {
        /* ...invalidate rw-memory */
        XF_PROXY_INVALIDATE(rw, sizeof(*rw));

        /* ...dequeue message from response queue */
        m = xf_msg_dequeue(&rw->remote);

        /* ...flush rw memory */
        XF_PROXY_FLUSH(rw, sizeof(*rw));

        /* ...invalidate message data if found */
        (m ? XF_PROXY_INVALIDATE(m, sizeof(*m)) : 0);
    }
    else
    {
        /* ...just dequeue message from response queue */
        m = xf_msg_dequeue(&rw->remote);
    }
    
    /* ...release the rw-lock */
    xf_mutex_unlock(core);

    return m;
}

/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

/* ...retrieve all incoming commands from shared memory ring-buffer */
static UWORD32 xf_shmem_process_input(UWORD32 core)
{
    xf_message_t   *m;
    UWORD32             status = 0;
    XosMsgQueue    *cmd_msgq;

    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);

    cmd_msgq = ro->ipc.cmd_msgq;

    /* ...process all committed commands */
    while (!xos_msgq_empty(cmd_msgq))
    {
        xf_proxy_message_t command;

        /* ...allocate message; the call should not fail */
        if ((m = xf_msg_pool_get(&XF_CORE_RO_DATA(core)->pool)) == NULL)
            break;

        /* ...if queue was full, set global proxy update flag */
        if (xos_msgq_full(cmd_msgq))
            status |= XF_PROXY_STATUS_REMOTE | XF_PROXY_STATUS_LOCAL;
        else
            status |= XF_PROXY_STATUS_LOCAL;

        /*  v-tbd ...synchronize memory contents */

        /* ...get oldest not processed command */
        XF_CHK_API(xos_msgq_get(cmd_msgq, (UWORD32 *) &command)); 

        /* ...fill message parameters */
        m->id = command.session_id;
        m->opcode = command.opcode;
        m->length = command.length;
        m->buffer = xf_ipc_a2b(core, command.address);

        TRACE(CMD, _b("C[%08x]:(%08x,%u,%p)"), m->id, m->opcode, m->length, m->buffer);

        /* ...invalidate message buffer contents as required - not here - tbd */
        (XF_OPCODE_CDATA(m->opcode) ? XF_PROXY_INVALIDATE(m->buffer, m->length) : 0);
        
        /* ...and schedule message execution on proper core */
        xf_msg_submit(m);
    }

    return status;
}

/* ...send out all pending outgoing responses to the shared memory ring-buffer */
static UWORD32 xf_shmem_process_output(UWORD32 core)
{
    xf_message_t   *m;
    UWORD32             status = 0;
    XosMsgQueue    *resp_msgq;

    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);

    resp_msgq = ro->ipc.resp_msgq;

    /* ...while we have response messages and there's space to write out one */
    while (!xos_msgq_full(resp_msgq))
    {
        xf_proxy_message_t     response;

        /* ...remove message from internal queue */
        if ((m = xf_msg_proxy_get(core)) == NULL)
            break;

        /* ...notify remote interface each time we send it a message (only if it was empty?) */
        status = XF_PROXY_STATUS_REMOTE | XF_PROXY_STATUS_LOCAL;

#if 0
        /* ...need to decide on best strategy - tbd */
        if (XF_QUEUE_EMPTY(read_idx, write_idx))
            status |= XF_PROXY_STATUS_REMOTE | XF_PROXY_STATUS_LOCAL;
        else
            status |= XF_PROXY_STATUS_LOCAL;
#endif

        /* ...flush message buffer contents to main memory as required - too late - different core - tbd */
        (XF_OPCODE_RDATA(m->opcode) ? XF_PROXY_FLUSH(m->buffer, m->length) : 0);

        /* ...put the response message fields */
        response.session_id = m->id;
        response.opcode = m->opcode;
        response.length = m->length;
        response.address = xf_ipc_b2a(core, m->buffer);

        /* ...put the response in message queue */
        XF_CHK_API(xos_msgq_put(resp_msgq, (UWORD32 *) &response));

        /* v-tbd ...flush the content of the caches to main memory */

        TRACE(RSP, _b("R[%08x]:(%08x,%u,%p)"), m->id, m->opcode, m->length, m->buffer);

        /* ...return message back to the pool */
        xf_msg_pool_put(&XF_CORE_RO_DATA(core)->pool, m);
    }

    /* ...return interface status change flags */
    return status;
}

/*******************************************************************************
 * Entry points
 ******************************************************************************/

/* ...process local/remote shared memory interface status change */
void xf_shmem_process_queues(UWORD32 core)
{
    UWORD32     status;

    do
    {
        /* ...acknowledge/clear any pending incoming interrupt */
        XF_PROXY_SYNC_PEER(core);

        /* ...send out pending response messages (frees message buffers, so do it first) */
        status = xf_shmem_process_output(core);

        /* ...receive and forward incoming command messages (allocates message buffers) */
        status |= xf_shmem_process_input(core);

        /* ...assert remote mailbox interrupt if global update bit is set */
        if (status & XF_PROXY_STATUS_REMOTE)
        {
            XF_PROXY_NOTIFY_PEER(core);
        }
    }
    while (status);
}

/* ...completion callback for message originating from remote proxy */
void xf_msg_proxy_complete(xf_message_t *m)
{
    /* ...place message into proxy response queue */
    xf_msg_proxy_put(m);
}

/* ...initialize shared memory interface (DSP side) */
int xf_shmem_init(UWORD32 core)
{
    xf_core_rw_data_t  *rw = XF_CORE_RW_DATA(core);
    xf_core_ro_data_t  *ro = XF_CORE_RO_DATA(core);

    /* ...initialize local/remote message queues */
    xf_msg_queue_init(&rw->local);
    xf_msg_queue_init(&rw->remote);

    /* ...initialize global message list */
    XF_CHK_API(xf_msg_pool_init(&ro->pool, XF_CFG_MESSAGE_POOL_SIZE, core));

    /* ...flush memory content as needed */
    (XF_REMOTE_IPC_NON_COHERENT ? XF_PROXY_FLUSH(rw, sizeof(*rw)) : 0);

    /* ...system-specific initialization of IPC layer */
    XF_CHK_API(xf_ipc_init(core));

    TRACE(INIT, _b("SHMEM-%u subsystem initialized"), core);

    return 0;
}
