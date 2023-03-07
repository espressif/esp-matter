/*
 * Copyright (c) 2012-2019, Texas Instruments Incorporated
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
 */

requires ti.sysbios.interfaces;
requires ti.sysbios.family;

/*!
 *  ======== ti.sysbios.hal ========
 *  Contains Hardware Abstraction Layer (HAL) modules for the SYS/BIOS kernel.
 *
 *  SYS/BIOS provides services for configuration and management of interrupts,
 *  cache, and timers. Unlike other SYS/BIOS services such as threading, these
 *  modules directly program aspects of a device's hardware and are grouped
 *  together in the Hardware Abstraction Layer (HAL) package. These modules
 *  provide services such as enabling and disabling interrupts, plugging of
 *  interrupt vectors, multiplexing of multiple interrupts to a single vector,
 *  and cache invalidation or writeback.
 *
 *  The HAL APIs fall into two categories:
 *
 *  @p(blist)
 *  - Generic APIs that are available across all targets and devices.
 *  - Target/device-specific APIs that are available only for a specific device
 *    or ISA family
 *  @p
 *
 *  The generic APIs are designed to cover the great majority of use cases.
 *  If you are concerned with easy portability between
 *  different TI devices, you should use the generic APIs as much
 *  as possible. However, if the generic APIs cannot enable use of a
 *  device-specific hardware feature that is advantageous to the software
 *  application, you may choose to use the target/device-specific APIs.
 *
 *  The {@link ti.sysbios.hal.Hwi}, {@link ti.sysbios.hal.Timer}, and
 *  {@link ti.sysbios.hal.Cache} modules require target/device-specific API
 *  implementations to achieve their functionality. To provide a common set
 *  of APIs for these modules across all supported families/devices, SYS/BIOS
 *  uses the RTSC proxy-delegate module mechanism. That is, each of these
 *  modules serves as a proxy for a corresponding
 *  target/device-specific module implementation.
 *
 *  During the configuration step of the application build, the
 *  modules in the ti.sysbios.hal package locate and bind themselves to
 *  appropriate delegate module implementations based on the current target
 *  and platform specified in your config.bld file. The delegate binding
 *  process is done internally.
 *
 *  For example, if your config.bld file specifies that one of your build
 *  targets is the C64P family, the {@link ti.sysbios.hal.Hwi} knows to
 *  bind itself to the {@link ti.sysbios.family.c64p.Hwi} implementation.
 */
package ti.sysbios.hal [2,0,0,0] {
    module Cache;
    module CacheNull;
    module Core;
    module CoreNull;
    module Hwi;
    module Power;
    module PowerNull;
    module Seconds;
    module SecondsCallback;
    module SecondsClock;
    module Timer;
    module TimerNull;
}
