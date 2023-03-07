/*
 *-------------------------------------------------------------------------------
 * \file cy_syslib_iar.c
 * \version 2.20
 *
 *  \brief Assembly routines for IAR.
 *
 *-------------------------------------------------------------------------------
 * Copyright 2016-2020 Cypress Semiconductor Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Derived from ../armclang/cy_syslib_mdk.s
 */

#include "system_psoc6.h"

/*
 *-------------------------------------------------------------------------------
 * Function Name: Cy_SysLib_DelayCycles
 *-------------------------------------------------------------------------------
 *
 * Summary:
 *  Delays for the specified number of cycles.
 *
 * Parameters:
 *  uint32_t cycles: The number of cycles to delay.
 *
 *-------------------------------------------------------------------------------
 */

void Cy_SysLib_DelayCycles(uint32_t cycles)
{
asm("ADDS %0, %0, #2         ;    1    2    Round to the nearest multiple of 4. \n"
    "LSRS %0, %0, #2         ;    1    2    Divide by 4 and set flags.          \n"
    "BEQ Cy_DelayCycles_done ;    2    2    Skip if 0.                          \n"
"Cy_DelayCycles_loop:                                                           \n"
    "ADDS %0, %0, #1         ;    1    2    Increment the counter.              \n"
    "SUBS %0, %0, #2         ;    1    2    Decrement the counter by 2.         \n"
    "BNE Cy_DelayCycles_loop ;   (1)2  2    2 CPU cycles (if branch is taken).  \n"
    "NOP                     ;    1    2    Loop alignment padding.             \n"
"Cy_DelayCycles_done:                                                           \n"
    ";BX lr                   ;    3    2                                        \n"
    :
    : "r" (cycles));
}


/*
 *-------------------------------------------------------------------------------
 * Function Name: Cy_SysLib_EnterCriticalSection
 *-------------------------------------------------------------------------------
 *
 * Summary:
 *  Cy_SysLib_EnterCriticalSection disables interrupts and returns a value
 *  indicating whether interrupts were previously enabled.
 *
 *  Note Implementation of Cy_SysLib_EnterCriticalSection manipulates the IRQ
 *  enable bit with interrupts still enabled. The test and set of the interrupt
 *  bits are not atomic. Therefore, to avoid a corrupting processor state, it must
 *  be the policy that all interrupt routines restore the interrupt enable bits as
 *  they were found on entry.
 *
 * Return:
 *  uint8_t
 *   Returns 0 if interrupts were previously enabled or 1 if interrupts
 *   were previously disabled.
 *
 *-------------------------------------------------------------------------------
 */

uint8_t Cy_SysLib_EnterCriticalSection(void)
{
    uint8_t mask;
asm("MRS %0, PRIMASK         ; Save and return an interrupt state. \n"
    "CPSID I                 ; Disable the interrupts.             \n"
    : "=r" (mask)
    :);
    return mask;
}

/*
 *-------------------------------------------------------------------------------
 * Function Name: Cy_SysLib_ExitCriticalSection
 *-------------------------------------------------------------------------------
 *
 * Summary:
 *  Cy_SysLib_ExitCriticalSection re-enables interrupts if they were enabled
 *  before Cy_SysLib_EnterCriticalSection was called. The argument should be the
 *  value returned from Cy_SysLib_EnterCriticalSection.
 *
 * Parameters:
 *  uint8_t savedIntrStatus:
 *   The saved interrupt status returned by the Cy_SysLib_EnterCriticalSection
 *   function.
 *
 *-------------------------------------------------------------------------------
 */

void Cy_SysLib_ExitCriticalSection(uint8_t savedIntrStatus)
{
asm("MSR PRIMASK, %0         ; Restore the interrupt state. \n"
    :
    : "r" (savedIntrStatus)
    );
}
