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
 *  ======== GateTest.c ========
 *  Implementation of functions specified in GateTest.xdc.
 */

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "package/internal/GateTest.xdc.h"

/*
 *  ======== GateTest_Instance_init ========
 */
Void  GateTest_Instance_init(GateTest_Object *obj,
                               const GateTest_Params *params)
{
    obj->createCount++;    
}

/*
 *  ======== GateTest_enter ========
 *  Returns FIRST_ENTER when it gets the gate, returns NESTED_ENTER
 *  on nested calls.
 */
IArg GateTest_enter(GateTest_Object *obj)
{
    obj->enterCount++;    
    return (1);
}

/*
 *  ======== GateTest_leave ========
 *  Only releases the gate if key == FIRST_ENTER.
 */
Void GateTest_leave(GateTest_Object *obj, IArg key)
{
    obj->leaveCount++;    
}

/*
 *  ======== query ========
 *  
 */
Bool GateTest_query(Int qual)
{
    return (TRUE);
}

/*
 *  ======== GateTest_getEnterCount ========
 *  
 */
Int GateTest_getEnterCount(GateTest_Object *obj)
{
    return (obj->enterCount);
}

/*
 *  ======== GateTest_getLeaveCount ========
 *  
 */
Int GateTest_getLeaveCount(GateTest_Object *obj)
{
    return (obj->leaveCount);
}

/*
 *  ======== GateTest_getCreateCount ========
 *  
 */
Int GateTest_getCreateCount(GateTest_Object *obj)
{
    return (obj->createCount);
}


