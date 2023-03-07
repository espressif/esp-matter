/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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


//*****************************************************************************
//
// Check if compiler is CCS
//
//*****************************************************************************
#if !(defined(__TI_COMPILER_VERSION__))
#error "startup_cc13x1_cc26x1_ccs.c: Unsupported compiler!"
#endif

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/setup.h)


//*****************************************************************************
//
//! Forward declaration of the reset ISR and the default fault handlers.
//
//*****************************************************************************
void        resetISR( void );
static void nmiISR( void );
static void faultISR( void );
static void intDefaultHandler( void );


//*****************************************************************************
//
//! The entry point for the application startup code.
//
//*****************************************************************************
extern void _c_int00(void);


//*****************************************************************************
//
// CCS: Linker variable that marks the top of the stack.
//
//*****************************************************************************
extern unsigned long __STACK_END;


//! The vector table. Note that the proper constructs must be placed on this to
//! ensure that it ends up at physical address 0x0000.0000 or at the start of
//! the program if located at a start address other than 0.
//
//*****************************************************************************
#pragma DATA_SECTION(g_pfnVectors, ".intvecs")
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((unsigned long)&__STACK_END),
                                            //  0 The initial stack pointer
    resetISR,                               //  1 The reset handler
    nmiISR,                                  //  2 The NMI handler
    faultISR,                               //  3 The hard fault handler
    intDefaultHandler,                      //  4 The MPU fault handler
    intDefaultHandler,                      //  5 The bus fault handler
    intDefaultHandler,                      //  6 The usage fault handler
    0,                                      //  7 Reserved
    0,                                      //  8 Reserved
    0,                                      //  9 Reserved
    0,                                      // 10 Reserved
    intDefaultHandler,                      // 11 SVCall handler
    intDefaultHandler,                      // 12 Debug monitor handler
    0,                                      // 13 Reserved
    intDefaultHandler,                      // 14 The PendSV handler
    intDefaultHandler,                      // 15 The SysTick handler
    //--- External interrupts ---
    intDefaultHandler,                      // 16 AON edge detect
    intDefaultHandler,                      // 17 I2C
    intDefaultHandler,                      // 18 RF Core Command & Packet Engine 1
    0,                                      // 19 Reserved
    intDefaultHandler,                      // 20 AON RTC
    intDefaultHandler,                      // 21 UART0 Rx and Tx
    intDefaultHandler,                      // 22 AUX software event 0
    intDefaultHandler,                      // 23 SSI0 Rx and Tx
    0,                                      // 24 Reserved
    intDefaultHandler,                      // 25 RF Core Command & Packet Engine 0
    intDefaultHandler,                      // 26 RF Core Hardware
    intDefaultHandler,                      // 27 RF Core Command Acknowledge
    intDefaultHandler,                      // 28 I2S
    intDefaultHandler,                      // 29 AUX software event 1
    intDefaultHandler,                      // 30 Watchdog timer
    intDefaultHandler,                      // 31 Timer 0 subtimer A
    intDefaultHandler,                      // 32 Timer 0 subtimer B
    intDefaultHandler,                      // 33 Timer 1 subtimer A
    intDefaultHandler,                      // 34 Timer 1 subtimer B
    intDefaultHandler,                      // 35 Timer 2 subtimer A
    intDefaultHandler,                      // 36 Timer 2 subtimer B
    intDefaultHandler,                      // 37 Timer 3 subtimer A
    intDefaultHandler,                      // 38 Timer 3 subtimer B
    intDefaultHandler,                      // 39 Crypto Core Result available
    intDefaultHandler,                      // 40 uDMA Software
    intDefaultHandler,                      // 41 uDMA Error
    intDefaultHandler,                      // 42 Flash controller
    intDefaultHandler,                      // 43 Software Event 0
    intDefaultHandler,                      // 44 AUX combined event
    intDefaultHandler,                      // 45 AON programmable 0
    intDefaultHandler,                      // 46 Dynamic Programmable interrupt
                                            //    source (Default: PRCM)
    intDefaultHandler,                      // 47 AUX Comparator A
    intDefaultHandler,                      // 48 AUX ADC new sample or ADC DMA
                                            //    done, ADC underflow, ADC overflow
    intDefaultHandler,                      // 49 TRNG event
    intDefaultHandler,                      // 50 Combined event from Oscillator control
    0,                                      // 51 Reserved
    0,                                      // 52 Reserved
    intDefaultHandler                       // 53 Combined event from battery monitor
};


//*****************************************************************************
//
//! This is the code that gets called when the processor first starts execution
//! following a reset event. Only the absolutely necessary set is performed,
//! after which the application supplied entry() routine is called. Any fancy
//! actions (such as making decisions based on the reset cause register, and
//! resetting the bits in that register) are left solely in the hands of the
//! application.
//
//*****************************************************************************
void
resetISR(void)
{
    IntMasterDisable();

    //
    // Final trim of device
    //
    SetupTrimDevice();

    //
    // Jump to the CCS C Initialization Routine.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
    //
    // If we ever return signal Error
    //
    faultISR();
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives a NMI. This
//! simply enters an infinite loop, preserving the system state for examination
//! by a debugger.
//
//*****************************************************************************
static void
nmiISR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives a fault
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
static void
faultISR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}


//*****************************************************************************
//
//! This is the code that gets called when the processor receives an unexpected
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
static void
intDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
