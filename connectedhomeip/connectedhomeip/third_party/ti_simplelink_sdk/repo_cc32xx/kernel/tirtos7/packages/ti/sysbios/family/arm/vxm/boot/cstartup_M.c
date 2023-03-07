/**************************************************
 *
 * This file contains an interrupt vector for Cortex-M written in C.
 * The actual interrupt functions must be provided by the application developer.
 *
 * Copyright 2007 IAR Systems. All rights reserved.
 *
 * $Revision: 47408 $
 * 2014-Oct-08 ashish  Rename __iar_program_start to __iar_program_startC
 * 2013-May-23 vikram  Removed vector table, added vfp init
 * 2013-Apr-04 vikram  Added xdc startup call hooks
 *
 **************************************************/

/* XDC startup functions */
extern void iar_startup_reset( void );

void __cmain( void );
__weak void __iar_init_core( void );

void __iar_program_startC( void )
{
    /* XDC startup reset hook */
    iar_startup_reset();

    __iar_init_core();

#if defined (__ARMVFP__)
/*------------------------------------------------------
 * SETUP FULL ACCESS TO COPROCESSORS 10 AND 11,
 * REQUIRED FOR FP. COPROCESSOR ACCESS CONTROL REG
 * BITS [23:22] - CP11, [21:20] - CP10
 * SET TO 0b11 TO ENABLE FULL ACCESS
 *------------------------------------------------------
 */
    asm volatile( 
        " MOVW     r1, #0xE000ED88 & 0xFFFF\n"
        " MOVT     r1, #0xE000ED88 >> 16\n"
        " LDR      r0, [ r1 ]\n"
        " MOV      r3, #0xf0\n"
        " ORR      r0,r0,r3, LSL #16\n"
        " STR      r0, [ r1 ]\n"
    );   
#endif

    __cmain();
}
