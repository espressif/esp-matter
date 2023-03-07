/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 * For LibType_Custom build, force all ROM required modules to have
 * outPolicy = Types.SEPARATE_FILE
 */
var Types = xdc.useModule('xdc.runtime.Types');

var BIOS = xdc.useModule('ti.sysbios.BIOS');
BIOS.common$.outPolicy = Types.SEPARATE_FILE;

var BIOSRtsGateProxy = xdc.module('ti.sysbios.BIOS_RtsGateProxy');
BIOSRtsGateProxy.common$.outPolicy = Types.SEPARATE_FILE;

var Clock = xdc.useModule('ti.sysbios.knl.Clock');
Clock.common$.outPolicy = Types.SEPARATE_FILE;

var ClockTimerProxy = xdc.module('ti.sysbios.knl.Clock_TimerProxy');
ClockTimerProxy.common$.outPolicy = Types.SEPARATE_FILE;

var Task = xdc.useModule('ti.sysbios.knl.Task');
Task.common$.outPolicy = Types.SEPARATE_FILE;

var TaskSupportProxy = xdc.module('ti.sysbios.knl.Task_SupportProxy');
TaskSupportProxy.common$.outPolicy = Types.SEPARATE_FILE;

var Swi = xdc.useModule('ti.sysbios.knl.Swi');
Swi.common$.outPolicy = Types.SEPARATE_FILE;

var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');
Semaphore.common$.outPolicy = Types.SEPARATE_FILE;

var Event = xdc.useModule('ti.sysbios.knl.Event');
Event.common$.outPolicy = Types.SEPARATE_FILE;

var Mailbox = xdc.useModule('ti.sysbios.knl.Mailbox');
Mailbox.common$.outPolicy = Types.SEPARATE_FILE;

var Queue = xdc.useModule('ti.sysbios.knl.Queue');
Queue.common$.outPolicy = Types.SEPARATE_FILE;

var halHwi = xdc.useModule('ti.sysbios.hal.Hwi');
halHwi.common$.outPolicy = Types.SEPARATE_FILE;

var halHwiProxy = xdc.module('ti.sysbios.hal.Hwi_HwiProxy');
halHwiProxy.common$.outPolicy = Types.SEPARATE_FILE;

var m3Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');
m3Hwi.common$.outPolicy = Types.SEPARATE_FILE;

var TaskSupport = xdc.useModule('ti.sysbios.family.arm.m3.TaskSupport');
TaskSupport.common$.outPolicy = Types.SEPARATE_FILE;

var GateHwi = xdc.useModule('ti.sysbios.gates.GateHwi');
GateHwi.common$.outPolicy = Types.SEPARATE_FILE;

var GateMutex = xdc.useModule('ti.sysbios.gates.GateMutex');
GateMutex.common$.outPolicy = Types.SEPARATE_FILE;

var System = xdc.useModule('xdc.runtime.System');
System.common$.outPolicy = Types.SEPARATE_FILE;

var System_Module_GateProxy = xdc.module('xdc.runtime.System_Module_GateProxy');
System_Module_GateProxy.common$.outPolicy = Types.SEPARATE_FILE;

var SysCallback = xdc.module('xdc.runtime.SysCallback');
SysCallback.common$.outPolicy = Types.SEPARATE_FILE;

var Timestamp = xdc.useModule('xdc.runtime.Timestamp');
Timestamp.common$.outPolicy = Types.SEPARATE_FILE;

var Timestamp_SupportProxy = xdc.module('xdc.runtime.Timestamp_SupportProxy');
Timestamp_SupportProxy.common$.outPolicy = Types.SEPARATE_FILE;

var LocalError = xdc.module('xdc.runtime.Error');
LocalError.common$.outPolicy = Types.SEPARATE_FILE;

var Memory = xdc.module('xdc.runtime.Memory');
Memory.common$.outPolicy = Types.SEPARATE_FILE;

var Memory_HeapProxy = xdc.module('xdc.runtime.Memory_HeapProxy');
Memory_HeapProxy.common$.outPolicy = Types.SEPARATE_FILE;

var HeapMem = xdc.useModule('ti.sysbios.heaps.HeapMem');
HeapMem.common$.outPolicy = Types.SEPARATE_FILE;

var HeapMem_Module_GateProxy = xdc.module('ti.sysbios.heaps.HeapMem_Module_GateProxy');
HeapMem_Module_GateProxy.common$.outPolicy = Types.SEPARATE_FILE;

var HeapTrack = xdc.useModule('ti.sysbios.heaps.HeapTrack');
HeapTrack.common$.outPolicy = Types.SEPARATE_FILE;

/* use the cc26xx Timer and TimestampProvider modules */
var rtcTimer = xdc.useModule('ti.sysbios.family.arm.cc26xx.Timer');
rtcTimer.common$.outPolicy = Types.SEPARATE_FILE;

var rtcTimestamp = xdc.useModule('ti.sysbios.family.arm.cc26xx.TimestampProvider');
rtcTimestamp.common$.outPolicy = Types.SEPARATE_FILE;
