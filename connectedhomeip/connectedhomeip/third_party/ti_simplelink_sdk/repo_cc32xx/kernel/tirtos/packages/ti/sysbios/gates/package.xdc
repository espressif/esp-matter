/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
requires ti.sysbios.knl;

/*!
 *  ======== ti.sysbios.gates ========
 *  Contains modules that inherit from IGatePovider.
 *
 *  The {@link xdc.runtime.Gate} module is provided by XDCtools, and
 *  is documented in the XDCtools help, but SYS/BIOS provides
 *  additional implementations in the {@link ti.sysbios.gates} package.
 *
 *  A "Gate" is a module that implements the IGateProvider interface. 
 *  Gates are devices for preventing concurrent accesses to critical 
 *  regions of code. The various Gate implementations differ in how they 
 *  attempt to lock the critical regions.
 *  
 *  Threads can be preempted by other threads of higher priority, and 
 *  some sections of code need to be completed by one thread before they 
 *  can be executed by another thread. Code that modifies a linked list 
 *  is a common example of a critical region that may need to be protected 
 *  by a Gate.
 *  
 *  Gates generally work by either disabling some level of preemption such 
 *  as disabling task switching or even hardware interrupts, or by using a 
 *  binary semaphore.
 *  
 *  All Gate implementations support nesting through the use of a "key". 
 *  For Gates that function by disabling preemption, it is possible that 
 *  multiple threads would call Gate_enter(), but preemption should not be 
 *  restored until all of the threads have called Gate_leave(). 
 *  This functionality is provided through the use of a key. A call to 
 *  Gate_enter() returns a key that must then be passed back to Gate_leave(). 
 *  Only the outermost call to Gate_enter() returns the correct key for 
 *  restoring preemption. (The actual module name for the implementation 
 *  is used instead of "Gate" in the function name.)
 *  
 *  @a(Runtime Example)
 *  The following C code protects a critical region with 
 *  a Gate. This example uses a GateHwi, which disables and enables 
 *  interrupts as the locking mechanism.
 *  
 *  @p(code)
 *  UInt gateKey;
 *  GateHwi_Handle gateHwi;
 *  GateHwi_Params prms;
 *  GateHwi_Params_init(&prms);
 *  
 *  gateHwi = GateHwi_create(&prms, NULL);
 *  
 *  // Simultaneous operations on a List by multiple threads could
 *  // corrupt the List structure, so modifications to the List 
 *  // are protected with a Gate. 
 *  gateKey = GateHwi_enter(gateHwi);
 *  List_get(myList);
 *  GateHwi_leave(gateHwi, gateKey);
 *  @p
 *  
 */

package ti.sysbios.gates [2,0,0,0] {
    module GateHwi;
    module GateAll;
    module GateSwi;
    module GateTask;
    module GateTest;
    module GateMutexPri;
    module GateMutex;
    module GateSpinlock;
}
