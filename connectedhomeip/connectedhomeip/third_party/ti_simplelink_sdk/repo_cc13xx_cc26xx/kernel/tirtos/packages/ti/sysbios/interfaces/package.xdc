/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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

/*!
 *  ======== ti.sysbios.interfaces ========
 *  Contains all interface specifications.
 *
 *  For the proxy-delegate mechanism between generic modules and the 
 *  modules in {@link ti.sysbios.family} to work properly, both the 
 *  proxy and the delegate modules must be implementations of a common 
 *  interface specification. The Timer, Hwi, Cache and Seconds interface
 *  specifications reside in ti.sysbios.interfaces and are 
 *  {@link ti.sysbios.interfaces.ITimer}, 
 *  {@link ti.sysbios.interfaces.IHwi},
 *  {@link ti.sysbios.interfaces.ICache}, and
 *  {@link ti.sysbios.interfaces.ISeconds} respectively.
 *
 *  These interface specifications define a minimum set of general APIs
 *  that, it is  believed, will satisfy a vast majority of application
 *  requirements. 
 *  
 *  The I<mod>Support interfaces define "internal" interfaces used by the
 *  <mod> modules in their implementation.  These interfaces simplify the
 *  process of porting to a new architecture; one only needs to implement
 *  the I<mod>Support interface to enable the full functionality of the
 *  ti.sysbios.<mod> module.
 *
 */
package ti.sysbios.interfaces [2,0,0,0] {
    interface ICore;
    interface IHwi;
    interface ITaskSupport;
    interface ITimer;
    interface ITimerSupport;
    interface ITimestamp;
    interface IIntrinsicsSupport;
    interface ICache;
    interface ISettings;
    interface IPower;
    interface IRomDevice;
    interface ISeconds;
}
