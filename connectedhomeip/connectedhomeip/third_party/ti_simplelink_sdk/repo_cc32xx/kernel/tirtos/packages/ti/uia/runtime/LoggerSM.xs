/*
 * Copyright (c) 2012-2015, Texas Instruments Incorporated
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
 *  ======== LoggerSM.xs ========
 */

var LoggerSM  = null;
var Defaults  = null;
var Types     = null;
var Memory    = null;
var Timestamp = null;
var UIAMetaData = null;

/*
 *  ======== module$use ========
 */
function module$use() {

    /* Modules used in the .c file */
    xdc.module("xdc.runtime.Diags");

    /* Modules used in the .xs file (and maybe in the .c file) */
    Defaults = xdc.useModule('xdc.runtime.Defaults');
    Types    = xdc.useModule('xdc.runtime.Types');
    Memory   = xdc.useModule('xdc.runtime.Memory');
    Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
    LoggerSM = this;

    var Program = xdc.module('xdc.cfg.Program');
    var bitsPerChar = Program.build.target.bitsPerChar;
    var bytesPerInt = Program.build.target.stdTypes.t_Int.size;
    var bytesPerLong = Program.build.target.stdTypes.t_Long.size;

    if ((bitsPerChar * bytesPerInt != 32) &&
            (bitsPerChar * bytesPerLong != 32)) {
        LoggerSM.$logError("LoggerSM does not support this target", LoggerSM);
    }

    /* Memory module is needed only if memoryPolicy is not STATIC_POLICY. */
    if (Defaults.getCommon(LoggerSM, "memoryPolicy") !=
        Types.STATIC_POLICY) {
        xdc.useModule('xdc.runtime.Memory');
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    /* Assign the default filtering levels based on the configuration. */
    mod.level1 = params.level1Mask;
    mod.level2 = params.level2Mask;
    mod.level3 = params.level3Mask;
    mod.sharedObj = null;
    mod.sharedBuffer.length = params.sharedMemorySize;
    mod.serial = 0;
    mod.partitionId = params.partitionId;
    mod.enabled = true;
    /* Need 4KB alignment */
    Memory.staticPlace(mod.sharedBuffer, 4096, params.bufSection);
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{

}

/*
 *  ======== getLoggerInstanceId ========
 *  Parses the string returned from the instance (e.g. Mod.Instance#1)
 *  Returns the trailing number +1 as the instance ID.  This can then
 *  be used to look up the instance using mod.$instances[instanceId].
 *  It is also used as a unique identifier in the Queue Descriptor header
 *  and UIA Metadata to identify which instance of a logger is being used
 *  for a specific purpose within UIA. Note that a LoggerInstanceId of 0
 *  is reserved for dynamically created instances of the logger - the
 *  target code can test to see if the logger's queue descriptor header
 *  has an ID field of 0, and scan through the list of queue descriptors
 *  to determine what the maximum logger ID that is in use is in order
 *  to initialize the instance ID to a unique value for that logger module.
 */
function getLoggerInstanceId(inst){
   var id = 1;
   return (id);
}

/*
 *  ======== getLoggerPriority ========
 *  returns the priority of this logger instance.
 */
function getLoggerPriority(inst){
   var mod = xdc.module('ti.uia.runtime.LoggerSM');
   var priority = mod.hdr.priority;
   return (priority);
}

/*
 *  ======== setLoggerPriority ========
 *  sets the priority of this logger instance.
 */
function setLoggerPriority(inst, priority){
   var mod = xdc.module('ti.uia.runtime.LoggerSM');
   mod.hdr.priority = Number(priority);
}

/*
 *  ======== getMetaArgs ========
 *  Returns meta data to support stop-mode RTA.
 */
function getMetaArgs(inst, instNum)
{
    var LoggerSM = xdc.module('ti.uia.runtime.LoggerSM');

    /* Create a new StopModeData structure to populate. */
    var data = new LoggerSM.MetaData();
    data.instanceId = 1;
    data.priority = 1;
    return (data);
}

/*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    var LoggerSM  = xdc.useModule("ti.uia.runtime.LoggerSM");

    var LoggerSMModConfig = Program.getModuleConfig(LoggerSM.$name);

    view.isTimestampEnabled = LoggerSMModConfig.isTimestampEnabled;
    view.decode = LoggerSMModConfig.decode;
    view.overwrite = LoggerSMModConfig.overwrite;
}

/*
 *  ======== viewInitProxy ========
 *  Initializes the Proxy view in ROV.
 */
function viewInitInstances(view, obj)
{
    /* Retrieve the LoggerSM instance's name */
    view.label = Program.getShortName(obj.$label);
}
