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
 *  ======== GateSwi.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>

#include <xdc/runtime/IGateProvider.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>

#include "package/internal/GateSwi.xdc.h"

/*
 * Inheritd IGateProvider. Part of runtime. Called in first pass.
 * APIs ready after first pass because Swi_disable() and Swi_restore() 
 * do nothing until Swis are enabled in BIOS_start().
 */

/*
 *  ======== GateSwi_Instance_init ========
 */
Void GateSwi_Instance_init(GateSwi_Object *gate,
                           const GateSwi_Params *params)
{
}
 
/*
 *  ======== GateSwi_enter ========
 *  Return the key for Task_disable.
 */
IArg GateSwi_enter(GateSwi_Object *obj)
{
    /* Ensure that GateSwi is entered within a Task, Swi, or Main */
    Assert_isTrue(((BIOS_getThreadType() == BIOS_ThreadType_Task) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Swi) ||
                   (BIOS_getThreadType() == BIOS_ThreadType_Main)),
                   GateSwi_A_badContext);

    return(Swi_disable());
}

/*
 *  ======== GateSwi_leave ========
 *  Use the key from the Task_disable returned from the enter function.
 */
Void GateSwi_leave(GateSwi_Object *obj, IArg key)
{
    Swi_restore(key);
}

/*
 *  ======== query ========
 *  
 */
Bool GateSwi_query(Int qual)
{
    Bool rc;
    
    switch (qual) {
        case IGateProvider_Q_BLOCKING:
           rc = FALSE; 
           break;
           
        case IGateProvider_Q_PREEMPTING:
           rc = TRUE; 
           break;
           
        default:
           /* TODO assert */
           rc = FALSE;
           break;  
    }
    return (rc);
}

