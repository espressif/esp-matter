/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include <stdint.h>
#include <string.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/setup.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)

#include <FreeRTOSConfig.h>

/* IAR includes */
#include <intrinsics.h>

// Forward declaration of the default fault and interrupt handlers.
static void nmiISR(void);
static void faultISR(void);
static void busFaultHandler(void);
static void intDefaultHandler(void);

// FreeRTOS handlers
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void __iar_program_start(void);
int localProgramStart(void);

//*****************************************************************************
//
// Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern const void* STACK_TOP;

// Place something in the CSTACK segment to get the stack check feature
// to work as expected
__root static void* dummy_stack @ ".stack";

//*****************************************************************************
//
// The vector table in Flash. Note that the proper constructs must be placed
// on this to ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__root void (* const __vector_table[NUM_INTERRUPTS])(void) @ ".intvec" =
{
    (void (*)(void))&STACK_TOP,      // The initial stack pointer
    __iar_program_start,             //  1 The reset handler
    nmiISR,                          //  2 The NMI handler
    faultISR,                        //  3 The hard fault handler
    intDefaultHandler,               //  4 The MPU fault handler
    busFaultHandler,                 //  5 The bus fault handler
    intDefaultHandler,               //  6 The usage fault handler
    intDefaultHandler,               //  7 Reserved
    intDefaultHandler,               //  8 Reserved
    intDefaultHandler,               //  9 Reserved
    intDefaultHandler,               // 10 Reserved
    vPortSVCHandler,                 // 11 SVCall handler
    intDefaultHandler,               // 12 Debug monitor handler
    intDefaultHandler,               // 13 Reserved
    xPortPendSVHandler,              // 14 The PendSV handler
    xPortSysTickHandler,             // 15 The SysTick handler
    intDefaultHandler,               // 16 CPUIRQ0
    intDefaultHandler,               // 17 CPUIRQ1
    intDefaultHandler,               // 18 CPUIRQ2
    intDefaultHandler,               // 19 CPUIRQ3
    intDefaultHandler,               // 20 CPUIRQ4
    intDefaultHandler,               // 21 GPIO event interrupt
    intDefaultHandler,               // 22 LRF interrupt 0
    intDefaultHandler,               // 23 LRF interrupt 1
    intDefaultHandler,               // 24 uDMA done events
    intDefaultHandler,               // 25 AES interrupt
    intDefaultHandler,               // 26 SPI0 combined interrupt
    intDefaultHandler,               // 27 UART0 combined interrupt
    intDefaultHandler,               // 28 I2C0 combined interrupt
    intDefaultHandler,               // 29 LGPT0 interrupt
    intDefaultHandler,               // 30 LGPT1 interrupt
    intDefaultHandler,               // 31 ADC interrupt
    intDefaultHandler,               // 32 CPUIRQ16
    intDefaultHandler,               // 33 LGPT2 interrupt
    intDefaultHandler,               // 34 LGPT3 interrupt
};

//*****************************************************************************
//
// Perform early-boot device initialisation.
//
//*****************************************************************************
int localProgramStart(void)
{
    unsigned long *vtor = (unsigned long *)0xE000ED08;

    /* do final trim of device */
    SetupTrimDevice();

    /* disable interrupts */
    __set_BASEPRI( configMAX_SYSCALL_INTERRUPT_PRIORITY );
    __DSB();
    __ISB();

    /*
     * set vector table base to point to above vectors in Flash; during
     * driverlib interrupt initialization this table will be copied to RAM
     */
    *vtor = (unsigned long)&__vector_table[0];

    return 1;
}


//*****************************************************************************
//
// This function is called by __iar_program_start() early in the boot sequence.
//
//*****************************************************************************
int __low_level_init(void)
{
    /*
     *  Initialize the stack pointer and branch to localProgramStart().
     *  Some debuggers do not load the stack pointer from the reset vector.
     *  This code ensures that the stack pointer is initialized.
     *  The first entry of the vector table is the address of the stack.
     */
    __asm (
        " ldr r0, =__vector_table\n"
        " ldr r0, [r0]\n"
        " mov sp, r0\n"
        " bl localProgramStart");

    // This code is unreachable but the compiler expects a return statement
    return 1;
}


//*****************************************************************************
//
// This is the code that gets called when the processor receives an NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void nmiISR(void)
{
    /* Enter an infinite loop. */
    while (1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void faultISR(void)
{
    /* Enter an infinite loop. */
    while (1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a bus fault.
// This simply enters an infinite loop, preserving the system state for
// examination by a debugger.
//
//*****************************************************************************
static void busFaultHandler(void)
{
    /* Enter an infinite loop. */
    while (1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void intDefaultHandler(void)
{
    /* Enter an infinite loop. */
    while (1)
    {
    }
}
