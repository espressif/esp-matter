/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== package.xdc ========
 *
 */

requires ti.sysbios.interfaces;
requires ti.sysbios.family;

/*!
 *  ======== ti.sysbios.knl ========
 *  Contains core threading modules for the SYS/BIOS kernel.
 *
 *  Many real-time applications must perform a number of functions at the 
 *  same time, often in response to external events such as the availability of 
 *  data or the presence of a control signal. Both the functions and `when` 
 *  they are performed are important.
 *
 *  Such functions are called "threads". Within SYS/BIOS, the term is defined 
 *  broadly to include any independent stream of instructions executed by the 
 *  processor. A thread is a single point of control that can activate a 
 *  function call or an interrupt service routine (ISR).
 * 
 *  SYS/BIOS enables your applications to be structured as a collection of 
 *  threads. Multithreaded programs allow higher-priority threads to preempt 
 *  lower-priority threads and allow various types of interaction between 
 *  threads, including blocking, communication, and synchronization.
 *  
 *  SYS/BIOS provides support for several types of threads with different 
 *  priorities. Each thread type has different execution and preemption 
 *  characteristics. The thread types (from highest to lowest priority) are:
 *
 *  @p(blist)
 *  - Hardware interrupts ({@link ti.sysbios.hal.Hwi}), which includes 
 *    {@link ti.sysbios.hal.Timer} functions
 *  - Software interrupts ({@link ti.sysbios.knl.Swi}), which includes 
 *    {@link ti.sysbios.knl.Clock} functions
 *  - Tasks ({@link ti.sysbios.knl.Task})
 *  - Background thread ({@link ti.sysbios.knl.Idle})
 *  @p
 *
 *  The ti.sysbios.knl package also provides several modules for 
 *  synchronizing threads. 
 * 
 *  @p(blist)
 *  - Semaphores ({@link ti.sysbios.knl.Semaphore}) are often used to 
 *    coordinate access to a shared resource 
 *    among a set of competing tasks. Semaphores can be used for task 
 *    synchronization and mutual exclusion. Semaphore objects can be declared 
 *    as either counting or binary semaphores. The same APIs are used for  
 *    both counting and binary semaphores.
 *  - Events ({@link ti.sysbios.knl.Event}) provide a means for 
 *    communicating between and synchronizing 
 *    threads. They are similar to Semaphores, except that they allow you 
 *    to specify multiple conditions ("events") that must occur before the 
 *    waiting thread returns.
 *  - Mailboxes ({@link ti.sysbios.knl.Mailbox}) can be used to pass buffers 
 *    from one thread to another.
 *    A Mailbox instance can be used by multiple readers and writers.
 *  @p
 * 
 *  In addition to Semaphores, Events, and Mailboxes, you can use
 *  Gates to prevent concurrent accesses to critical regions of code. 
 *  The {@link xdc.runtime.Gate} module is provided by XDCtools, but 
 *  SYS/BIOS 
 *  provides some additional implementations in {@link ti.sysbios.gates}.
 */
package ti.sysbios.knl [2,0,0,0] {
    module Clock;
    module Idle;
    module Intrinsics;
    module Event;     // Must be before Semaphore!!!
    module Mailbox;
    module Queue;
    module Semaphore;
    module Swi;
    module Task;
}
