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
 * xf-component.h
 *
 * Xtensa processing framework component definition
 *******************************************************************************/

#ifndef __XF_H
#error "xf-component.h mustn't be included directly"
#endif

/*******************************************************************************
 * Types definitions
 ******************************************************************************/

/* ...component literal identifier */
typedef const char * const xf_id_t;

/* ...component descriptor (base structure) */
typedef struct xf_component
{
    /* ...scheduler node */
    xf_task_t               task;

    /* ...component id */
    UWORD32                     id;
    
    /* ...message-processing function */
    int                   (*entry)(struct xf_component *, xf_message_t *);

    /* ...component destructor function */
    int                   (*exit)(struct xf_component *, xf_message_t *);
    
}   xf_component_t;

/*******************************************************************************
 * Helpers
 ******************************************************************************/

/* ...return core-id of the component */
static inline UWORD32 xf_component_core(xf_component_t *component)
{
    return XF_PORT_CORE(component->id);
}

/* ...schedule component execution */
#define xf_component_schedule(c, dts)                                       \
({                                                                          \
    xf_sched_t *__sched = &XF_CORE_DATA(xf_component_core((c)))->sched;     \
    xf_sched_put(__sched, &(c)->task, xf_sched_timestamp(__sched) + (dts)); \
})

/* ...cancel component execution */
#define xf_component_cancel(c)                                          \
({                                                                      \
    xf_sched_t *__sched = &XF_CORE_DATA(xf_component_core((c)))->sched; \
    xf_sched_cancel(__sched, &(c)->task);                               \
})
    
/*******************************************************************************
 * API functions
 ******************************************************************************/

/* ...component factory */
extern xf_component_t * xf_component_factory(UWORD32 core, xf_id_t id, UWORD32 length);
