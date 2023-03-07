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
 * */

import ti.uia.runtime.IUIATraceSyncProvider;

/*
 * ======== IUIATraceSyncClient.xdc ========
 *  Interface for client modules that can inject info into a
 *  device trace stream whenever a sync point event, snapshot event and / or
 *  context change event occurs.  Using a module that implements IUIATraceSyncProvider
 *  will cause that module to automatically register its callback function
 *  with any modules that implement the IUIATraceSyncClient interface.
 *  @p
 *  Examples of modules that implement this interface include
 *  {@link ti.uia.family.c64p.GemTraceSync ti.uia.family.c64p.GemTraceSync} and
 *  {@link ti.uia.family.c66.GemTraceSync ti.uia.family.c66.GemTraceSync}
 */
interface IUIATraceSyncClient inherits ti.uia.events.IUIAMetaProvider {

    /*!
     * ======== injectIntoTraceFxn ========
     * Callback function that handles injection of info such as serial numbers
     * of sync point events, context change events or snapshot events into a
     * hardware trace stream. (e.g. GEM CPU Trace, System Trace, etc.)
     *
     * Users can provide their own custom injectIntoTraceFxn to log whatever
     * additional information they wish to record when the hook function is called.
     * For example, event serial numbers can be injected into the CPU
     * trace stream and / or STM trace stream in order to enable correlation
     * of information logged in these streams with UIA software events.
     * @a(Examples)
     * @p(html)
     * <B>Example 1: Correlating events with C64X+ and C66 CPU Trace</B>
     * @p
     * The following is an example of the configuration script used
     * to inject serial numbers of sync point events or context change events or
     * snapshot Ids associated with snapshot events.
     * @p
     * Note that the GemTraceSync module's .xs script takes care of finding
     * all modules that implement the IUIATraceSyncClient and assigning the
     * GemTraceSync_injectIntoTrace function pointer to those modules'
     * injectIntoTraceFxn config option.
     * @p(code)
     * //The following 3 modules all implement the IUIATraceSyncClient interface
     * var LogSnapshot   = xdc.useModule('ti.uia.runtime.LogSnapshot');
     * var LogCtxChg   = xdc.useModule('ti.uia.runtime.LogCtxChg');
     * var LogSync   = xdc.useModule('ti.uia.runtime.LogSync');
     * //For C66 devices, replace the following line with
     * // var GemTraceSync = xdc.useModule('ti.uia.family.c66.GemTraceSync');
     * var GemTraceSync = xdc.useModule('ti.uia.family.c64p.GemTraceSync');
     * @p
     * @p(html)
     * <hr>
     * <B>Example 2: How to create a custom hook function
     * and assign it to the LogSnapshot module</B>
     * @p
     * The following is an example of a 'C' code program that implements
     * a hook function that prints out the snapshot ID that is passed in
     * as the serialNumber
     * @p(code)
     * #include <xdc/std.h>
     * #include <xdc/runtime/Gate.h>
     * #include <ti/uia/runtime/IUIATraceSyncProvider.h>
     * #include <ti/uia/runtime/LogSnapshot.h>
     * #include <stdio.h>
     * #include <string.h>
     * extern Void myHookFxn(UInt32 serialNumber, IUIATraceSyncProvider_ContextType ctxType);
     * Void Test();
     * char name[32]={"Bob"};
     * UInt32 newAppId = 0;
     * Void myHookFxn(UInt32 serialNumber, IUIATraceSyncProvider_ContextType ctxType){
     *      volatile UInt32 syncWord;
     *      IArg key = Gate_enterSystem();
     *      printf("newAppId written with serialNumber %d and ctxType = %d\n",serialNumber,ctxType);
     *      Gate_leaveSystem(key);
     * }
     * Void Test(){
     *     // note that the hook function is triggered by calling LogSnapshot_getSnapshotId()
     *     // since that is where the unique snapshot ID that is passed to the
     *     // hook function is generated.
     *     Int snapshotId = LogSnapshot_getSnapshotId();
     *     LogSnapshot_writeString(snapshotId,"User-defined name=%s.",name, strlen(name));
     * }
     *
     * Void main(){
     *     while(TRUE){  Test();  }
     *  }
     * @p
     * In order to have the above user-defined function called by the LogSnapshot
     * module whenever it writes an event, the following configuration script
     * is needed:
     * @p(code)
     * var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
     * var LogSnapshot   = xdc.useModule('ti.uia.runtime.LogSnapshot');
     * var IUIATraceSyncClient = xdc.useModule('ti.uia.runtime.IUIATraceSyncClient');
     * LogSnapshot.injectIntoTraceFxn = $externFxn('myHookFxn');
     *
     * @p
     * @see LoggerTypes#InjectIntoTraceFxn
     * @see LogSnapshot
     */
    config LoggerTypes.InjectIntoTraceFxn injectIntoTraceFxn = null;
    /*!
     * ======== isInjectIntoTraceEnabled ========
     * set false to turn off injection of sync point info into trace even
     *  if a module that implements IUIATraceSyncProvider is configured.
     *
     * The XDCScript associated with a module that implements IUIATraceSyncProvider
     * is responsible for checking this config option for all IUIATraceSyncClient
     * modules before automatically configuring the client callback function.
     * This allows users to control which features have sync points injected
     * into the trace stream.  For example, a user may wish to configure
     * LogSync.isInjectIntoTraceEnabled = true and
     * LogCtxChg.isInjectIntoTraceEnabled = false in order to reduce the number
     * of sync point events injected into the trace stream.
     */
    metaonly config Bool isInjectIntoTraceEnabled = true;

}
