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
 *  ======== GateHwi.c ========
 */

#include <xdc/std.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/hal/Hwi.h>

#include "package/internal/GateHwi.xdc.h"

/* No startup needed because it uses only unconstrained APIs */

/*
 *  ======== GateHwi_Instance_init ========
 */
Void GateHwi_Instance_init(GateHwi_Object *gate,
                           const GateHwi_Params *params)
{
}

/*
 *  ======== GateHwi_enter ========
 *  Return the key for Task_disable.
 */
IArg GateHwi_enter(GateHwi_Object *obj)
{
    return(Hwi_disable());
}

/*
 *  ======== GateHwi_leave ========
 *  Use the key from the Task_disable returned from the enter function.
 */
Void GateHwi_leave(GateHwi_Object *obj, IArg key)
{
    Hwi_restore((UInt)key);
}

/*
 *  ======== query ========
 *  
 */
Bool GateHwi_query(Int qual)
{
    return (FALSE);
}


