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
 * xf-proxy.c
 *
 * Xtensa audio processing framework; application side
 ******************************************************************************/

#define MODULE_TAG                      PROXY

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xos-msgq-if.h"
#include "xf.h"
#include "xaf-structs.h"

/* ...check if non-zero value is a power-of-two */
#define xf_is_power_of_two(v)       (((v) & ((v) - 1)) == 0)

/* ...invalid proxy address */
#define XF_PROXY_BADADDR        XF_CFG_REMOTE_IPC_POOL_SIZE

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(INIT, 1);
TRACE_TAG(CMD, 1);
TRACE_TAG(EXEC, 1);
TRACE_TAG(RSP, 1);
TRACE_TAG(REG, 1);
TRACE_TAG(MEM, 1);
TRACE_TAG(GRAPH, 1);
TRACE_TAG(BUFFER, 1);

/*******************************************************************************
 * DSP-ONLY-SOLUTION Global variables, TBD - move
 ******************************************************************************/
#ifdef HIFI_ONLY_XAF
#define PROXY_THREAD_PRIORITY            6
#define PROXY_THREAD_STACK_SIZE          4096
const char proxy_thread_name[] = "proxyListen";
extern xf_ap_t *xf_g_ap;
extern XAF_ERR_CODE xaf_malloc(void **buf_ptr, int size, int id); 
#endif /* HIFI_ONLY_XAF */

/*******************************************************************************
 * Internal functions definitions
 ******************************************************************************/

/* ...execute proxy command synchronously */
static inline int xf_proxy_cmd_exec(xf_proxy_t *proxy, xf_user_msg_t *msg)
{
    xf_proxy_msg_t  m;
    
    /* ...send command to remote proxy */
    m.id = msg->id, m.opcode = msg->opcode, m.length = msg->length;

    /* ...translate address */
    XF_CHK_ERR((m.address = xf_proxy_b2a(proxy, msg->buffer)) != XF_PROXY_BADADDR, -EINVAL);

    /* ...pass command to remote proxy */
    XF_CHK_API(xf_ipc_send(&proxy->ipc, &m, msg->buffer));

    /* ...wait for response reception indication from proxy thread */
    XF_CHK_API(xf_proxy_response_get(proxy, &m));

    /* ...copy parameters */
    msg->id = m.id, msg->opcode = m.opcode, msg->length = m.length;

    /* ...translate address back to virtual space */
    XF_CHK_ERR((msg->buffer = xf_proxy_a2b(proxy, m.address)) != (void *)-1, -EBADFD);
    
    TRACE(EXEC, _b("proxy[%p]: command done: [%08x:%p:%u]"), proxy, msg->opcode, msg->buffer, msg->length);

    return 0;
}

/* ...pass command to remote DSP */
static inline int xf_proxy_cmd(xf_proxy_t *proxy, xf_handle_t *handle, xf_user_msg_t *m)
{
    xf_proxy_msg_t  msg;

    /* ...set session-id of the message */
    msg.id = __XF_MSG_ID(__XF_AP_CLIENT(proxy->core, handle->client), m->id);
    msg.opcode = m->opcode;
    msg.length = m->length;

    /* ...translate buffer pointer to shared address */
    XF_CHK_ERR((msg.address = xf_proxy_b2a(proxy, m->buffer)) != XF_PROXY_BADADDR, -EINVAL);

    /* ...submit command message to IPC layer */
    return XF_CHK_API(xf_ipc_send(&proxy->ipc, &msg, m->buffer));
}

/* ...allocate local client-id number */
static inline UWORD32 xf_client_alloc(xf_proxy_t *proxy, xf_handle_t *handle)
{
    UWORD32     client;
    
    if ((client = proxy->cmap[0].next) != 0)
    {
        /* ...pop client from free clients list */
        proxy->cmap[0].next = proxy->cmap[client].next;

        /* ...put client handle into association map */
        handle->client = client, proxy->cmap[client].handle = handle;
    }

    return client;
}

/* ...recycle local client-id number */
static inline void xf_client_free(xf_proxy_t *proxy, xf_handle_t *handle)
{
    UWORD32     client = handle->client;
    
    /* ...push client into head of the free clients list */
    proxy->cmap[client].next = proxy->cmap[0].next;
    
    /* ...adjust head of free clients */
    proxy->cmap[0].next = client;
}

/* ...lookup client basing on its local id */
static inline xf_handle_t * xf_client_lookup(xf_proxy_t *proxy, UWORD32 client)
{
    /* ...client index must be in proper range */
    BUG(client >= XF_CFG_PROXY_MAX_CLIENTS, _x("Invalid client index: %u"), client);
    
    /* ...check if client index is small */
    if (proxy->cmap[client].next < XF_CFG_PROXY_MAX_CLIENTS)
        return NULL;
    else
        return proxy->cmap[client].handle;
}

/* ...create new client on remote core */
static inline int xf_client_register(xf_proxy_t *proxy, xf_handle_t *handle, xf_id_t id, UWORD32 core)
{
    void           *b = xf_handle_aux(handle);
    xf_user_msg_t   msg;
    
    /* ...set session-id: source is local proxy, destination is remote proxy */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(proxy->core), __XF_DSP_PROXY(core));
    msg.opcode = XF_REGISTER;
    msg.buffer = b;
    msg.length = strlen(id) + 1;

    /* ...copy component identifier */
    strncpy(b, id, xf_buffer_length(handle->aux));

    /* ...execute command synchronously */
    XF_CHK_API(xf_proxy_cmd_exec(proxy, &msg));
    
    /* ...check operation is successfull */
    XF_CHK_ERR(msg.opcode == XF_REGISTER, -EFAULT);
    
    /* ...save received component global client-id */
    handle->id = XF_MSG_SRC(msg.id);  

    TRACE(REG, _b("[%p]=[%s:%u:%u]"), handle, id, XF_PORT_CORE(handle->id), XF_PORT_CLIENT(handle->id));

    return 0;
}

/* ...unregister client from remote proxy */
static inline int xf_client_unregister(xf_proxy_t *proxy, xf_handle_t *handle)
{
    xf_user_msg_t   msg;

    /* ...make sure the client is consistent */
    BUG(proxy->cmap[handle->client].handle != handle, _x("Invalid handle: %p"), handle);
    
    /* ...set message parameters */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(proxy->core), handle->id);
    msg.opcode = XF_UNREGISTER;
    msg.buffer = NULL;
    msg.length = 0;

    /* ...synchronously execute command on remote proxy */
    XF_CHK_API(xf_proxy_cmd_exec(proxy, &msg));
    
    /* ...opcode must be XF_UNREGISTER - tbd */
    BUG(msg.opcode != XF_UNREGISTER, _x("Invalid opcode: %X"), msg.opcode);
    
    TRACE(REG, _b("%p[%u:%u] unregistered"), handle, XF_PORT_CORE(handle->id), XF_PORT_CLIENT(handle->id));

    return 0;
}

/* ...allocate shared buffer */
static inline int xf_proxy_buffer_alloc(xf_proxy_t *proxy, UWORD32 length, void **buffer)
{
    UWORD32             core = proxy->core;
    xf_user_msg_t   msg;
    
    /* ...prepare command parameters */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(core), __XF_DSP_PROXY(core));
    msg.opcode = XF_ALLOC;
    msg.length = length;
    msg.buffer = NULL;

    /* ...synchronously execute command on remote proxy */
    XF_CHK_API(xf_proxy_cmd_exec(proxy, &msg));

    /* ...check if response is valid */
    XF_CHK_ERR(msg.opcode == XF_ALLOC, -EBADFD);

    /* ...check if allocation is successful */
    XF_CHK_ERR(msg.buffer != NULL, -ENOMEM);

    /* ...save output parameter */
    *buffer = msg.buffer;
    
    TRACE(MEM, _b("proxy-%u: allocated [%p:%u]"), core, *buffer, length);

    return 0;
}

/* ...free shared AP-DSP memory */
static inline int xf_proxy_buffer_free(xf_proxy_t *proxy, void *buffer, UWORD32 length)
{
    UWORD32             core = proxy->core;
    xf_user_msg_t   msg;

    /* ...prepare command parameters */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(core), __XF_DSP_PROXY(core));
    msg.opcode = XF_FREE;
    msg.length = length;
    msg.buffer = buffer;
    
    /* ...synchronously execute command on remote proxy */
    XF_CHK_API(xf_proxy_cmd_exec(proxy, &msg));

    /* ...check if response is valid */
    XF_CHK_ERR(msg.opcode == XF_FREE, -EBADFD);

    TRACE(MEM, _b("proxy-%u: free [%p:%u]"), core, buffer, length);

    return 0;
}

/*******************************************************************************
 * Proxy interface asynchronous receiving thread
 ******************************************************************************/
#ifdef HIFI_ONLY_XAF
static int xf_proxy_thread(void *arg, int unused)
#else
static void * xf_proxy_thread(void *arg)
#endif
{
    xf_proxy_t     *proxy = arg;
    xf_handle_t    *client;
    int             r;
    
    /* ...start polling thread */
    while (1)
    {
        xf_proxy_msg_t  m;
        xf_user_msg_t   msg;
        
        /* ...wait for response from remote proxy (infinite timeout) */
        if ((r = xf_ipc_wait(&proxy->ipc, 0)) < 0)
            break;

        /* ...retrieve all responses received */
        while ((r = xf_ipc_recv(&proxy->ipc, &m, &msg.buffer)) == sizeof(m))
        {
            /* ...make sure we have proper core identifier of SHMEM interface */
            BUG(XF_MSG_DST_CORE(m.id) != proxy->core, _x("Invalid session-id: %X (core=%u)"), m.id, proxy->core);

            /* ...make sure translation is successful */
            BUG(msg.buffer == (void *)-1, _x("Invalid buffer address: %08x"), m.address);        

            /* ...retrieve information fields */
            msg.id = XF_MSG_SRC(m.id), msg.opcode = m.opcode, msg.length = m.length;           
        
            TRACE(RSP, _b("R[%08x]:(%08x,%u,%08x)"), m.id, m.opcode, m.length, m.address);

            /* ...lookup component basing on destination port specification */
            if (XF_AP_CLIENT(m.id) == 0)
            {
                /* ...put proxy response to local IPC queue */
                xf_proxy_response_put(proxy, &m);
            }
            else if ((client = xf_client_lookup(proxy, XF_AP_CLIENT(m.id))) != NULL)
            {
                /* ...client is found; invoke its response callback (must be non-blocking) */
                client->response(client, &msg);
            }
            else
            {
                /* ...client has been disconnected already; drop message */
                TRACE(RSP, _b("Client look-up failed - drop message"));
            }
        }

        /* ...if result code is negative; terminate thread operation */
        if (r < 0)
        {
            TRACE(ERROR, _x("abnormal proxy[%p] thread termination: %d"), proxy, r);
            break;
        }
    }
    
    TRACE(INIT, _b("IPC proxy[%p] thread terminated: %d"), proxy, r);

#ifdef HIFI_ONLY_XAF
    return r;
#else    
    return (void *)(intptr_t)r;
#endif
}

/*******************************************************************************
 * HiFi proxy API
 ******************************************************************************/

/* ...open HiFi proxy */
int xf_proxy_init(xf_proxy_t *proxy, UWORD32 core)
{
    UWORD32             i;
    int             r;
    
    /* ...initialize proxy lock */
    __xf_lock_init(&proxy->lock);
   
    /* ...open proxy IPC interface */
    XF_CHK_API(xf_ipc_open(&proxy->ipc, core));

    /* ...save proxy core - hmm, too much core identifiers - tbd */
    proxy->core = core;
    
    /* ...line-up all clients into single-linked list */
    for (i = 0; i < XF_CFG_PROXY_MAX_CLIENTS - 1; i++)
    {
        proxy->cmap[i].next = i + 1;
    }
    
    /* ...tail of the list points back to head (list terminator) */
    proxy->cmap[i].next = 0;

    /* ...initialize thread attributes (joinable, with minimal stack) */
#ifdef HIFI_ONLY_XAF
    if ((r = __xf_thread_create(&proxy->thread, xf_proxy_thread, proxy, proxy_thread_name, xf_g_ap->proxy_thread_stack, PROXY_THREAD_STACK_SIZE, PROXY_THREAD_PRIORITY)) < 0)
#else
    if ((r = __xf_thread_create(&proxy->thread, xf_proxy_thread, proxy)) < 0)
#endif
    {
        TRACE(ERROR, _x("Failed to create polling thread: %d"), r);
        xf_ipc_close(&proxy->ipc, core);
        return r;
    }
        
    TRACE(INIT, _b("proxy-%u[%p] opened"), core, proxy);
    
    return 0;
}

/* ...close proxy handle */
void xf_proxy_close(xf_proxy_t *proxy)
{
    UWORD32     core = proxy->core;
    
    /* ...trigger proxy IPC interface close event */
    xf_ipc_close_set_event(&proxy->ipc, core);

    /* TENA-2117*/
    __xf_thread_join(&proxy->thread, NULL); //wait for the proxy to complete

    /* ...close proxy IPC interface */
    xf_ipc_close(&proxy->ipc, core); //close after proxy thread stops waiting in loop with ipc_wait()

    /* ...terminate proxy thread */
    __xf_thread_destroy(&proxy->thread);

    TRACE(INIT, _b("proxy-%u[%p] closed"), core, proxy);
}

/*******************************************************************************
 * HiFi component API
 ******************************************************************************/

/* ...open component handle */
int xf_open(xf_proxy_t *proxy, xf_handle_t *handle, xf_id_t id, UWORD32 core, xf_response_cb response)
{
    int     r;
    
    /* ...retrieve auxiliary control buffer from proxy - need I */
    XF_CHK_ERR(handle->aux = xf_buffer_get(proxy->aux), -EBUSY);

    /* ...initialize IPC data */
    XF_CHK_API(xf_ipc_data_init(&handle->ipc));    

    /* ...register client in interlocked fashion */
    xf_proxy_lock(proxy);

    /* ...allocate local client */
    if (xf_client_alloc(proxy, handle) == 0)
    {
        TRACE(ERROR, _x("client allocation failed"));
        r = -EBUSY;
    }
    else if ((r = xf_client_register(proxy, handle, id, core)) < 0)
    {
        TRACE(ERROR, _x("client registering failed"));
        xf_client_free(proxy, handle);
    }
    
    xf_proxy_unlock(proxy);

    /* ...if failed, release buffer handle */
    if (r < 0)
    {
        /* ...operation failed; return buffer back to proxy pool */
        xf_buffer_put(handle->aux), handle->aux = NULL;
    }
    else
    {
        /* ...operation completed successfully; assign handle data */
        handle->response = response;
        handle->proxy = proxy;

        TRACE(INIT, _b("component[%p]:(id=%s,core=%u) created"), handle, id, core);
    }
    
    return XF_CHK_API(r);
}

/* ...close component handle */
void xf_close(xf_handle_t *handle)
{
    xf_proxy_t *proxy = handle->proxy;

    /* ...do I need to take component lock here? guess no - tbd */

    /* ...buffers and stuff? - tbd */

    /* ...acquire global proxy lock */
    xf_proxy_lock(proxy);
    
    /* ...unregister component from remote DSP proxy (ignore result code) */
    (void) xf_client_unregister(proxy, handle);
    
    /* ...recycle client-id afterwards */
    xf_client_free(proxy, handle);

    /* ...release global proxy lock */
    xf_proxy_unlock(proxy);

    /* ...destroy IPC data */
    xf_ipc_data_destroy(&handle->ipc);
    
    /* ...clear handle data */
    xf_buffer_put(handle->aux), handle->aux = NULL;
    
    /* ...wipe out proxy pointer */
    handle->proxy = NULL;

    TRACE(INIT, _b("component[%p] destroyed"), handle);
}

/* ...port binding function */
int xf_route(xf_handle_t *src, UWORD32 src_port, xf_handle_t *dst, UWORD32 dst_port, UWORD32 num, UWORD32 size, UWORD32 align)
{
    xf_proxy_t             *proxy = src->proxy;
    xf_buffer_t            *b;
    xf_route_port_msg_t    *m;
    xf_user_msg_t           msg;
    
    /* ...sanity checks - proxy pointers are same */
    XF_CHK_ERR(proxy == dst->proxy, -EINVAL);
    
    /* ...buffer data is sane */
    XF_CHK_ERR(num && size && xf_is_power_of_two(align), -EINVAL);
    
    /* ...get control buffer */
    XF_CHK_ERR(b = xf_buffer_get(proxy->aux), -EBUSY);

    /* ...get message buffer */
    m = xf_buffer_data(b);
    
    /* ...fill-in message parameters */
    //m->src = __XF_PORT_SPEC2(src->id, src_port);
    m->dst = __XF_PORT_SPEC2(dst->id, dst_port);
    m->alloc_number = num;
    m->alloc_size = size;
    m->alloc_align = align;

    /* ...set command parameters */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(proxy->core), __XF_PORT_SPEC2(src->id, src_port));
    msg.opcode = XF_ROUTE;
    msg.length = sizeof(*m);
    msg.buffer = m;

    /* ...synchronously execute command on remote DSP */    
    XF_CHK_API(xf_proxy_cmd_exec(proxy, &msg));

    /* ...return buffer to proxy */
    xf_buffer_put(b);

    /* ...check result is successfull */
    XF_CHK_ERR(msg.opcode == XF_ROUTE, -ENOMEM);
    
    /* ...port binding completed */
    TRACE(GRAPH, _b("[%p]:%u bound to [%p]:%u"), src, src_port, dst, dst_port);
    
    return 0;
}

/* ...port unbinding function */
int xf_unroute(xf_handle_t *src, UWORD32 src_port)
{
    xf_proxy_t             *proxy = src->proxy;
    xf_buffer_t            *b;
    xf_unroute_port_msg_t  *m;
    xf_user_msg_t           msg;
    int                     r;
    
    /* ...get control buffer */
    XF_CHK_ERR(b = xf_buffer_get(proxy->aux), -EBUSY);

    /* ...get message buffer */
    m = xf_buffer_data(b);
    
    /* ...fill-in message parameters */
    //m->src = __XF_PORT_SPEC2(src->id, src_port);

    /* ...set command parameters */
    msg.id = __XF_MSG_ID(__XF_AP_PROXY(proxy->core), __XF_PORT_SPEC2(src->id, src_port));
    msg.opcode = XF_UNROUTE;
    msg.length = sizeof(*m);
    msg.buffer = m;

    /* ...synchronously execute command on remote DSP */    
    if ((r = xf_proxy_cmd_exec(proxy, &msg)) != 0)
    {
        TRACE(ERROR, _x("Command failed: %d"), r);
        goto out;
    }
    else if (msg.opcode != XF_UNROUTE)
    {
        TRACE(ERROR, _x("Port unbinding failed"));
        r = -EBADFD;
        goto out;
    }
    
    /* ...port binding completed */
    TRACE(GRAPH, _b("[%p]:%u unbound"), src, src_port);
    
out:
    /* ...return buffer to proxy */
    xf_buffer_put(b);
    
    return r;
}

/* ...send a command message to component */
int xf_command(xf_handle_t *handle, UWORD32 port, UWORD32 opcode, void *buffer, UWORD32 length)
{
    xf_proxy_t     *proxy = handle->proxy;
    xf_proxy_msg_t  msg;
    
    /* ...fill-in message parameters */
    msg.id = __XF_MSG_ID(__XF_AP_CLIENT(proxy->core, handle->client), __XF_PORT_SPEC2(handle->id, port));
    msg.opcode = opcode;
    msg.length = length;
    XF_CHK_ERR((msg.address = xf_proxy_b2a(proxy, buffer)) != XF_PROXY_BADADDR, -EINVAL);

    TRACE(CMD, _b("[%p]:[%08x]:(%08x,%u,%p)"), handle, msg.id, opcode, length, buffer);

    /* ...pass command to IPC layer */
    return XF_CHK_API(xf_ipc_send(&proxy->ipc, &msg, buffer));
}

/*******************************************************************************
 * Buffer pool API
 ******************************************************************************/

/* ...allocate buffer pool */
int xf_pool_alloc(xf_proxy_t *proxy, UWORD32 number, UWORD32 length, xf_pool_type_t type, xf_pool_t **pool, WORD32 id)
{
    xf_pool_t      *p;
    xf_buffer_t    *b;
    void           *data;
    int             r, ret;

    /* ...basic sanity checks; number of buffers is positive */
    XF_CHK_ERR(number > 0, -EINVAL);

    /* ...get properly aligned buffer length */
    length = (length + XF_PROXY_ALIGNMENT - 1) & ~(XF_PROXY_ALIGNMENT - 1);

    /* ...allocate data structure */

    ret = xaf_malloc((void **)&p, (offset_of(xf_pool_t, buffer) + number * sizeof(xf_buffer_t)), id); 
    if(ret != XAF_NO_ERROR)
        return ret;

    XF_CHK_ERR(p, -ENOMEM);

    /* ...issue memory pool allocation request to remote DSP */
    xf_proxy_lock(proxy);
    r = xf_proxy_buffer_alloc(proxy, number * length, &p->p);
    xf_proxy_unlock(proxy);
    
    /* ...if operation is failed, do cleanup */
    if (r < 0)
    {
        TRACE(ERROR, _x("failed to allocate buffer: %d"), r);
        xf_g_ap->xf_mem_free_fxn(p, id);
        return r;
    }
    else
    {
        /* ...set pool parameters */
        p->number = number, p->length = length;
        p->proxy = proxy;
    }
 
    /* ...create individual buffers and link them into free list */
    for (p->free = b = &p->buffer[0], data = p->p; number > 0; number--, b++)
    {
        /* ...set address of the buffer (no length there) */
        b->address = data;
            
        /* ...file buffer into the free list */
        b->link.next = b + 1;

        /* ...advance data pointer in contiguous buffer */
        data += length;
    }

    /* ...terminate list of buffers (not too good - tbd) */
    b[-1].link.next = NULL;

    TRACE(BUFFER, _b("[%p]: pool[%p] created: %u * %u"), proxy, p, p->number, p->length);
    
    /* ...return buffer pointer */
    *pool = p;
    
    return 0;
}

/* ...buffer pool destruction */
void xf_pool_free(xf_pool_t *pool, WORD32 id)
{
    xf_proxy_t     *proxy = pool->proxy;

    /* ...check buffers are all freed - tbd */

    /* ...use global proxy lock for pool operations protection */
    xf_proxy_lock(proxy);
    
    /* ...release allocated buffer on remote DSP */
    xf_proxy_buffer_free(proxy, pool->p, pool->length * pool->number);

    /* ...release global proxy lock */
    xf_proxy_unlock(proxy);
    
    /* ...deallocate pool structure itself */
    xf_g_ap->xf_mem_free_fxn(pool, id);

    TRACE(BUFFER, _b("[%p]::pool[%p] destroyed"), proxy, pool);
}

/* ...get new buffer from a pool */
xf_buffer_t * xf_buffer_get(xf_pool_t *pool)
{
    xf_buffer_t    *b;

    /* ...use global proxy lock for pool operations protection */
    xf_proxy_lock(pool->proxy);
    
    /* ...take buffer from a head of the free list */
    if ((b = pool->free) != NULL)
    {
        /* ...advance free list head */
        pool->free = b->link.next, b->link.pool = pool;

        TRACE(BUFFER, _b("pool[%p]::get[%p]"), pool, b);
    }

    xf_proxy_unlock(pool->proxy);
    
    return b;
}

/* ...return buffer back to pool */
void xf_buffer_put(xf_buffer_t *buffer)
{
    xf_pool_t  *pool = buffer->link.pool;
    
    /* ...use global proxy lock for pool operations protection */
    xf_proxy_lock(pool->proxy);
    
    /* ...put buffer back to a pool */
    buffer->link.next = pool->free, pool->free = buffer;
    
    TRACE(BUFFER, _b("pool[%p]::put[%p]"), pool, buffer);

    xf_proxy_unlock(pool->proxy);
}
