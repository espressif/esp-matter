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
 * xf-sched.c
 *
 * Non-preemptive earliest-deadline-first scheduler
 ******************************************************************************/

#define MODULE_TAG                      SCHED

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"

/*******************************************************************************
 * Tracing configuration
 ******************************************************************************/

TRACE_TAG(DEBUG, 1);

/*******************************************************************************
 * Global functions definitions
 ******************************************************************************/

/* ...place task into scheduler queue */
void xf_sched_put(xf_sched_t *sched, xf_task_t *t, UWORD32 ts)
{
    rb_tree_t  *tree = (rb_tree_t *)sched;
    rb_node_t  *node = (rb_node_t *)t;
    rb_idx_t    p_idx, t_idx;
    UWORD32         _ts;

    /* ...set scheduling timestamp */
    xf_task_timestamp_set(t, ts);
    
    /* ...find a place in the tree where the message should be inserted */
    for (p_idx = rb_root(tree); p_idx != rb_null(tree); p_idx = t_idx)
    {
        /* ...get timestamp of the p_idx */
        _ts = xf_task_timestamp((xf_task_t *)p_idx);

        /* ...ordering respects FIFO order of messages with same timestamp */
        if (xf_timestamp_before(ts, _ts))
        {
            if ((t_idx = rb_left(tree, p_idx)) == rb_null(tree))
            {
                /* ...p_idx is a direct successor of the message */
                rb_set_left(tree, p_idx, node);

                /* ...adjust head of the tree if needed */
                if (p_idx == rb_cache(tree))    goto insert_head;
                else                            goto insert;
            }
        }
        else
        {
            if ((t_idx = rb_right(tree, p_idx)) == rb_null(tree))
            {
                /* ...p_idx is a direct predeccessor of the message */
                rb_set_right(tree, p_idx, node);

                goto insert;
            }
        }        
    }

insert_head:
    /* ...adjust scheduler head element */
    rb_set_cache(tree, node);

insert:    
    /* ...insert item and rebalance the tree */
    rb_insert(tree, node, p_idx);

    /* ...head cannot be NULL */
    BUG(rb_cache(tree) == rb_null(tree), _x("Invalid scheduler state"));
    
    TRACE(DEBUG, _b("in:  %08x:[%p] (ts:%08x)"), ts, node, xf_sched_timestamp(sched));
}

/* ...get first item from the scheduler */
xf_task_t * xf_sched_get(xf_sched_t *sched)
{
    rb_tree_t      *tree = (rb_tree_t *)sched;
    rb_idx_t        n_idx, t_idx;
    UWORD32             ts;
    
    /* ...head of the tree is cached; replace it with its parent (direct successor) */
    if ((n_idx = rb_cache(tree)) == rb_null(tree))
    {
        /* ...tree is empty; bail out */
        return NULL;
    }
    else
    {
        /* ...delete current node and rebalance the tree */
        t_idx = rb_delete(tree, n_idx), rb_set_cache(tree, t_idx);

        /* ...get task timestamp */
        ts = xf_task_timestamp((xf_task_t *)n_idx);
        
        /* ...advance scheduler timestamp */
        xf_sched_timestamp_set(sched, ts);
        
        TRACE(DEBUG, _b("out: %08x:[%p]"), ts, n_idx);
        
        /* ...return task */
        return (xf_task_t *)n_idx;
    }
}

/* ...cancel specified task execution (must be scheduled!) */
void xf_sched_cancel(xf_sched_t *sched, xf_task_t *t)
{
    rb_tree_t      *tree = (rb_tree_t *)sched;
    rb_idx_t        n_idx = t;
    rb_idx_t        t_idx;

    /* ...delete message from tree */
    t_idx = rb_delete(tree, n_idx);

    /* ...adjust head if that was the first message */
    (n_idx == rb_cache(tree) ? rb_set_cache(tree, t_idx), 1 : 0);
}

/* ...initialize scheduler data */
void xf_sched_init(xf_sched_t *sched)
{
    rb_init((rb_tree_t *)sched);
}
