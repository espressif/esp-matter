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

/*
 * ======== UIAStatistic.xdc ========
 */
import xdc.runtime.Diags;

/*!
 * UIA Statistics Events
 *
 * The UIAStatistic module defines events that allow
 * tooling to analyze the performance of the software
 * (CPU Utilization, throughput, etc.)
 *
 *  The generation of UIAStatistic events is controlled by a module's diagnostics
 *  mask, which is described in details in `{@link xdc.runtime.Diags}`.
 * `UIAStatistic` events are generated only when the Diags.ANALYSIS bit is set
 *  in the module's diagnostics mask.
 *
 *  The following configuration script demonstrates how the application might
 *  control the logging of ANALYSIS events embedded in the `Mod` module at configuration
 *  time. In this case, the configuration script arranges for the `Log`
 *  statements within modules to always generate ANALYSIS events.
 *  Without these configuration statements, no ANALYSIS events would be generated
 *  by any modules.
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAStatistic = xdc.useModule('ti.uia.events.UIAStatistic');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerSys = xdc.useModule('xdc.runtime.LoggerSys');
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var logger = LoggerSys.create();
 *
 *  Defaults.common$.diags_ANALYSIS = Diags.ALWAYS_ON;
 *  Defaults.common$.logger = logger;
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of ANALYSIS events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAStatistic = xdc.useModule('ti.uia.events.UIAStatistic');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var Mod = xdc.useModule('my.pkg.Mod');
 *
 *  Mod.common$.diags_ANALYSIS = Diags.RUNTIME_OFF;
 *  @p
 *
 *  This is a part of the C code for the application:
 *
 *  @p(code)
 *  // turn on logging of ANALYSIS events in the module
 *  Diags_setMask("my.pkg.Mod+Z");
 *
 *  // turn off logging of ANALYSIS events in the module
 *  Diags_setMask("my.pkg.Mod-Z");
 *  @p
 */

module UIAStatistic inherits IUIAEvent {

    /*! ====== cpuLoad ======
     * Number of cycles used by an XDC module
     *
     * @a(Example)
     *   The following C code shows how to log a cpuLoad
     *   event that tracks the number of cycles used by an XDC module.
     *   The module ID is logged along with the event.  For non-XDC
     *   code, or for code with multiple instances @see #cpuLoadByInstance.
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAStatistic.h>
     *  ...
     * Void myFunction(){
     *   int numCycles = 0;
     *   // update numCycles with the number of cycles processed
     *   Log_write1(UIAStatistic_cpuLoad, numCycles);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  CPU Load: NumCycles=1234
     *  @p
     *
     *  @param(numCycles)           the CPU load in cycles
     */
    config xdc.runtime.Log.Event cpuLoad = {
        mask: Diags.ANALYSIS,
        msg: "CPU Load: NumCycles=%d"
    };

    /*! ====== cpuLoadByInstance ======
     * Number of cycles used by a non XDC module or thread
     *
     * @a(Example)
     *   The following C code shows how to log a cpuLoad
     *   event that tracks the number of cycles used by
     *   code that is not in an XDC module or by a thread
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAStatistic.h>
     *  static volatile int gMyGlobalInstanceId = 0;
     *  ...
     * Void myFunction(){
     *  IArg key;
     *  int localInstanceId;
     *  int numCycles = 0;
     * // protect pre-increment operation from race conditions
     *  key = Gate_enterSystem();
     *  localInstanceId = ++gMyGlobalInstanceId;
     *  Gate_leaveSystem(key);
     *   // update numCycles with the number of cycles processed
     *   Log_write3(UIAStatistic_cpuLoadByInstance, "myFunction",localInstanceId,numCycles);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  CPU Load for myFunction (instanceId = 0x1234): NumCycles=1234
     *  @param(name)                a constant string that provides the name of the entity that is processing the data
     *  @param(instanceId)          the instance ID (e.g. thread handle) of the entity that is processing the data
     *  @param(numCycles)           the CPU load in cycles
     */
    config xdc.runtime.Log.Event cpuLoadByInstance = {
        mask: Diags.ANALYSIS,
        msg: "CPU Load for %s (instanceId = 0x%x): NumCycles=%d"
    };

    /*! ====== bytesProcessed ======
     *  bytesProcessed statistic event
     *
     *  Number of bytes that were processed.
     *  @param(name)                a constant string that provides the name
     *                              of the entity that is processing the data
     *  @param(numBytes)            the number of bytes processed
     */
    config xdc.runtime.Log.Event bytesProcessed = {
        mask: Diags.ANALYSIS,
        msg: "Bytes Processed by %s: NumBytes=0x%x"
    };

    /*! ====== bytesProcessedByInstance ======
     * bytesProcessedByInstance statistic event
     *
     *  Number of bytes that were processed along with filename,
     *  line number and instance ID.
     * @a(Example)
     *   The following C code shows how to log an
     *   event that tracks the number of bytes processed
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAStatistic.h>
     *  ...
     * Void myFunction(){
     *   int instanceId = 0x1234; // change to e.g. a TaskId or some other unique ID
     *   int numBytes= 567;       // change to number of bytes actually processed
     *
     *   Log_write4(UIAStatistic_bytesProcessedByInstance, (IArg)__FILE__,(IArg)__LINE__,instanceId,numBytes);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Bytes Processed at Line 123 in demo.c (InstanceId 0x1234): Num Bytes=567
     *  @p
     *  @param(__FILE__)           constant string identifying the file the event was logged from
     *  @param(__LINE__)           the line number the event was logged from
     *  @param(instanceId)         the instance ID (e.g. thread handle) of the
     *                             entity that is processing the data
     *  @param(numBytes)           the number of bytes processed
     */
    config xdc.runtime.Log.Event bytesProcessedByInstance = {
        mask: Diags.ANALYSIS,
        msg: "Bytes Processed at %$F (InstanceId 0x%x): Num Bytes=%d"
    };

     /*!
     *  ======== metaEventBytesProcessedByInstance ========
     *  Metadata description of the bytesProcessedByInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventBytesProcessedByInstance = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STATISTIC,
        displayText: "Bytes Processed",
        tooltipText: "Bytes Processed",
        numParameters: 4,
        paramInfo: [
        {   name: '__FILE__',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: '__LINE__',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Instance ID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Words Processed',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'bytes',
            isHidden: false
        }]

    };


    /*! ====== wordsProcessed ======
     *  wordsProcessed statistic event
     *
     *  number of words that were processed.
     *  @param(name)                a constant string that provides the name
     *                              of the entity that is processing the data
     *  @param(numWords)            the number of words processed
     */
    config xdc.runtime.Log.Event wordsProcessed = {
        mask: Diags.ANALYSIS,
        msg: "Words Processed by %s: NumWords=0x%x"
    };


    /*! ====== wordsProcessedByInstance ======
     * wordsProcessedByInstance statistic event
     *
     *  Number of words that were processed along with filename,
     *  line number and instance ID.
     * @a(Example)
     *   The following C code shows how to log an
     *   event that tracks the number of words processed
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAStatistic.h>
     *  ...
     * Void myFunction(){
     *   int instanceId = 0x1234;  // change to e.g. a TaskId or some other unique ID
     *   int numWords= 567;        // change to number of words actually processed
     *
     *   Log_write4(UIAStatistic_wordsProcessedByInstance, (IArg)__FILE__,(IArg)__LINE__,instanceId,numWords);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Words Processed at Line 123 in demo.c (InstanceId 0x1234): Num Words=567
     *  @p
     *  @param(__FILE__)           constant string identifying the file the event was logged from
     *  @param(__LINE__)           the line number the event was logged from
     *  @param(instanceId)         the instance ID (e.g. thread handle) of the
     *                             entity that is processing the data
     *  @param(numWords)           the number of words processed
     */
    config xdc.runtime.Log.Event wordsProcessedByInstance = {
        mask: Diags.ANALYSIS,
        msg: "Words Processed at %$F (InstanceId 0x%x): Num Words=%d"
    };

     /*!
     *  ======== metaEventWordsProcessedByInstance ========
     *  Metadata description of the wordsProcessedByInstance event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventWordsProcessedByInstance = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STATISTIC,
        displayText: "Words Processed",
        tooltipText: "Words Processed",
        numParameters: 4,
        paramInfo: [
        {   name: '__FILE__',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: '__LINE__',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Instance ID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Words Processed',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'words',
            isHidden: false
        }]

    };

    /*! ====== freeBytes ======
     *  Number of free bytes in heap.
     *
     * @a(Example)
     *   The following C code shows how to log a freeBytes
     *   event that tracks the number of bytes free in the heap
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAStatistic.h>
     *  ...
     * Void myFunction(){
     *   int heapId = 0x1234;    // change to heap ID
     *   int numBytesFree = 567; // change to number of bytes free on the heap
     *
     *   Log_write4(UIAStatistic_freeBytes, (IArg)__FILE__,(IArg)__LINE__,heapId,numFreeBytes);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  Heap at Line 123 in demo.c (HeapId 0x1234): Free Bytes=567
     *  @p
     *  @param(__FILE__)           constant string identifying the file the event was logged from
     *  @param(__LINE__)           the line number the event was logged from
     *  @param(heapId)             heap identifier (e.g IHeap_Handle)
     *  @param(freeBytes)          the number of bytes free on the heap
     */
    config xdc.runtime.Log.Event freeBytes = {
        mask: Diags.ANALYSIS,
        msg: "Heap at %$F (HeapId 0x%x): Free Bytes=%d"
    };

    /*!
     *  ======== metaEventFreeBytes ========
     *  Metadata description of the FreeBytes event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventFreeBytes = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STATISTIC,
        displayText: "Free Bytes",
        tooltipText: "Free Bytes in Heap",
        numParameters: 4,
        paramInfo: [
        {   name: '__FILE__',
            dataDesc: DvtTypes.DvtDataDesc_FILENAMESTR,
            dataTypeName: 'String',
            units: 'none',
            isHidden: false
        },
        {   name: '__LINE__',
            dataDesc: DvtTypes.DvtDataDesc_LINENUM,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Heap ID',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'Free Bytes',
            dataDesc: DvtTypes.DvtDataDesc_VALUE,
            dataTypeName: 'Int',
            units: 'bytes',
            isHidden: false
        }]

    };
}
