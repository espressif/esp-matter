/***************************************************************************//**
 * @file
 * @brief Faults
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "asm.h"

        __IMPORT__ halCrashInfo
        __IMPORT__ halInternalCrashHandler
        __IMPORT__ halInternalSysReset

// define the values the link register can have on entering an exception
__EQU__(EXC_RETURN_HANDLER_MSP, 0xFFFFFFF1)
__EQU__(EXC_RETURN_THREAD_MSP,  0xFFFFFFF9)
__EQU__(EXC_RETURN_THREAD_PSP,  0xFFFFFFFD)

// define stack bytes needed by halCrashHandler()
__EQU__(CRASH_STACK_SIZE, 64)

//------------------------------------------------------------------------------
//
// Various types of crashes generate NMIs, hard, bus, memory, usage and debug 
// monitor faults that vector to the routines defined here. 
//
// Although all of these could vector directly to the common fault handler,
// each is given its own entry point to allow setting a breakpoint for a 
// particular type of crash.
//
//------------------------------------------------------------------------------

        __CODE__
        __THUMB__
        __EXPORT__ NMI_Handler
NMI_Handler:
        b.w fault

        __CODE__
        __THUMB__
        __EXPORT__ HardFault_Handler
HardFault_Handler:
        b.w fault

        __CODE__
        __THUMB__
        __EXPORT__ BusFault_Handler
BusFault_Handler:
        b.w fault

// MemManage_Handler is now consumed by the Simple MPU Driver in emdrv, which
// will call mpu_fault_handler
        __CODE__
        __THUMB__
        __EXPORT__ mpu_fault_handler
mpu_fault_handler:
        b.w fault

        __CODE__
        __THUMB__
        __EXPORT__ UsageFault_Handler
UsageFault_Handler:
        b.w fault

#if (__CORTEX_M >= 23U)
        __CODE__
        __THUMB__
        __EXPORT__ SecureFault_Handler
SecureFault_Handler:
        b.w fault
#endif

        __CODE__
        __THUMB__
        __EXPORT__ DebugMon_Handler
DebugMon_Handler:
        b.w fault

        __CODE__
        __THUMB__
        __EXPORT__ uninitializedIsr
uninitializedIsr:
        b.w fault

//------------------------------------------------------------------------------
// Common fault handler prolog processing
//
// Determines which of the three possible EXC_RETURN values is in lr,
// then uses lr to save processor registers r0-r12 to the crash info area
//
// restores lr's value, and then saves lr, msp and psp to the crash info area
//
// checks the stack pointer to see if it is valid and won't overwrite the crash
// info area - if needed, resets the stack pointer to the top of the stack
//
// calls halInternalCrashHandler() to continue saving crash data. This 
// C function can finish crash processing without risking further faults.
// It returns the fault reason, and this is passed to halInternalSysReset() 
// that forces a processor reset.
//------------------------------------------------------------------------------

        __CODE__
        __THUMB__
        __EXPORT__ fault
fault:
        cpsid   i               // in case the fault priority is low
        tst     lr, #8          // test EXC_RETURN for handler vs thread mode
        beq.n   fault_handler_msp
        tst     lr, #4          // thread mode: test for msp versus psp
        beq.n   fault_thread_msp

fault_thread_psp:
        ldr     lr, =(halCrashInfo)
        stmia.w lr!, {r0-r12}
        ldr     r0, =EXC_RETURN_THREAD_PSP
        b.n     fault_continue

fault_handler_msp:
        ldr     lr, =(halCrashInfo)
        stmia.w lr!, {r0-r12}
        ldr     r0, =EXC_RETURN_HANDLER_MSP
        b.n     fault_continue

fault_thread_msp:
        ldr     lr, =(halCrashInfo)
        stmia.w lr!, {r0-r12}
        ldr     r0, =EXC_RETURN_THREAD_MSP

fault_continue:
        mov     r1, sp
        mrs     r2, PSP
        stm     lr, {r0-r2}     // save lr, msp and psp
        mov     lr, r0          // restore lr

fault_check_sp:
        // make sure that the current stack pointer is within the minimum region
        // that must be occupied by the stack and that there is some headroom
        // for the crash handler's stack.
        ldr     r0, =__BEGIN_STACK__(CRASH_STACK_SIZE)
        ldr     r4, =__END_STACK__(0)
        cmp     sp, r0
        blo.n   fault_fix_sp
        // compare to make sure SP otherwise looks valid
        cmp     sp, r4
        blo.n   fault_sp_okay
fault_fix_sp:
        msr     msp, r4
fault_sp_okay:
        bl.w    halInternalCrashHandler // saves rest of data, returns reason
        msr     msp, r4                 // set sp to top of stack segment
        bl.w    halInternalSysReset     // call with reset reason in R0     

        __END__
