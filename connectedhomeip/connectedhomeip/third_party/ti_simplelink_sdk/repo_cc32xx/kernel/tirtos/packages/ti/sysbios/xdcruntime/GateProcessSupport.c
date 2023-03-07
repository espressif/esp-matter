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
 *  ======== GateProcessSupport.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/gates/GateMutexPri.h>

#include "package/internal/GateProcessSupport.xdc.h"

/*
 *  ======== GateProcessSupport_Instance_init ========
 */
Void GateProcessSupport_Instance_init(GateProcessSupport_Handle gate,
    const GateProcessSupport_Params* params)
{
    GateMutexPri_Handle bios6gate;
 
    bios6gate = GateProcessSupport_Instance_State_gate(gate);

    GateMutexPri_construct(GateMutexPri_struct(bios6gate), NULL);
}

/*
 *  ======== GateProcessSupport_Instance_finalize ========
 */
Void GateProcessSupport_Instance_finalize(GateProcessSupport_Handle gate)
{
    GateMutexPri_Handle bios6gate;
 
    bios6gate = GateProcessSupport_Instance_State_gate(gate);
    GateMutexPri_destruct(GateMutexPri_struct(bios6gate));
}

/*
 *  ======== GateProcessSupport_enter ========
 */
IArg GateProcessSupport_enter(GateProcessSupport_Handle gate)
{
    GateMutexPri_Handle bios6gate;
 
    bios6gate = GateProcessSupport_Instance_State_gate(gate);

    return (GateMutexPri_enter(bios6gate));
}

/*
 *  ======== GateProcessSupport_leave ========
 */
Void GateProcessSupport_leave(GateProcessSupport_Handle gate, IArg key)
{
    GateMutexPri_Handle bios6gate;
 
    bios6gate = GateProcessSupport_Instance_State_gate(gate);
    GateMutexPri_leave(bios6gate, key);
}

/*
 *  ======== GateProcessSupport_getReferenceCount ========
 */
Int GateProcessSupport_getReferenceCount(GateProcessSupport_Handle gate, 
    Error_Block* eb)
{
    return (0);
}

/*
 *  ======== GateProcessSupport_query ========
 */
Bool GateProcessSupport_query(Int qual)
{
    return (GateMutexPri_query(qual));
}
