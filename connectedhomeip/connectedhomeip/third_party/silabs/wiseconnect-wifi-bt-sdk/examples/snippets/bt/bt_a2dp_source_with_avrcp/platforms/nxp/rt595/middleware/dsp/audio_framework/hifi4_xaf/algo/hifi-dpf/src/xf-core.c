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
 * xf-core.c
 *
 * DSP processing framework core
 ******************************************************************************/

#define MODULE_TAG                      CORE

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

/* ...general initialization sequence */
TRACE_TAG(INIT, 1);

/* ...message dispatching */
TRACE_TAG(DISP, 1);

/* ...client registration procedures */
TRACE_TAG(REG, 1);

/* ...ports routing/unrouting */
TRACE_TAG(ROUTE, 1);

/*******************************************************************************
 * Internal helpers
 ******************************************************************************/

/* ...translate client-id into component handle */
static inline xf_component_t * xf_client_lookup(xf_core_data_t *cd, UWORD32 client)
{
    xf_cmap_link_t *link = &cd->cmap[client];

    /* ...if link pointer is less than XF_CFG_MAX_CLIENTS, it is a free descriptor */
    return (link->next > XF_CFG_MAX_CLIENTS ? link->c : NULL);
}

/* ...allocate client-id */
static inline UWORD32 xf_client_alloc(xf_core_data_t *cd)
{
    UWORD32     client = cd->free;

    /* ...advance list head to next free id */
    (client < XF_CFG_MAX_CLIENTS ? cd->free = cd->cmap[client].next : 0);

    return client;
}

/* ...recycle client-id */
static inline void xf_client_free(xf_core_data_t *cd, UWORD32 client)
{
    /* ...put client into the head of the free id list */
    cd->cmap[client].next = cd->free, cd->free = client;
}

/*******************************************************************************
 * Process commands to a proxy
 ******************************************************************************/

/* ...register new client */
static int xf_proxy_register(UWORD32 core, xf_message_t *m)
{
    xf_core_data_t *cd = XF_CORE_DATA(core);
    UWORD32             src = XF_MSG_SRC(m->id);
    UWORD32             client;
    xf_component_t *component;
    
    /* ...allocate new client-id */
    XF_CHK_ERR((client = xf_client_alloc(cd)) != XF_CFG_MAX_CLIENTS, -EBUSY);
    
    /* ...create component via class factory */
    if ((component = xf_component_factory(core, m->buffer, m->length)) == NULL)
    {
        TRACE(ERROR, _x("Component creation failed"));
        
        /* ...recycle client-id */
        xf_client_free(cd, client);
        
        /* ...return generic out-of-memory code always (tbd) */
        return -ENOMEM;
    }

    /* ...register component in a map */
    cd->cmap[client].c = component;

    /* ...set component "default" port specification ("destination") */
    component->id = __XF_PORT_SPEC(core, client, 0);
    
    /* ...adjust session-id to include newly created component-id */
    m->id = __XF_MSG_ID(src, component->id);

    /* ...do system-specific registration of component within IPC layer */
    xf_ipc_component_addref(m->id);
    
    TRACE(REG, _b("registered client: %u:%u (%s)"), core, client, (xf_id_t)m->buffer);

    /* ...and return success to remote proxy (zero-length output) */
    xf_response_ok(m);
    
    return 0;
}

/* ...shared buffer allocation request */
static int xf_proxy_alloc(UWORD32 core, xf_message_t *m)
{
    /* ...command is valid only if shared memory interface for core is specified */
    XF_CHK_ERR(xf_shmem_enabled(core), -EPERM);

    /* ...allocate shared memory buffer (system-specific function; may fail) */
    xf_shmem_alloc(core, m);

    /* ...pass result to remote proxy (on success buffer is non-null) */
    xf_response(m);

    return 0;
}

/* ...shared buffer freeing request */
static int xf_proxy_free(UWORD32 core, xf_message_t *m)
{
    /* ...command is valid only if shared memory interface for core is specified */
    XF_CHK_ERR(xf_shmem_enabled(core), -EPERM);
    
    /* ...pass buffer freeing request to system-specific function */
    xf_shmem_free(core, m);

    /* ...return success to remote proxy (function never fails) */
    xf_response(m);
        
    return 0;
}

#if 0
/* ...port routing command processing */
static int xf_proxy_route(UWORD32 core, xf_message_t *m)
{
    xf_route_port_msg_t    *cmd = m->buffer;
    UWORD32                     src = cmd->src;
    UWORD32                     dst = cmd->dst;
    xf_component_t         *component;
    xf_output_port_t       *port;

    /* ...source component must reside on the local core */
    XF_CHK_ERR(XF_MSG_SRC_CORE(src) == core, -EINVAL);
    
    /* ...make sure the "src" component is valid ("dst" may reside on other core) */
    if ((component = xf_client_lookup(XF_CORE_DATA(core), XF_PORT_CLIENT(src))) == NULL)
    {
        TRACE(ERROR, _x("Source port lookup failed: %x"), src);
        return -ENOENT;
    }
    else if (!component->port || !(port = component->port(component, XF_PORT_ID(src))))
    {
        TRACE(ERROR, _b("Source port doesn't exist: %x"), src);
        return -ENOENT;
    }
    else if (xf_output_port_routed(port))
    {
        TRACE(ERROR, _b("Source port is already routed: %x"), src);
        return -EBUSY;
    }

    /* ...route output port with source port set as destination */
    XF_CHK_API(xf_output_port_route(port, __XF_MSG_ID(dst, src), cmd->alloc_number, cmd->alloc_size, cmd->alloc_align));
  
    TRACE(ROUTE, _b("Ports routed: %03x -> %03x"), src, dst);

    /* ...invoke component data-processing function directly (ignore errors? - tbd) */
    component->entry(component, NULL);

    /* ...return success result code (no output attached) */
    xf_response_ok(m);

    return 0;
}

/* ...disconnect ports */
static int xf_proxy_unroute(UWORD32 core, xf_message_t *m)
{
    xf_unroute_port_msg_t  *cmd = m->buffer;
    UWORD32                     src = cmd->src;
    xf_component_t         *component;
    xf_output_port_t       *port;

    /* ...source component must reside on the local core */
    XF_CHK_ERR(XF_MSG_SRC_CORE(src) == core, -EINVAL);

    /* ...lookup source (output) port */
    if ((component = xf_client_lookup(XF_CORE_DATA(core), XF_PORT_CLIENT(src))) == NULL)
    {
        TRACE(ERROR, _b("Source port lookup failed: %x"), src);
        return -ENOENT;
    }
    else if (!component->port || !(port = component->port(component, XF_PORT_ID(src))))
    {
        TRACE(ERROR, _b("Source port doesn't exist: %x"), src);
        return -ENOENT;
    }
    else if (!xf_output_port_routed(port))
    {
        /* ...port is not routed; satisfy immediately */
        goto done;
    }
    else if (!xf_output_port_idle(port))
    {
        TRACE(ERROR, _b("Source port is not idle: %x"), src);
        return -EBUSY;
    }

    /* ...unroute port (call must succeed) */
    xf_output_port_unroute(port);

    /* ...we cannot satisfy the command now, and need to propagate it to a sink - tbd */
    //return 0;

done:
    /* ...pass success result code to caller */
    xf_response_ok(m);

    return 0;
}
#endif

/* ...fill-this-buffer command processing */
static int xf_proxy_output(UWORD32 core, xf_message_t *m)
{
    /* ...determine destination "client" */
    switch (XF_MSG_SRC_CLIENT(m->id))
    {
#if XF_TRACE_REMOTE
    case 0:
        /* ...destination is a tracer facility; submit buffer to tracer */
        xf_trace_submit(core, m);
        return 0;
#endif

    default:
        /* ...unrecognized destination; return general failure response */
        return XF_CHK_ERR(0, -EINVAL);
    }
}

/* ...flush command processing */
static int xf_proxy_flush(UWORD32 core, xf_message_t *m)
{
    /* ...determine destination "client" */
    switch (XF_MSG_SRC_CLIENT(m->id))
    {
#if XF_TRACE_REMOTE
    case 0:
        /* ...destination is a tracer facility; flush current buffer */
        xf_trace_flush(core, m);
        return 0;
#endif

    default:
        /* ...unrecognized destination; return general failure response */
        return XF_CHK_ERR(0, -EINVAL);
    }
}

/* ...proxy command processing table */
static int (* const xf_proxy_cmd[])(UWORD32, xf_message_t *) = 
{
    [XF_OPCODE_TYPE(XF_REGISTER)] = xf_proxy_register,
    [XF_OPCODE_TYPE(XF_ALLOC)] = xf_proxy_alloc,
    [XF_OPCODE_TYPE(XF_FREE)] = xf_proxy_free,
#if 0
    [XF_OPCODE_TYPE(XF_ROUTE)] = xf_proxy_route,
    [XF_OPCODE_TYPE(XF_UNROUTE)] = xf_proxy_unroute,
#endif
    [XF_OPCODE_TYPE(XF_FILL_THIS_BUFFER)] = xf_proxy_output,
    [XF_OPCODE_TYPE(XF_FLUSH)] = xf_proxy_flush,
};

/* ...total number of commands supported */
#define XF_PROXY_CMD_NUM        (sizeof(xf_proxy_cmd) / sizeof(xf_proxy_cmd[0]))

/* ...process commands to a proxy */
static void xf_proxy_command(UWORD32 core, xf_message_t *m)
{
    UWORD32     opcode = m->opcode;
    int     res;

    /* ...dispatch command to proper hook */
    if (XF_OPCODE_TYPE(opcode) < XF_PROXY_CMD_NUM)
    {
        if ((res = xf_proxy_cmd[XF_OPCODE_TYPE(opcode)](core, m)) >= 0)
        {
            /* ...command processed successfully; do nothing */
            return;
        }
    }
    else
    {
        TRACE(ERROR, _x("invalid opcode: %x"), opcode);
    }

    /* ...command processing failed; return generic failure response */
    xf_response_err(m);
}

/*******************************************************************************
 * Message completion helper
 ******************************************************************************/

/* ...put message into local IPC command queue on remote core (src != dst) */
static inline void xf_msg_local_ipc_put(UWORD32 src, UWORD32 dst, xf_message_t *m)
{
    xf_core_rw_data_t  *rw = XF_CORE_RW_DATA(dst);    
    int                 first;
    
    /* ...flush message payload if needed */
    if (XF_LOCAL_IPC_NON_COHERENT)
    {
        /* ...it may be a command with output payload only - tbd */
        XF_PROXY_FLUSH(m->buffer, m->length);
    }

    /* ...acquire mutex to target rw-data (running on source core) */
    xf_mutex_lock(src);

    /* ...assure memory coherency as needed */
    if (XF_LOCAL_IPC_NON_COHERENT)
    {
        /* ...invalidate local queue data */
        XF_PROXY_INVALIDATE(&rw->local, sizeof(rw->local));
        
        /* ...place message into queue */
        first = xf_msg_enqueue(&rw->local, m);
        
        /* ...flush both queue and message data */
        XF_PROXY_FLUSH(&rw->local, sizeof(rw->local)), XF_PROXY_FLUSH(m, sizeof(*m));
    }
    else
    {
        /* ...just enqueue the message */
        first = xf_msg_enqueue(&rw->local, m);
    }
    
    /* ...release global rw-memory access lock */
    xf_mutex_unlock(src);

    /* ...signal IPI interrupt on destination core as needed */
    (first ? xf_ipi_assert(dst), 1 : 0);
}

/* ...dequeue message from core-specific dispatch queue */
static inline xf_message_t * xf_msg_local_ipc_get(UWORD32 core)
{
    xf_core_rw_data_t  *rw = XF_CORE_RW_DATA(core);
    xf_message_t       *m;
    
    /* ...retrieve message from queue in atomic fashion */
    xf_mutex_lock(core);

    /* ...process memory coherency as required */
    if (XF_LOCAL_IPC_NON_COHERENT)
    {
        /* ...inavlidate local rw-data */
        XF_PROXY_INVALIDATE(&rw->local, sizeof(rw->local));
        
        /* ...get message from the queue */
        if ((m = xf_msg_dequeue(&rw->local)) != NULL)
        {
            /* ...flush rw-queue data */
            XF_PROXY_FLUSH(&rw->local, sizeof(rw->local));
        }
    }
    else
    {
        /* ...just dequeue message from the queue */
        m = xf_msg_dequeue(&rw->local);
    }
    
    /* ...release rw-memory access lock */
    xf_mutex_unlock(core);

    /* ...invalidate message header and data as needed */
    if (XF_LOCAL_IPC_NON_COHERENT && m != NULL)
    {
        /* ...invalidate message header */
        XF_PROXY_INVALIDATE(m, sizeof(*m));
        
        /* ...and data if needed (it may not be always needed - tbd) */
        (m->length ? XF_PROXY_INVALIDATE(m->buffer, m->length) : 0);
    }

    /* ...return message */
    return m;
}

/* ...retrieve message from local queue (protected from ISR) */
static inline int xf_msg_local_put(UWORD32 core, xf_message_t *m)
{
    xf_core_data_t *cd = XF_CORE_DATA(core);
    int             first;
    UWORD32             status;
    
    /* ...use interrupt masking protocol to protect message queue */
    status = xf_isr_disable(core);
    first = xf_msg_enqueue(&cd->queue, m);
    xf_isr_restore(core, status);
    
    return first;
}

/* ...retrieve message from local queue (protected from ISR) */
static inline xf_message_t * xf_msg_local_get(UWORD32 core)
{
    xf_core_data_t *cd = XF_CORE_DATA(core);
    xf_message_t   *m;
    UWORD32             status;
    
    /* ...use interrupt masking protocol to protect message queue */
    status = xf_isr_disable(core);
    m = xf_msg_dequeue(&cd->queue);
    xf_isr_restore(core, status);
    
    return m;
}

/* ...retrieve message from local queue (protected from ISR) */
static inline xf_message_t * xf_msg_local_response_get(UWORD32 core)
{
    xf_core_data_t *cd = XF_CORE_DATA(core);
    xf_message_t   *m;
    UWORD32             status;
    
    /* ...use interrupt masking protocol to protect message queue */
    status = xf_isr_disable(core);
    m = xf_msg_dequeue(&cd->response);
    xf_isr_restore(core, status);
    
    return m;
}

/* ...call component data processing function */
static inline void xf_core_process(xf_component_t *component)
{
    UWORD32     id = component->id;
    
    /* ...client look-up successfull */
    TRACE(DISP, _b("core[%u]::client[%u]::process"), XF_PORT_CORE(id), XF_PORT_CLIENT(id));

    /* ...call data-processing interface */
    if (component->entry(component, NULL) < 0)
    {
        TRACE(ERROR, _b("execution error (ignored)"));
    }
}

/* ...dispatch message queue execution */
static inline void xf_core_dispatch(xf_core_data_t *cd, UWORD32 core, xf_message_t *m)
{
    UWORD32             client;
    xf_component_t *component;

    /* ...do client-id/component lookup */
    if (XF_MSG_DST_PROXY(m->id))
    {
        TRACE(DISP, _b("core[%u]::proxy-cmd(id=%08x, opcode=%08x)"), core, m->id, m->opcode);
        
        /* ...process message addressed to proxy */
        xf_proxy_command(core, m);

        /* ...do not like this return statement... - tbd */
        return;
    }

    /* ...message goes to local component */
    client = XF_MSG_DST_CLIENT(m->id);
    
    /* ...check if client is alive */
    if ((component = xf_client_lookup(cd, client)) != NULL)
    {
        /* ...client look-up successfull */
        TRACE(DISP, _b("core[%u]::client[%u]::cmd(id=%08x, opcode=%08x)"), core, client, m->id, m->opcode);
            
        /* ...pass message to component entry point */
        if (component->entry(component, m) < 0)
        {
            /* ...call component destructor */
            if (component->exit(component, m) == 0)
            {
                /* ...component cleanup completed; recycle component-id */
                xf_client_free(cd, client);

                /* ...do system-specific deregistration of component within IPC layer */
                xf_ipc_component_rmref(__XF_PORT_SPEC(core, client, 0));
            }
        }
    }
    else
    {
        TRACE(DISP, _b("Discard message id=%08x - client %u:%u not registered"), m->id, core, client);
        
        /* ...complete message with general failure response */   
        xf_response_err(m);
    }
}

/*******************************************************************************
 * Entry points
 ******************************************************************************/

/* ...submit message for instant execution on some core */
void xf_msg_submit(xf_message_t *m)
{
    UWORD32     src = XF_MSG_SRC_CORE(m->id);
    UWORD32     dst = XF_MSG_DST_CORE(m->id);

    /* ...check if message shall go through local IPC layer */
    if (src ^ dst)
    {
        /* ...put message into local IPC queue */
        xf_msg_local_ipc_put(src, dst, m);
    }
    else
    {
        /* ...message is addressed to same core */
        xf_msg_local_put(src, m);
    }
}

/* ...complete message and pass response to a caller */
void xf_msg_complete(xf_message_t *m)
{
    UWORD32     src = XF_MSG_SRC(m->id);
    UWORD32     dst = XF_MSG_DST(m->id);

    /* ...swap src/dst specifiers */
    m->id = __XF_MSG_ID(dst, src);

    /* ...check if message goes to remote IPC layer */
    if (XF_MSG_DST_PROXY(m->id))
    {
        /* ...return message to proxy */
        xf_msg_proxy_complete(m);
    }
    else
    {
        /* ...destination is within DSP cluster; check if that is a data buffer */
        switch (m->opcode)
        {
        case XF_EMPTY_THIS_BUFFER:
            /* ...emptied buffer goes back to the output port */
            m->opcode = XF_FILL_THIS_BUFFER;
            break;

        case XF_FILL_THIS_BUFFER:
            /* ...filled buffer is passed to the input port */
            m->opcode = XF_EMPTY_THIS_BUFFER;
            break;
        }

        /* ...submit message for execution */
        xf_msg_submit(m);
    }
}

/* ...initialize per-core framework data */
int xf_core_init(UWORD32 core)
{
    xf_core_data_t     *cd = XF_CORE_DATA(core);
    xf_cmap_link_t     *link;
    UWORD32                 i;
    
    /* ...create list of free client descriptors */
    for (link = &cd->cmap[i = 0]; i < XF_CFG_MAX_CLIENTS; i++, link++)
    {
        link->next = i + 1;
    }

    /* ...set head of free clients list */
    cd->free = 0;
    
    /* ...initialize local queue scheduler */
    xf_sched_init(&cd->sched);

    /* ...initialize IPI subsystem */
    XF_CHK_API(xf_ipi_init(core));
    
    /* ...initialize shared read-write memory */
    XF_CHK_API(xf_shmem_enabled(core) ? xf_shmem_init(core) : 0);

    /* ...initialize scratch memory */
    XF_CHK_ERR(cd->scratch = xf_scratch_mem_init(core), -ENOMEM);

    /* ...okay... it's all good */
    TRACE(INIT, _b("core-%u initialized"), core);
    
    return 0;
}

/* ...core executive loop function */
void xf_core_service(UWORD32 core)
{
    xf_core_data_t *cd = &(xf_g_dsp->xf_core_data[core]);
    UWORD32             status;
    xf_message_t   *m;
    xf_task_t      *t;

    do
    {           
        /* ...clear local status change */
        status = 0;
            
        /* ...if core is servicing shared memory with AP, do it first - actually, they all need to support it */
        if (xf_shmem_enabled(core))
        {
            /* ...process all commands */
            xf_shmem_process_queues(core);
        }

        /* ...check if we have a backlog message placed into interim queue */
        while ((m = xf_msg_local_ipc_get(core)) || (m = xf_msg_local_get(core)))
        {
            /* ...dispatch message execution */
            xf_core_dispatch(cd, core, m);

            /* ...set local status change */
            status = 1;
        }

        /* ...check if we have pending responses (submitted from ISR) we need to process */
        while ((m = xf_msg_local_response_get(core)) != NULL)
        {
            /* ...call completion handler on current stack */
            xf_msg_complete(m);

            /* ...set local status change */
            status = 1;
        }
            
        /* ...if scheduler queue is empty, break the loop and pause the core */
        if ((t = xf_sched_get(&cd->sched)) != NULL)
        {
            /* ...data-processing execution (ignore internal errors) */
            xf_core_process((xf_component_t *)t);

            /* ...set local status change */
            status = 1;
        }
    }
    while (status);
}

/* ...global data initialization function */
int xf_global_init(void)
{
    /* ...what global data we have to initialize? - tbd */
    TRACE(INIT, _b("Global data initialized"));
    
    return 0;
}
