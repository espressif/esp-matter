/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*!
 * @file ti/sysbios/knl/Queue.h
 * @brief Queue Manager.
 *
 * The Queue module makes available a set of functions that manipulate queue
 * objects accessed through handles of type Queue_Handle. Each queue contains a
 * linked sequence of zero or more elements referenced through variables of type
 * Queue_Elem, which are embedded as the first field within a structure.
 *
 * In the Queue API descriptions, the APIs which disable interrupts before
 * modifying the Queue are noted as "atomic", while APIs that do not disable
 * interrupts are "non-atomic".
 *
 * Queues are represented as doubly-linked lists, so calls to Queue_next or
 * Queue_prev can loop continuously over the Queue. The following code
 * demonstrates one way to iterate over a Queue once from beginning to end. In
 * this example, 'myQ' is a Queue_Handle.
 *
 * @code
 *  Queue_Elem *elem;
 *
 *  for (elem = Queue_head(myQ);
 *      elem != (Queue_Elem *)myQ;
 *      elem = Queue_next(elem)) {
 *      ...
 *  }
 * @endcode
 *
 * Below is a simple example of how to create a Queue, enqueue two elements, and
 * dequeue the elements until the queue is empty:
 *
 * @code
 *  #include <xdc/std.h>
 *  #include <xdc/runtime/System.h>
 *
 *  #include <ti/sysbios/knl/Queue.h>
 *
 *  typedef struct Rec {
 *      Queue_Elem _elem;
 *      int data;
 *  } Rec;
 *
 *  int main(int argc, char *argv[])
 *  {
 *      Queue_Handle q;
 *      Rec r1, r2;
 *      Rec* rp;
 *
 *      r1.data = 100;
 *      r2.data = 200;
 *
 *
 *      // create a Queue instance 'q'
 *      q = Queue_create(NULL, NULL);
 *
 *
 *      // enQ a couple of records
 *      Queue_enqueue(q, &r1._elem);
 *      Queue_enqueue(q, &r2._elem);
 *
 *
 *      // deQ the records and print their data values until Q is empty
 *      while (!Queue_empty(q)) {
 *          rp = Queue_dequeue(q);
 *          System_printf("rec: %d\n", rp->data);
 *      }
 *
 *      System_exit(0);
 *      return (0);
 *  }
 * @endcode
 *
 * Unconstrained Functions All functions are unconstrained
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> Queue_create          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_insert          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_next            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_Params_init     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_prev            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_remove          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_construct       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_delete          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_dequeue         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_destruct        </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_empty           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_enqueue         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_get                     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_head            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Queue_put                     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. Queue_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Queue_Module_startupDone() returns false).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Queue.h ========
 */

#ifndef ti_sysbios_knl_Queue__include
#define ti_sysbios_knl_Queue__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Queue_long_names
#include "Queue_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Opaque queue element.
 *
 * A field of this type is placed at the head of client structs.
 */
typedef struct Queue_Elem {
    /*!
     * @brief Opaque queue element.
     *
     * A field of this type is placed at the head of client structs.
     */
    struct Queue_Elem *volatile next;
    /*!
     * @brief Opaque queue element.
     *
     * A field of this type is placed at the head of client structs.
     */
    struct Queue_Elem *volatile prev;
} Queue_Elem;

typedef Queue_Elem Queue_Struct;
typedef Queue_Struct Queue_Object;
typedef Queue_Struct *Queue_Handle;

typedef struct {
    /*! @cond NODOC */
    /*!
     * @brief Added to get the Grace instance view to work.
     */
    uint32_t dummy;    /* to make IAR happy */
    /*! @endcond */
} Queue_Params;

/*! @cond NODOC */
extern void Queue_Instance_init(Queue_Object *obj, const Queue_Params *prms);
/*! @endcond */

/*!
 * @brief Create a Queue object
 * 
 * This function creates a new Queue object.
 * 
 * @param prms queue parameters
 * @param eb error block
 *
 * @retval Queue handle (NULL on failure)
 */
extern Queue_Handle Queue_create(const Queue_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a queue
 *
 * Queue_construct is equivalent to Queue_create except that the
 * Queue_Struct is pre-allocated.  See Queue_create() for a description
 * of this API.
 *
 * @param obj pointer to a Queue object
 * @param prms queue parameters
 *
 * @retval Queue handle (NULL on failure)
 */
extern Queue_Handle Queue_construct(Queue_Struct *obj, const Queue_Params *prms);

/*!
 * @brief Delete a queue
 *
 * Queue_delete deletes a Queue object. Note that Queue_delete takes
 * a pointer to a Queue_Handle which enables Queue_delete to set the
 * Queue_handle to NULL.
 * 
 * @param queue pointer to Task handle
 */
extern void Queue_delete(Queue_Handle *queue);

/*!
 * @brief Destruct a queue
 *
 * Queue_destruct destructs a Queue object.
 *
 * @param obj pointer to Queue object
 */
extern void Queue_destruct(Queue_Struct *obj);

/*!
 * @brief Initialize the Queue_Params structure with default values.
 *
 * Queue_Params_init initializes the Queue_Params structure with default
 * values. Queue_Params_init should always be called before setting individual
 * parameter fields. This allows new fields to be added in the future with
 * compatible defaults -- existing source code does not need to change when
 * new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Queue_Params_init(Queue_Params *prms);

/*! @cond NODOC */
/*!
 * @brief Clears a Queue element's pointers so that if isQueued() is called on
 * the element it will return false. When a Queue element is dequeued or removed
 * from a Queue, this API must be called on the element for  isQueued() to
 * return false.
 *
 * To be clear, this API is not for removing elements from a queue, and should
 * never be called on an element in a queue--only on dequeued elements.
 *
 * @param qelem element to be cleared
 */
extern void Queue_elemClear(Queue_Elem *qelem);
/*! @endcond */

/*!
 * @brief Insert `elem` in the queue in front of `qelem`.
 *
 * @param qelem element already in queue
 *
 * @param elem element to be inserted in queue
 */
extern void Queue_insert(Queue_Elem *qelem, Queue_Elem *elem);

/*!
 * @brief Return next element in queue (non-atomically).
 *
 * This function returns a pointer to an Elem object in the queue after `qelem`.
 * A Queue is represented internally as a doubly-linked list, so 'next' can be
 * called in a continuous loop over the queue. See the module description for an
 * example of iterating once over a Queue.
 *
 * @param qelem element in queue
 * 
 * @retval next element in queue
 */
extern void * Queue_next(Queue_Elem *qelem);

/*!
 * @brief Return previous element in queue (non-atomically).
 *
 * This function returns a pointer to an Elem object in the queue before
 * `qelem`. A Queue is represented internally as a doubly-linked list, so 'prev'
 * can be called in a continuous loop over the queue. See the module description
 * for an example of iterating once over a Queue.
 *
 * @param qelem element in queue
 * 
 * @retval previous element in queue
 */
extern void * Queue_prev(Queue_Elem *qelem);

/*!
 * @brief Remove qelem from middle of queue (non-atomically).
 *
 * The `qelem` parameter is a pointer to an existing element to be removed from
 * the Queue.
 *
 * @param qelem element in queue
 */
extern void Queue_remove(Queue_Elem *qelem);

/*! @cond NODOC */
/*!
 * @brief Check if the elem is on any queue.
 *
 * In order for this API to return false on an element that has been dequeued or
 * removed from a Queue, elemClear must have been called on the element.
 *
 * @param qelem element in queue
 */
extern bool Queue_isQueued(Queue_Elem *qelem);
/*! @endcond */

/*!
 * @brief Remove the element from the front of queue and return elem (non-
 * atomically).
 *
 * This function removes an element from the front of a queue and returns it.
 *
 * If called with an empty queue, this function will return a pointer to the
 * queue itself.
 *
 * @note
 * empty Queue as shown in @ref Queue_get "Queue_get()" isn't reliable in a
 * multi-threaded system. Thread safety can be achieved as shown below:
 *
 * @code
 *  key = Hwi_disable();
 *
 *  if ((Queue_Handle)(elem = Queue_dequeue(q)) != q) {
 *      ` process elem `
 *  }
 *
 *  Hwi_restore(key);
 * @endcode
 * 
 * @param queue Queue handle
 * 
 * @retval pointer to former first element
 */
extern void * Queue_dequeue(Queue_Handle queue);

/*!
 * @brief Test for an empty queue.
 * 
 * @param queue Queue handle
 * 
 * @retval true if this queue is empty
 */
extern bool Queue_empty(Queue_Handle queue);

/*!
 * @brief Insert at end of queue (non-atomically).
 *
 * @param queue Queue handle
 * @param elem pointer to an element
 */
extern void Queue_enqueue(Queue_Handle queue, Queue_Elem *elem);

/*!
 * @brief Get element from front of queue (atomically).
 *
 * This function removes an element from the front of a queue and returns it.
 *
 * If called with an empty queue, this function will return a pointer to the
 * queue itself. This provides a means for using a single atomic action to check
 * if a queue is empty, and to remove and return the first element if it is not
 * empty:
 *
 * @code
 *  if ((Queue_Handle)(elem = Queue_get(q)) != q) {
 *      ` process elem `
 *  }
 * @endcode
 *
 * @param queue Queue handle
 * 
 * @retval pointer to former first element
 */
extern void * Queue_get(Queue_Handle queue);

/*!
 * @brief Get the element at the end of the queue (atomically).
 *
 * This function removes the element at the end of a queue and returns a pointer
 * to it. If called with an empty queue, this function will return a pointer to
 * the queue itself. This provides a means for using a single atomic action to
 * check if a queue is empty, and to remove and return the last element if it is
 * not empty:
 *
 * @code
 *  if ((Queue_Handle)(elem = Queue_getTail(q)) != q) {
 *      `process elem`
 *  }
 * @endcode
 * 
 * @param queue Queue handle
 * 
 * @retval pointer to former end element
 */
extern void * Queue_getTail(Queue_Handle queue);

/*!
 * @brief Return element at front of queue. (atomically)
 *
 * This function returns a pointer to the element at the front of a queue. The
 * element is not removed from the queue. If called with an empty queue, this
 * function will return a pointer to the queue itself. This provides a means for
 * using a single atomic action to check if a queue is empty, and to return a
 * pointer to the first element if it is not empty:
 *
 * @code
 *  if ((Queue_Handle)(elem = Queue_head(q)) != q) {
 *      `process elem`
 * @endcode
 * 
 * @param queue Queue handle
 *
 * @retval pointer to first element
 */
extern void * Queue_head(Queue_Handle queue);

/*!
 * @brief Put element at end of queue (atomically).
 *
 * @param queue Queue handle
 * @param elem pointer to new queue element
 */
extern void Queue_put(Queue_Handle queue, Queue_Elem *elem);

/*!
 * @brief Put element at the front of the queue (atomically).
 *
 * @param queue Queue handle
 * @param elem pointer to new queue element
 */
extern void Queue_putHead(Queue_Handle queue, Queue_Elem *elem);

/*! @cond NODOC */
static inline Queue_Handle Queue_handle(Queue_Struct *str)
{  
    return ((Queue_Handle)str);
}

static inline Queue_Struct * Queue_struct(Queue_Handle handle)
{  
    return ((Queue_Struct *)handle);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_knl_Queue__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Queue_long_names
#include "Queue_defs.h"
/*! @endcond */
