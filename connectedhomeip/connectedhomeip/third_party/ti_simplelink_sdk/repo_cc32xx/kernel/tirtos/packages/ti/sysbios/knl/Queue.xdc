/*
 * Copyright (c) 2013-2017, Texas Instruments Incorporated
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
 */
/*
 *  ======== Queue.xdc ========
 *
 */
package ti.sysbios.knl;

import xdc.rov.ViewInfo;

/*!
 *  ======== Queue ========
 *  Queue manager.
 *
 *  The Queue module makes available a set of functions that manipulate
 *  queue objects accessed through handles of type Queue_Handle.
 *  Each queue contains a linked sequence of zero or more elements
 *  referenced through variables of type Queue_Elem, which are
 *  embedded as the first field within a structure.
 *
 *  In the Queue API descriptions, the APIs which disable interrupts before 
 *  modifying the Queue are noted as "atomic", while APIs that do not disable
 *  interrupts are "non-atomic".
 *
 *  Queues are represented as doubly-linked lists, so calls to Queue_next 
 *  or Queue_prev can loop continuously over the Queue. The following code
 *  demonstrates one way to iterate over a Queue once from beginning to end.
 *  In this example, 'myQ' is a Queue_Handle. 
 *  
 *  @p(code)
 *  Queue_Elem *elem;
 *
 *  for (elem = Queue_head(myQ); 
 *      elem != (Queue_Elem *)myQ; 
 *      elem = Queue_next(elem)) {
 *      ...
 *  }
 *  @p
 * 
 *  Below is a simple example of how to create a Queue, enqueue two elements,
 *  and dequeue the elements until the queue is empty:
 *
 *  @p(code)
 *  #include <xdc/std.h>
 *  #include <xdc/runtime/System.h>
 *  
 *  #include <ti/sysbios/knl/Queue.h>
 *  
 *  typedef struct Rec {
 *      Queue_Elem _elem;
 *      Int data;
 *  } Rec;
 *  
 *  Int main(Int argc, Char *argv[])
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
 *  @p
 *
 *
 *
 *  Unconstrained Functions
 *  All functions are unconstrained
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> {@link #create}          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #insert}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #next}            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #prev}            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #remove}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #construct}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #dequeue}         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}        </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #empty}           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #enqueue}         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #get}                     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #head}            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #put}                     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started 
 *    (e.g. Queue_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started 
 *    (e.g. Queue_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
@InstanceInitStatic /* Construct/Destruct CAN be called at runtime */
module Queue
{

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String  label;
        Ptr     elems[];
    }
    
    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitInstance', structName: 'BasicView'}]
            ]
        });
    
    /*!
     *  ======== Elem ========
     *  Opaque queue element.
     *
     *  A field of this type is placed at the head of client structs.
     */
    struct Elem {
        Elem *volatile next;
        Elem *volatile prev;
    };

    /*!
     *  @_nodoc
     *  ======== elemClear ========
     *  Clears a Queue element's pointers so that if isQueued() is called on
     *  the element it will return FALSE. When a Queue element is dequeued or
     *  removed from a Queue, this API must be called on the element for 
     *  isQueued() to return FALSE.     
     *
     *  To be clear, this API is not for removing elements from a queue, and
     *  should never be called on an element in a queue--only on dequeued 
     *  elements.
     *
     *  @param(qelem)           element to be cleared
     */
    Void elemClear(Elem *qelem); 

    /*!
     *  ======== elemClearMeta ========
     *  @_nodoc
     *  Clears a Queue element's pointers so that if isQueued() is called on
     *  the element it will return FALSE. When a Queue element is dequeued or
     *  removed from a Queue, this API must be called on the element for 
     *  isQueued() to return FALSE.     
     *
     *  To be clear, this API is not for removing elements from a queue, and
     *  should never be called on an element in a queue--only on dequeued 
     *  elements.
     *
     *  @param(qelem)           element to be cleared
     */
    metaonly Void elemClearMeta(Elem *qelem); 

    /*!
     *  ======== insert ========
     *  Insert `elem` in the queue in front of `qelem`.
     *
     *  @param(qelem)           element already in queue
     *
     *  @param(elem)            element to be inserted in queue
     */
    Void insert(Elem *qelem, Elem *elem); 

    /*!
     *  ======== insertMeta ========
     *  @_nodoc
     *  Insert `elem` in the queue in front of `qelem`.
     *
     *  @param(qelem)           element already in queue
     *
     *  @param(elem)            element to be inserted in queue
     */
    metaonly Void insertMeta(Elem *qelem, Elem *elem); 

    /*!
     *  ======== next ========
     *  Return next element in queue (non-atomically).
     *
     *  This function returns a pointer to an Elem object in the queue 
     *  after `qelem`. A Queue is represented internally as a doubly-linked
     *  list, so 'next' can be called in a continuous loop over the queue.
     *  See the module description for an example of iterating once over a
     *  Queue.
     *
     *  @param(qelem)           element in queue
     *
     *  @b(returns)             next element in queue
     */
    Ptr next(Elem *qelem);

    /*!
     *  ======== prev ========
     *  Return previous element in queue (non-atomically).
     *
     *  This function returns a pointer to an Elem object in the queue 
     *  before `qelem`. A Queue is represented internally as a doubly-linked
     *  list, so 'prev' can be called in a continuous loop over the queue.
     *  See the module description for an example of iterating once over a
     *  Queue.
     *
     *  @param(qelem)           element in queue
     *
     *  @b(returns)             previous element in queue
     */
    Ptr prev(Elem *qelem);

    /*!
     *  ======== remove ========
     *  Remove qelem from middle of queue (non-atomically).
     *
     *  The `qelem` parameter is a pointer to an existing element to be removed
     *  from the Queue.
     *
     *  @param(qelem)           element in queue
     */
    Void remove (Elem *qelem);

    /*!
     *  @_nodoc
     *  ======== isQueued ========
     *  Check if the elem is on any queue. 
     *  
     *  In order for this API to return false on an element that has been
     *  dequeued or removed from a Queue, elemClear must have been called on
     *  the element.
     *
     *  @param(qelem)           element in queue
     */
    Bool isQueued (Elem *qelem);

instance:

    /*!
     *  @_nodoc
     *  Added to get the Grace instance view to work.
     */
    metaonly config UInt dummy = 0;

    /*!
     *  ======== create ========
     *  Create a Queue object
     */
    create();

    /*!
     *  ======== dequeue ========
     *  Remove the element from the front of queue and return elem
     *  (non-atomically).
     *
     *  This function removes an element from the front of a queue and returns
     *  it.
     *
     *  If called with an empty queue, this function will return a pointer to
     *  the queue itself.
     *
     *  @a(note) As this function is non-atomic, the method for detecting an
     *  empty Queue as shown in {@link #get Queue_get()} isn't reliable in
     *  a multi-threaded system. Thread safety can be achieved as shown below:
     *
     *  @p(code)
     *  key = Hwi_disable();
     *
     *  if ((Queue_Handle)(elem = Queue_dequeue(q)) != q) {
     *      ` process elem `
     *  }
     *
     *  Hwi_restore(key);
     *  @p
     *
     *  @b(returns)             pointer to former first element
     */
    Ptr dequeue();

    /*!
     *  ======== empty ========
     *  Test for an empty queue.
     *
     *  @b(returns)             TRUE if this queue is empty
     */
    Bool empty();

    /*!
     *  ======== enqueue ========
     *  Insert at end of queue (non-atomically).
     *
     *  @param(elem)            pointer to an element
     */
    Void enqueue(Elem *elem);

    /*!
     *  ======== get ========
     *  Get element from front of queue (atomically).
     *
     *  This function removes an element from the front of a queue and returns
     *  it.
     *
     *  If called with an empty queue, this function will return a pointer to
     *  the queue itself.
     *  This provides a means for using a single atomic action to check if a
     *  queue is empty, and to remove and return the first element if it is
     *  not empty:
     *
     *  @p(code)
     *  if ((Queue_Handle)(elem = Queue_get(q)) != q) {
     *      ` process elem `
     *  }
     *  @p
     *
     *  @b(returns)             pointer to former first element
     */
    Ptr get();

    /*!
     *  ======== getTail ========
     *  Get the element at the end of the queue (atomically).
     *
     *  This function removes the element at the end of a queue and returns
     *  a pointer to it.  
     *  If called with an empty queue, this function will return a pointer to
     *  the queue itself.
     *  This provides a means for using a single atomic action to check if a
     *  queue is empty, and to remove and return the last element if it is
     *  not empty:
     *
     *  @p(code)
     *  if ((Queue_Handle)(elem = Queue_getTail(q)) != q) {
     *      `process elem`
     *  }
     *  @p
     *
     *  @b(returns)             pointer to former end element
     */
    Ptr getTail();

    /*!
     *  ======== head ========
     *  Return element at front of queue. (atomically)
     *
     *  This function returns a pointer to the element at the front of a queue.
     *  The element is not removed from the queue.
     *  If called with an empty queue, this function will return a pointer to
     *  the queue itself.
     *  This provides a means for using a single atomic action to check if a queue
     *  is empty, and to return a pointer to the first element if it is not empty:
     *
     *  @p(code)
     *  if ((Queue_Handle)(elem = Queue_head(q)) != q) {
     *      `process elem`
     *  @p
     *
     *  @b(returns)             pointer to first element
     */
    Ptr head();

    /*!
     *  ======== headMeta ========
     *  @_nodoc
     *  Return element at front of queue. Returns null if queue is empty.
     *
     *  This function returns a pointer to the element at the front of queue.
     *  The element is not removed from the queue.
     *
     *  @b(returns)             pointer to first element
     */
    metaonly Ptr headMeta();

    /*!
     *  ======== put ========
     *  Put element at end of queue (atomically).
     *
     *  @param(elem)            pointer to new queue element
     */
    Void put(Elem *elem);
    
    /*!
     *  ======== putMeta ========
     *  @_nodoc
     *  Put element at end of queue.
     *
     *  @param(elem)            pointer to new queue element
     */
    metaonly Void putMeta(Elem* elem);

    /*!
     *  ======== putHead ========
     *  Put element at the front of the queue (atomically).
     *
     *  @param(elem)            pointer to new queue element
     */
    Void putHead(Elem *elem);

    /*!
     *  ======== nextMeta ========
     *  @_nodoc
     *  Return next element in queue. Returns null if end of queue.
     *
     *  This function returns a pointer to an Elem object in the queue 
     *  after `qelem`.
     *
     *  @param(qelem)           element in queue
     *
     *  @b(returns)             next element in queue
     */
    metaonly Ptr nextMeta(Elem *qelem);

internal:   // not for client use

    // instance object
    struct Instance_State {
            Elem elem;
    };
}
