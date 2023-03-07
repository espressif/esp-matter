@
@/***************************************************************************//**
@ * @file
@ * @brief CPU - ARM Cortex-M Port
@ *******************************************************************************
@ * # License
@ * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
@ *******************************************************************************
@ *
@ * The licensor of this software is Silicon Laboratories Inc.  Your use of this
@ * software is governed by the terms of Silicon Labs Master Software License
@ * Agreement (MSLA) available at
@ * www.silabs.com/about-us/legal/master-software-license-agreement.  This
@ * software is distributed to you in Source Code format and is governed by the
@ * sections of the MSLA applicable to Source Code.
@ *
@ ******************************************************************************/


@*****************************************************************************************************//**
@* @note     (1) This port targets the following:
@*               Core      : ARMv8M Cortex-M
@*               Mode      : Thumb-2 ISA
@*               Toolchain : GNU C Compiler
@*
@* @note     (2) This port supports the ARM Cortex-M33.
@********************************************************************************************************/


@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

        .global  CPU_CntLeadZeros
        .global  CPU_CntTrailZeros
        .global  CPU_RevBits


@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.syntax unified


@********************************************************************************************************
@                                         CPU_CntLeadZeros()
@                                        COUNT LEADING ZEROS
@
@ Description : Counts the number of contiguous, most-significant, leading zero bits before the
@                   first binary one bit in a data value.
@
@ Prototype   : CPU_DATA  CPU_CntLeadZeros(CPU_DATA  val);
@
@ Argument(s) : val         Data value to count leading zero bits.
@
@ Return(s)   : Number of contiguous, most-significant, leading zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'CPU_DATA' (see 'cpu.h
@                       CPU WORD CONFIGURATION  Note #1').
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  ...  b04  b03  b02  b01  b00    # Leading Zeros
@                             ---  ---  ---       ---  ---  ---  ---  ---    ---------------
@                              1    x    x         x    x    x    x    x            0
@                              0    1    x         x    x    x    x    x            1
@                              0    0    1         x    x    x    x    x            2
@                              :    :    :         :    :    :    :    :            :
@                              :    :    :         :    :    :    :    :            :
@                              0    0    0         1    x    x    x    x           27
@                              0    0    0         0    1    x    x    x           28
@                              0    0    0         0    0    1    x    x           29
@                              0    0    0         0    0    0    1    x           30
@                              0    0    0         0    0    0    0    1           31
@                              0    0    0         0    0    0    0    0           32
@
@
@               (2) MUST be defined in 'cpu_a.asm' (or 'cpu_c.c') if CPU_CFG_LEAD_ZEROS_ASM_PRESENT is
@                   #define'd in 'cpu_port.h'.
@********************************************************************************************************

.thumb_func
CPU_CntLeadZeros:
        CLZ     R0, R0                          @ Count leading zeros
        BX      LR


@********************************************************************************************************
@                                         CPU_CntTrailZeros()
@                                        COUNT TRAILING ZEROS
@
@ Description : Counts the number of contiguous, least-significant, trailing zero bits before the
@                   first binary one bit in a data value.
@
@ Prototype   : CPU_DATA  CPU_CntTrailZeros(CPU_DATA  val);
@
@ Argument(s) : val         Data value to count trailing zero bits.
@
@ Return(s)   : Number of contiguous, least-significant, trailing zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'CPU_DATA' (see 'cpu.h
@                       CPU WORD CONFIGURATION  Note #1').
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
@                             ---  ---  ---  ---  ---       ---  ---  ---    ----------------
@                              x    x    x    x    x         x    x    1            0
@                              x    x    x    x    x         x    1    0            1
@                              x    x    x    x    x         1    0    0            2
@                              :    :    :    :    :         :    :    :            :
@                              :    :    :    :    :         :    :    :            :
@                              x    x    x    x    1         0    0    0           27
@                              x    x    x    1    0         0    0    0           28
@                              x    x    1    0    0         0    0    0           29
@                              x    1    0    0    0         0    0    0           30
@                              1    0    0    0    0         0    0    0           31
@                              0    0    0    0    0         0    0    0           32
@
@
@               (2) MUST be defined in 'cpu_a.asm' (or 'cpu_c.c') if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT is
@                   #define'd in 'cpu_cfg.h' or 'cpu.h'.
@********************************************************************************************************

.thumb_func
CPU_CntTrailZeros:
        RBIT    R0, R0                          @ Reverse bits
        CLZ     R0, R0                          @ Count trailing zeros
        BX      LR


@********************************************************************************************************
@                                            CPU_RevBits()
@                                            REVERSE BITS
@
@ Description : Reverses the bits in a data value.
@
@ Prototypes  : CPU_DATA  CPU_RevBits(CPU_DATA  val);
@
@ Argument(s) : val         Data value to reverse bits.
@
@ Return(s)   : Value with all bits in 'val' reversed (see Note #1).
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application function(s).
@
@ Note(s)     : (1) The final, reversed data value for 'val' is such that :
@
@                       'val's final bit  0       =  'val's original bit  N
@                       'val's final bit  1       =  'val's original bit (N - 1)
@                       'val's final bit  2       =  'val's original bit (N - 2)
@
@                               ...                           ...
@
@                       'val's final bit (N - 2)  =  'val's original bit  2
@                       'val's final bit (N - 1)  =  'val's original bit  1
@                       'val's final bit  N       =  'val's original bit  0
@
@               (2) This function is DEPRECATED and will be removed in a future version of this product.
@********************************************************************************************************

.thumb_func
CPU_RevBits:
        RBIT    R0, R0                          @ Reverse bits
        BX      LR


@********************************************************************************************************
@                                     CPU ASSEMBLY PORT FILE END
@********************************************************************************************************

.end
