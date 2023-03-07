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
 * xf-shmem.h
 *
 * Definitions for Xtensa SHMEM configuration
 *******************************************************************************/

#ifndef __XF_H
#error "xf-shmem.h mustn't be included directly"
#endif

/*******************************************************************************
 * Memory structures
 ******************************************************************************/

/* ...data managed by host CPU (remote) - in case of shunt it is a IPC layer */
struct xf_proxy_host_data
{
    /* ...command queue */
    xf_proxy_message_t      command[XF_PROXY_MESSAGE_QUEUE_LENGTH];
    
    /* ...writing index into command queue */
    UWORD32                     cmd_write_idx;
    
    /* ...reading index for response queue */
    UWORD32                     rsp_read_idx;

}   __attribute__((__packed__, __aligned__(XF_PROXY_ALIGNMENT)));

/* ...data managed by DSP (local) */
struct xf_proxy_dsp_data
{
    /* ...response queue */
    xf_proxy_message_t      response[XF_PROXY_MESSAGE_QUEUE_LENGTH];
    
    /* ...writing index into response queue */
    UWORD32                     rsp_write_idx;
    
    /* ...reading index for command queue */
    UWORD32                     cmd_read_idx;

}   __attribute__((__packed__, __aligned__(XF_PROXY_ALIGNMENT)));

/* ...shared memory data */
typedef struct xf_shmem_data 
{
    /* ...outgoing data (maintained by host CPU (remote side)) */
    struct xf_proxy_host_data   remote      __xf_shmem__;
    
    /* ...ingoing data (maintained by DSP (local side)) */
    struct xf_proxy_dsp_data    local       __xf_shmem__;

    /* ...shared memory pool (page-aligned; why? we map memory to user-space) */
    UWORD8                          buffer[XF_CFG_REMOTE_IPC_POOL_SIZE]   __attribute__((__aligned__(4096)));

}   xf_shmem_data_t;

/*******************************************************************************
 * Shared memory accessors
 ******************************************************************************/

/* ...shared memory pointer for a core */
#define XF_SHMEM_DATA(core)                         \
    ((xf_shmem_data_t *)XF_CORE_DATA(core)->shmem)

/* ...atomic reading */
#define XF_PROXY_READ_ATOMIC(var)                   \
    ({ XF_PROXY_INVALIDATE(&(var), sizeof(var)); (var); })

/* ...atomic writing */
#define XF_PROXY_WRITE_ATOMIC(var, value)           \
    ({(var) = (value); XF_PROXY_FLUSH(&(var), sizeof(var)); (value); })

/* ...accessors */
#define XF_PROXY_READ(core, field)                  \
    __XF_PROXY_READ_##field(XF_SHMEM_DATA(core))

#define XF_PROXY_WRITE(core, field, v)              \
    __XF_PROXY_WRITE_##field(XF_SHMEM_DATA(core), (v))

/* ...individual fields accessors */
#define __XF_PROXY_READ_cmd_write_idx(proxy)        \
    XF_PROXY_READ_ATOMIC(proxy->remote.cmd_write_idx)

#define __XF_PROXY_READ_cmd_read_idx(proxy)         \
    proxy->local.cmd_read_idx

#define __XF_PROXY_READ_rsp_write_idx(proxy)        \
    proxy->local.rsp_write_idx

#define __XF_PROXY_READ_rsp_read_idx(proxy)         \
    XF_PROXY_READ_ATOMIC(proxy->remote.rsp_read_idx)

/* ...individual fields accessors */
#define __XF_PROXY_WRITE_cmd_write_idx(proxy, v)    \
    XF_PROXY_WRITE_ATOMIC(proxy->remote.cmd_write_idx, v)

#define __XF_PROXY_WRITE_cmd_read_idx(proxy, v)     \
    XF_PROXY_WRITE_ATOMIC(proxy->local.cmd_read_idx, v)

#define __XF_PROXY_WRITE_rsp_read_idx(proxy, v)     \
    XF_PROXY_WRITE_ATOMIC(proxy->remote.rsp_read_idx, v)

#define __XF_PROXY_WRITE_rsp_write_idx(proxy, v)    \
    XF_PROXY_WRITE_ATOMIC(proxy->local.rsp_write_idx, v)

/* ...command buffer accessor */
#define XF_PROXY_COMMAND(core, idx)                 \
    (&XF_SHMEM_DATA((core))->remote.command[(idx)])

/* ...response buffer accessor */
#define XF_PROXY_RESPONSE(core, idx)                \
    (&XF_SHMEM_DATA((core))->local.response[(idx)])

/*******************************************************************************
 * Platform-specific SHMEM enable status
 ******************************************************************************/

static inline int xf_shmem_enabled(UWORD32 core)
{
    return (core == 0);
}

/*******************************************************************************
 * API functions
 ******************************************************************************/

/* ...process shared memory interface on given DSP core */
extern void xf_shmem_process_queues(UWORD32 core);

/* ...completion callback for message originating from remote proxy */
extern void xf_msg_proxy_complete(xf_message_t *m);

/* ...initialize shared memory interface (DSP side) */
extern int xf_shmem_init(UWORD32 core);
