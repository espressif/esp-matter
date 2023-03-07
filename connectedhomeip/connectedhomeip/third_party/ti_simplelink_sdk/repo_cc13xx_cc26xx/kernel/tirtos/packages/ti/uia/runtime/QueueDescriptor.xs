/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 *  ======== QueueDescriptor.xs ========
 */

var QueueDescriptor = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    var QueueDescriptor = xdc.useModule('ti.uia.runtime.QueueDescriptor');
}


/*
 *  ======== generateInstanceId ========
 *  Returns a unique logger instance ID for use by a logger that
 *  implements the IUIATransfer interface.
 *
 *  Note that all Logger Instance Ids must be non-zero.
 *  LoggerInstanceIds with b15=1 are reserved for dynamically created
 *  instances of the logger.
 */
function generateInstanceId()
{
    this.maxId++;  // ensure non-zero
    var id = this.maxId;
    return (id);
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    if (xdc.om.$name != "cfg") {
        return;
    }

    /*
     * Initialize module-scope variables.
     */
    mod.mPtrToFirstDescriptor = null;
    mod.mUpdateCount = 0;
    mod.is5555ifInitialized = 0x5555;
}

 /*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    var QueueDescriptor  = xdc.useModule("ti.uia.runtime.QueueDescriptor");

    var qdModConfig = Program.getModuleConfig(QueueDescriptor.$name);

    /* Display the module's ptrToFirstDescriptor */
    view.mPtrToFirstDescriptor = mod.mPtrToFirstDescriptor;

    /* Display the number of times the linked list has been updated */
    view.mUpdateCount = mod.mUpdateCount;

    /* Display the init flag value */
    view.is5555ifInitialized = mod.is5555ifInitialized;
}
