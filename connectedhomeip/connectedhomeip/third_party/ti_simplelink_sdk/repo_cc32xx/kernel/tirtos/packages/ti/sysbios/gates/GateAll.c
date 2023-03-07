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
 *  ======== GateAll.c ========
 */

#include <xdc/std.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>

#include "package/internal/GateAll.xdc.h"

/* No startup needed because it uses only unconstrained APIs */

/*
 *  ======== GateAll_Instance_init ========
 */
Void GateAll_Instance_init(GateAll_Object *gate,
                               const GateAll_Params *params)
{
    gate->entered = FALSE;
    gate->hwiKey = 0;
    gate->swiKey = 0;
}

/*
 *  ======== GateAll_enter ========
 */
IArg GateAll_enter(GateAll_Object *gate)
{
    if (gate->entered == TRUE) {
        return (1);
    }
    else {
        gate->hwiKey = Hwi_disable();
        gate->swiKey = Swi_disable();
        gate->entered = TRUE;
        return (0);
    }
}

/*
 *  ======== GateAll_leave ========
 */
Void GateAll_leave(GateAll_Object *gate, IArg key)
{
    if (key == 0) {
        gate->entered = FALSE;
        Swi_restore(gate->swiKey);
        Hwi_restore(gate->hwiKey);
    }
}

/*
 *  ======== GateAll_query ========
 */
Bool GateAll_query(Int qual)
{
    return (FALSE);
}



