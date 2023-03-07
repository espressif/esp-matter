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
 * xf-msgq.c
 *
 * System-specific IPC layer Implementation
 ******************************************************************************/

#define MODULE_TAG                      MSGQ

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xos-msgq-if.h"
#include "xf.h"
#include "xaf-api.h"
#include "xaf-structs.h"

/*******************************************************************************
 * Global Definitions
 ******************************************************************************/

extern xf_ap_t *xf_g_ap;
extern XAF_ERR_CODE xaf_malloc(void **buf_ptr, int size, int id);

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(CMD, 1);
TRACE_TAG(RSP, 1);

/*******************************************************************************
 * Global abstractions
 ******************************************************************************/

/* ...prevent instructions reordering */
#define barrier()                           \
    __asm__ __volatile__("": : : "memory")

/* ...memory barrier */
#define XF_PROXY_BARRIER()                  \
    __asm__ __volatile__("memw": : : "memory")

/* ...memory invalidation */
#define XF_PROXY_INVALIDATE(buf, length)    \
    ({ if ((length)) { xthal_dcache_region_invalidate((buf), (length)); barrier(); } buf; })

/* ...memory flushing */
#define XF_PROXY_FLUSH(buf, length)         \
    ({ if ((length)) { barrier(); xthal_dcache_region_writeback((buf), (length)); XF_PROXY_BARRIER(); } buf; })


/*******************************************************************************
 * Local constants - tbd
 ******************************************************************************/

#define SEND_LOCAL_MSGQ_ENTRIES     16
#ifdef RTK_HW
#define MAXIMUM_TIMEOUT             100000
#else
#define MAXIMUM_TIMEOUT             100000
#endif

/*******************************************************************************
 * Internal IPC API implementation
 ******************************************************************************/

/* ...pass command to remote DSP */
int xf_ipc_send(xf_proxy_ipc_data_t *ipc, xf_proxy_msg_t *msg, void *b)
{
    int     ret;

    TRACE(CMD, _b("C[%08x]:(%x,%08x,%u)"), msg->id, msg->opcode, msg->address, msg->length);

    if (msg->address) XF_PROXY_FLUSH((void *)msg->address, msg->length);
    
    /* ...pass message to xos message queue */
    XF_CHK_API(xos_msgq_put(ipc->cmd_msgq, (UWORD32 *)msg));

    XF_CHK_API(xos_event_set(ipc->msgq_event, CMD_MSGQ_READY));

    return 0;
}

/* ...wait for response availability */
int xf_ipc_wait(xf_proxy_ipc_data_t *ipc, UWORD32 timeout)
{
#if 1
    uint32_t Flags;
    XF_CHK_API(xos_event_wait_any(ipc->msgq_event, RESP_MSGQ_READY | DIE_MSGQ_ENTRY));
    XF_CHK_API(xos_event_get(ipc->msgq_event, &Flags));
    if (Flags & DIE_MSGQ_ENTRY) // Check if termination message is received
    {
        TRACE(INIT, _b("xf_ipc_wait: Die received"));
        return -1;
    }
    XF_CHK_API(xos_event_clear(ipc->msgq_event, RESP_MSGQ_READY));
#else
    xf_proxy_msg_t msg;
    
    /* ...get message from xos message queue - blocking call */
#ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get(ipc->resp_msgq, (UWORD32 *) &msg));
#else   // #ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get_timeout(ipc->resp_msgq, (UWORD32 *) &msg, xos_msecs_to_cycles(MAXIMUM_TIMEOUT)));
#endif  // #ifdef __TOOLS_RF2__

    /* ...put message back in xos message queue - wait over */
    XF_CHK_API(xos_msgq_put(ipc->resp_msgq, (UWORD32 *) &msg));
#endif

    return 0;
}

/* ...read response from proxy - blocking API */
int xf_ipc_recv(xf_proxy_ipc_data_t *ipc, xf_proxy_msg_t *msg, void **buffer)
{
    if (xos_msgq_empty(ipc->resp_msgq))
    {
        return 0;
    }

    /* ...get message from xos message queue */
#ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get(ipc->resp_msgq, (UWORD32 *)msg));
#else   // #ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get_timeout(ipc->resp_msgq, (UWORD32 *)msg, xos_msecs_to_cycles(MAXIMUM_TIMEOUT)));
#endif  // #ifdef __TOOLS_RF2__

    TRACE(RSP, _b("R[%08x]:(%x,%u,%08x)"), msg->id, msg->opcode, msg->length, msg->address);

    /* ...translate shared address into local pointer */
    XF_CHK_ERR((*buffer = xf_ipc_a2b(ipc, msg->address)) != (void *)-1, -EBADFD);

    /* ...return positive result indicating the message has been received */
    return sizeof(*msg);
}

/*******************************************************************************
 * Internal API functions implementation
 ******************************************************************************/

/* ...open proxy interface on proper DSP partition */
int xf_ipc_open(xf_proxy_ipc_data_t *ipc, UWORD32 core)
{
    int ret;
    /* ...open xos-msgq interface */
    XF_CHK_API(ipc_msgq_init(&ipc->cmd_msgq, &ipc->resp_msgq, &ipc->msgq_event));

    /* ...proxy local queue */
    ret = xaf_malloc((void**)&(ipc->lresp_msgq), (XOS_MSGQ_SIZE(SEND_LOCAL_MSGQ_ENTRIES, sizeof(xf_user_msg_t))), XAF_MEM_ID_DEV);
    if(ret != XAF_NO_ERROR)
        return ret;

    /* ...allocation mustn't fail on host */
    BUG(ipc->lresp_msgq == NULL, _x("Out-of-memeory"));

    XF_CHK_API(xos_msgq_create(ipc->lresp_msgq, SEND_LOCAL_MSGQ_ENTRIES, sizeof(xf_user_msg_t), 0));

    TRACE(INIT, _b("proxy-%u interface opened"), core);

    return 0;
}

/* ...set event to close proxy handle */
void xf_ipc_close_set_event(xf_proxy_ipc_data_t *ipc, UWORD32 core)
{
    /* ...Signal thread to close */
    xos_event_set(ipc->msgq_event, DIE_MSGQ_ENTRY);
    xos_thread_yield();
}

/* ...close proxy handle */
void xf_ipc_close(xf_proxy_ipc_data_t *ipc, UWORD32 core)
{
    /* ...close xos-msgq interface */
    ipc_msgq_delete(&ipc->cmd_msgq, &ipc->resp_msgq, &ipc->msgq_event);

    /* ...close proxy local queue */
    xos_msgq_delete(ipc->lresp_msgq);
    xf_g_ap->xf_mem_free_fxn(ipc->lresp_msgq, XAF_MEM_ID_DEV);
    ipc->lresp_msgq = NULL;

    TRACE(INIT, _b("proxy-%u interface closed"), core);
}

/*******************************************************************************
 * Local component message queue API implementation
 ******************************************************************************/

int xf_proxy_ipc_response_put(xf_proxy_ipc_data_t *ipc, xf_proxy_msg_t *msg)
{
    /* ...pass message to xos message queue */
    XF_CHK_API(xos_msgq_put(ipc->lresp_msgq, (UWORD32 *)msg));

    return 0;
}

int xf_proxy_ipc_response_get(xf_proxy_ipc_data_t *ipc, xf_proxy_msg_t *msg)
{
    /* ...get message from xos message queue */
#ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get(ipc->lresp_msgq, (UWORD32 *)msg));
#else   // #ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get_timeout(ipc->lresp_msgq, (UWORD32 *)msg, xos_msecs_to_cycles(MAXIMUM_TIMEOUT)));
#endif  // #ifdef __TOOLS_RF2__

    return 0;
}

int xf_ipc_response_put(xf_ipc_data_t *ipc, xf_user_msg_t *msg)
{
    /* ...pass message to xos message queue */
    XF_CHK_API(xos_msgq_put(ipc->resp_msgq, (UWORD32 *)msg));

    return 0;
}

int xf_ipc_response_get(xf_ipc_data_t *ipc, xf_user_msg_t *msg)
{
    /* ...get message from xos message queue */
#ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get(ipc->resp_msgq, (UWORD32 *)msg));
#else   // #ifdef __TOOLS_RF2__
    XF_CHK_API(xos_msgq_get_timeout(ipc->resp_msgq, (UWORD32 *)msg, xos_msecs_to_cycles(MAXIMUM_TIMEOUT)));
#endif  // #ifdef __TOOLS_RF2__

    return 0;
}

int xf_ipc_data_init(xf_ipc_data_t *ipc)
{
    int ret, size;

    ret = xaf_malloc((void**)&(ipc->resp_msgq), (XOS_MSGQ_SIZE(SEND_LOCAL_MSGQ_ENTRIES, sizeof(xf_user_msg_t))), XAF_MEM_ID_COMP);
    if(ret != XAF_NO_ERROR)
        return ret;

    /* ...allocation mustn't fail on host */
    BUG(ipc->resp_msgq == NULL, _x("Out-of-memeory"));

    XF_CHK_API(xos_msgq_create(ipc->resp_msgq, SEND_LOCAL_MSGQ_ENTRIES, sizeof(xf_user_msg_t), 0));

    return 0;
}

int xf_ipc_data_destroy(xf_ipc_data_t *ipc)
{
    int ret;

    ret = xos_msgq_delete(ipc->resp_msgq);
    if (ret != XOS_OK)
        return XAF_XOS_ERROR;
    xf_g_ap->xf_mem_free_fxn(ipc->resp_msgq, XAF_MEM_ID_COMP);
    ipc->resp_msgq = NULL;

    return 0;
}

