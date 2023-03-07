/*
 * Copyright (c) 2012-2013, Texas Instruments Incorporated
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
 * ======== UIAProfile.xdc ========
 */

import xdc.runtime.Diags;
import xdc.runtime.Types;
import ti.uia.events.DvtTypes;

/*!
 * UIA Profile Events
 *
 * The UIAProfile module defines events that allow
 * tooling to analyze the performance of the software
 * (processing time, latency, etc.).  These events are
 * designed to be logged from function-entry and function-exit
 * hook functions that are called by compiler-generated code
 *
 * TI compilers can be configured to either pass in a parameter
 * to the hook functions containing either the address
 * of the function or the name of the function.
 *
 * If the compiler is configured to pass in the address of the function,
 * the `UIAProfile_enterFunctionAdrs` event should be logged by the
 * entry hook function and the `UIAProfile_exitFunctionAdrs` event
 * should be logged by the exit hook function.
 *
 * If the compiler is configured to pass in the name of the function,
 * the `UIAProfile_enterFunctionName` event should be logged by the
 * entry hook function and the `UIAProfile_exitFunctionName` event
 * should be logged by the exit hook function.
 *
 * When logging events using the xdc.runtime.Log module, the generation
 * of UIAProfile events is controlled by the `Diags.ENTRY` and `Diags.EXIT`
 * flags in a module's diagnostics  mask.  (For more information on
 * diagnostics masks, please see the xdc.runtime.Diags documentation.)
 *
 * By default, the UIAProfile module will automatically set both the
 * `Main.common$.Diags_ENTRY` and `Main.common$.Diags_EXIT` flags to
 * `Diags.ALWAYS_ON` if these flags have not been previously configured.
 * To turn off these flags at configuration time, set
 * `UIAProfile.enable = false;`  To allow these flags to be configured
 * at run time, set `UIAProfile.runtimeControl = true;`
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAProfile = xdc.useModule('ti.uia.events.UIAProfile');
 *  var LoggingSetup = xdc.useModule('ti.uia.sysbios.LoggingSetup');
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 *
 *  Example 2: The following example configures a module to support logging
 *  of ENTRY and EXIT events, but defers the actual activation and deactivation of the
 *  logging until runtime. See the `{@link Diags#setMask Diags_setMask()}`
 *  function for details on specifying the control string.
 *
 *  This is a part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  var UIAProfile = xdc.useModule('ti.uia.events.UIAProfile');
 *  UIAProfile.runtimeControl = true;
 *  UIAProfile.enable = false;
 *  @p
 *
 *  This is a part of the C code for the application.
 *  The diags_ENTRY mask is set by "E", and the diags_EXIT mask is set by "X".
 *
 *  @p(code)
 *  #include <xdc/runtime/Diags.h>
 *  #include <xdc/runtime/Main.h>
 *
 *  // turn on logging of ENTRY and EXIT events in the module
 *  Diags_setMask("xdc.runtime.Main+EX");
 *
 *  // turn off logging of ENTRY and EXIT events in the module
 *  Diags_setMask("xdc.runtime.Main-EX");
 *  @p
 *
 *
 */
@Template("./UIAProfile.xdt")
module UIAProfile inherits IUIAEvent {

    /*!
     *  ======== enterFunctionAdrs ========
     *  Profiling event used to log the entry point of a function
     *
     * @a(Example)
     * To add entry and exit hook functions to every function
     * 1. Use the following compiler options when compiling the source
     *  @p(code)
     *  --entry_hook=functionEntryHook
     *  --entry_param=address
     *  --exit_hook=functionExitHook
     *  --exit_param=address
     *  @p
     * 2. Add the following c code to implement the hook functions:
     *   The first parameter (the taskHandle) is set to  0 in this example.
     *   @see exitFunction for an example of how to log the current task ID
     *   for task-aware function profiling.
     *   In order to further reduce the CPU overhead of logging the
	 *   UIAProfile events, you can use the LogUC.h APIs. For more info, please
	 *   see @link http://processors.wiki.ti.com/index.php/SystemAnalyzerTutorial1F
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAProfile.h>
     *  ...
     * void functionEntryHook( void (*adrs)() ){
     *    Log_write2(UIAProfile_enterFunctionAdrs, 0,(IArg)adrs);
     *   ...
     * void functionExitHook( void (*adrs)() ){
     *    Log_write2(UIAProfile_exitFunctionAdrs, 0,(IArg)adrs);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  enterFunctionAdrs: taskHandle=0x0, adrs=0x820060
     *  exitFunctionAdrs: taskHandle0x0, adrs=0x820060
     *  @p
     *  @param(taskHandle)   task handle that identifies the currently active task (use 0 if not required)
     *  @param(functionAdrs) the address of a function that can differentiate this pair of start and stop events from others
     */
    config xdc.runtime.Log.Event enterFunctionAdrs = {
        mask: Diags.ENTRY,
        msg: "enterFunctionAdrs: taskHandle=0x%x, adrs=0x%x"
    };

    /*!
     *  ======== metaEventEnterFunctionAdrs ========
     *  Metadata description of the enterFunctionAdrs event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventEnterFunctionAdrs = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "enterFunctionAdrs",
        tooltipText: "function entry",
        numParameters: 2,
        paramInfo: [
        {   name: 'Qualifier',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };


    /*!
     *  ======== exitFunctionAdrs ========
     *  Profiling event used to log the exit point of a function
     *
     * @a(Example)
     * To add entry and exit hook functions to every function
     * 1. Use the following compiler options when compiling the source
     *  @p(code)
     *  --entry_hook=functionEntryHook
     *  --entry_param=address
     *  --exit_hook=functionExitHook
     *  --exit_param=address
     *  @p
     * 2. Add the following c code to implement the hook functions:
     *   Task_selfMacro() is used to get the current task handle in this example.
     *   @see enterFunction for an example of how to save CPU by logging 0
     *   instead of the task handle if task-aware profiling is not required.
     *   In order to further reduce the CPU overhead of logging the
	 *   UIAProfile events, you can use the LogUC.h APIs. For more info, please
	 *   see @link http://processors.wiki.ti.com/index.php/SystemAnalyzerTutorial1F
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAProfile.h>
     *  #include <ti/sysbios/knl/Task.h>
     *  ...
     * void functionEntryHook( void (*adrs)() ){
     *    Log_write2(UIAProfile_enterFunctionAdrs, (IArg)Task_selfMacro(),(IArg)addr);
     *   ...
     * void functionExitHook( void (*adrs)() ){
     *    Log_write2(UIAProfile_exitFunctionAdrs, (IArg)Task_selfMacro(),(IArg)addr);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  enterFunctionAdrs: taskHandle=0x0, adrs=0x820060
     *  exitFunctionAdrs: taskHandle=0x0, adrs=0x820060
     *  @p
     *  @param(taskHandle)   task handle that identifies the currently active task (use 0 if not required)
     *  @param(functionAdrs) the address of a function that can differentiate this pair of start and stop events from others
     */
    config xdc.runtime.Log.Event exitFunctionAdrs = {
        mask: Diags.EXIT,
        msg: "exitFunctionAdrs: taskHandle=0x%x, adrs=0x%x"
    };

    /*!
     *  ======== metaEventExitFunction ========
     *  Metadata description of the exitFunctionAdrs event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventExitFunctionAdrs = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "exitFunctionAdrs",
        tooltipText: "Marks the end of analysis for a module instance",
        numParameters: 2,
        paramInfo: [
        {   name: 'Qualifier',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_FUNCTIONADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };

    /*!
     *  ======== enterFunctionName ========
     *  Profiling event used to log the entry point of a function
     *
     * @a(Example)
     * To add entry and exit hook functions to every function
     * 1. Use the following compiler options when compiling the source
     *  @p(code)
     *  --entry_hook=functionEntryHook
     *  --entry_param=name
     *  --exit_hook=functionExitHook
     *  --exit_param=name
     *  @p
     * 2. Add the following c code to implement the hook functions:
     *   The first parameter (the taskHandle) is set to  0 in this example.
     *   @see exitFunction for an example of how to log the current task ID
     *   for task-aware function profiling.
     *   In order to further reduce the CPU overhead of logging the
     *   UIAProfile events, you can use the LogUC.h APIs. For more info, please
     *   see @link http://processors.wiki.ti.com/index.php/SystemAnalyzerTutorial1F
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAProfile.h>
     *  ...
     * void functionEntryHook(const char* name ){
     *    Log_write2(UIAProfile_enterFunctionName, 0,(IArg)name);
     *   ...
     * void functionExitHook(const char* name){
     *    Log_write2(UIAProfile_exitFunctionName, 0,(IArg)name);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  enterFunctionName: taskHandle=0x0, name=myFunctionName
     *  exitFunctionName: taskHandle0x0, name=myFunctionName
     *  @p
     *  @param(taskHandle)   task handle that identifies the currently active task (use 0 if not required)
     *  @param(functionName) the (const char*) name of the function that is passed to the hook fn by the compiler
     */
    config xdc.runtime.Log.Event enterFunctionName = {
        mask: Diags.ENTRY,
        msg: "enterFunctionName: taskHandle=0x%x, name=%s"
    };

    /*!
     *  ======== metaEventEnterFunctionName ========
     *  Metadata description of the enterFunctionName event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventEnterFunctionName = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_START,
        displayText: "enterFunctionName",
        tooltipText: "function entry",
        numParameters: 2,
        paramInfo: [
        {   name: 'Qualifier',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_STRINGADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };


    /*!
     *  ======== exitFunctionName ========
     *  Profiling event used to log the exit point of a function
     *
     * @a(Example)
     * To add entry and exit hook functions to every function
     * 1. Use the following compiler options when compiling the source
     *  @p(code)
     *  --entry_hook=functionEntryHook
     *  --entry_param=name
     *  --exit_hook=functionExitHook
     *  --exit_param=name
     *  @p
     * 2. Add the following c code to implement the hook functions:
     *   Task_selfMacro() is used to get the current task handle in this example.
     *   @see enterFunction for an example of how to save CPU by logging 0
     *   instead of the task handle if task-aware profiling is not required.
     *   In order to further reduce the CPU overhead of logging the
     *   UIAProfile events, you can use the LogUC.h APIs. For more info, please
     *   see @link http://processors.wiki.ti.com/index.php/SystemAnalyzerTutorial1F
     *  @p(code)
     *  #include <xdc/runtime/Log.h>
     *  #include <ti/uia/events/UIAProfile.h>
     *  #include <ti/sysbios/knl/Task.h>
     *  ...
     * void functionEntryHook(const char* name){
     *    Log_write2(UIAProfile_enterFunctionName, (IArg)Task_selfMacro(),(IArg)name);
     *   ...
     * void functionExitHook(const char* name){
     *    Log_write2(UIAProfile_exitFunctionName, (IArg)Task_selfMacro(),(IArg)name);
     * }
     *  @p
     *  The following text will be displayed for the event:
     *  @p(code)
     *  enterFunctionName: taskHandle=0x0, adrs=myFunctionName
     *  exitFunctionName: taskHandle=0x0, name=myFunctionName
     *  @p
     *  @param(taskHandle)   task handle that identifies the currently active task (use 0 if not required)
     *  @param(functionName) the (const char*) name of the function that is passed to the hook fn by the compiler
     */
    config xdc.runtime.Log.Event exitFunctionName = {
        mask: Diags.EXIT,
        msg: "exitFunctionName: taskHandle=0x%x, name=%s"
    };

    /*!
     *  ======== metaEventExitFunctionName ========
     *  Metadata description of the exitFunctionName event
     *
     *  @_nodoc
     */
    metaonly config DvtTypes.MetaEventDescriptor metaEventExitFunctionName = {
        versionId: "2.0",
        analysisType: DvtTypes.DvtAnalysisType_STOP,
        displayText: "exitFunctionName",
        tooltipText: "Marks the end of analysis for a module instance",
        numParameters: 2,
        paramInfo: [
        {   name: 'Qualifier',
            dataDesc: DvtTypes.DvtDataDesc_INSTANCE,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        },
        {   name: 'FunctionAdrs',
            dataDesc: DvtTypes.DvtDataDesc_STRINGADRS,
            dataTypeName: 'Int',
            units: 'none',
            isHidden: false
        }
        ]
    };

    /*!
     *  ======== runtimeControl ========
     *  Specify whether profile events can be enabled / disabled at runtime.
     *  (set to false by default)
     *
     *  This determines what diags settings are applied to the module's diags
     *  mask.
     *  if the UIAProfile enable config property is true (default):
     *    If runtimeControl = 'false' the diags bits will be configured as
     *    ALWAYS_ON, meaning they can't be changed at runtime.
     *    If runtimeControl = 'true', the bits will be configured as 'RUNTIME_ON'.
     *
     * if the UIAProfile enable config property is false:
     *    If runtimeControl = 'false' the diags bits will be configured as
     *    ALWAYS_OFF, meaning they can't be changed at runtime.
     *    If runtimeControl = 'true', the bits will be configured as 'RUNTIME_OFF'.
     */
    metaonly config Bool runtimeControl = false;

    /*!
     *  ======== isContextAwareProfilingEnabled ========
     *  Specify whether the task context that the function is executing within is logged or not
     *  Set to false if not using Sys/BIOS or to reduce CPU overhead.
     */
	metaonly config Bool isContextAwareProfilingEnabled = true;

    /*!
     *  ======== enable ========
     *  Specify whether profile events are enabled or disabled
     *  (set to true by default)
     *
     *  if the UIAProfile enable config property is true (default):
     *    If runtimeControl = 'false' the diags bits will be configured as
     *    ALWAYS_ON, meaning they can't be changed at runtime.
     *    If runtimeControl = 'true', the bits will be configured as 'RUNTIME_ON'.
     *
     * if the UIAProfile enable config property is false:
     *    If runtimeControl = 'false' the diags bits will be configured as
     *    ALWAYS_OFF, meaning they can't be changed at runtime.
     *    If runtimeControl = 'true', the bits will be configured as 'RUNTIME_OFF'.
     */
    metaonly config Bool enable = true;
}
