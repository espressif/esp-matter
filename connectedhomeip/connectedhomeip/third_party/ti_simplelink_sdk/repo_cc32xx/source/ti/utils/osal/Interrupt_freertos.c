/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Interrupt_freertos.c ========
 */

#include <stdint.h>

#include <ti/utils/osal/Interrupt.h>

#include <FreeRTOS.h>
#include <portmacro.h>

/* Masks off all bits but the VECTACTIVE bits in the ICSR register. */
#define portVECTACTIVE_MASK  (0xFFUL)

/*
 *  ======== Interrupt_disable ========
 */
uintptr_t Interrupt_disable(void)
{
    uintptr_t key;

    /*  If we're not in ISR context, use the FreeRTOS macro, since
     *  it handles nesting.
     */
    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        portENTER_CRITICAL();
        key = 0;
    }
    else {
#ifdef __TI_COMPILER_VERSION__
        key = _set_interrupt_priority(configMAX_SYSCALL_INTERRUPT_PRIORITY);
#else
#ifdef __IAR_SYSTEMS_ICC__
        asm volatile (
#else
        __asm__ __volatile__ (
#endif
            "mrs %0, basepri\n\t"
            "msr basepri, %1"
            : "=&r" (key)
            : "r" (configMAX_SYSCALL_INTERRUPT_PRIORITY)
        );
#endif
    }

    return (key);
}

/*
 *  ======== Interrupt_restore ========
 */
void Interrupt_restore(uintptr_t key)
{
    if ((portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0) {
        portEXIT_CRITICAL();
    }
    else {
#ifdef __TI_COMPILER_VERSION__
        _set_interrupt_priority(key);
#else
#ifdef __IAR_SYSTEMS_ICC__
        asm volatile (
#else
        __asm__ __volatile__ (
#endif
            "msr basepri, %0"
            :: "r" (key)
            );
#endif
    }
}
