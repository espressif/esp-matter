/******************************************************************************
*  Filename:       hw_cpu_nvic_h
*  Revised:        $Date$
*  Revision:       $Revision$
*
* Copyright (c) 2015 - 2017, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1) Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2) Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3) Neither the name of the ORGANIZATION nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __HW_CPU_NVIC_H__
#define __HW_CPU_NVIC_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_NVIC component
//
//*****************************************************************************
// Enables or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ISER0                                            0x00000000

// Enables or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ISER1                                            0x00000004

// Enables or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ISER2                                            0x00000008

// Clears or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ICER0                                            0x00000080

// Clears or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ICER1                                            0x00000084

// Clears or reads the enabled state of each group of 32 interrupts
#define CPU_NVIC_O_ICER2                                            0x00000088

// Enables or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ISPR0                                            0x00000100

// Enables or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ISPR1                                            0x00000104

// Enables or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ISPR2                                            0x00000108

// Clears or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ICPR0                                            0x00000180

// Clears or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ICPR1                                            0x00000184

// Clears or reads the pending state of each group of 32 interrupts
#define CPU_NVIC_O_ICPR2                                            0x00000188

// For each group of 32 interrupts, shows the active state of each interrupt
#define CPU_NVIC_O_IABR0                                            0x00000200

// For each group of 32 interrupts, shows the active state of each interrupt
#define CPU_NVIC_O_IABR1                                            0x00000204

// For each group of 32 interrupts, shows the active state of each interrupt
#define CPU_NVIC_O_IABR2                                            0x00000208

// For each group of 32 interrupts, determines whether each interrupt targets
// Non-secure or Secure state
#define CPU_NVIC_O_ITNS0                                            0x00000280

// For each group of 32 interrupts, determines whether each interrupt targets
// Non-secure or Secure state
#define CPU_NVIC_O_ITNS1                                            0x00000284

// For each group of 32 interrupts, determines whether each interrupt targets
// Non-secure or Secure state
#define CPU_NVIC_O_ITNS2                                            0x00000288

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR0                                             0x00000300

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR1                                             0x00000304

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR2                                             0x00000308

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR3                                             0x0000030C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR4                                             0x00000310

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR5                                             0x00000314

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR6                                             0x00000318

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR7                                             0x0000031C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR8                                             0x00000320

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR9                                             0x00000324

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR10                                            0x00000328

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR11                                            0x0000032C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR12                                            0x00000330

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR13                                            0x00000334

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR14                                            0x00000338

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR15                                            0x0000033C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR16                                            0x00000340

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR17                                            0x00000344

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR18                                            0x00000348

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR19                                            0x0000034C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR20                                            0x00000350

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR21                                            0x00000354

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR22                                            0x00000358

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR23                                            0x0000035C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR24                                            0x00000360

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR25                                            0x00000364

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR26                                            0x00000368

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR27                                            0x0000036C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR28                                            0x00000370

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR29                                            0x00000374

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR30                                            0x00000378

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR31                                            0x0000037C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR32                                            0x00000380

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR33                                            0x00000384

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR34                                            0x00000388

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR35                                            0x0000038C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR36                                            0x00000390

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR37                                            0x00000394

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR38                                            0x00000398

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR39                                            0x0000039C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR40                                            0x000003A0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR41                                            0x000003A4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR42                                            0x000003A8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR43                                            0x000003AC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR44                                            0x000003B0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR45                                            0x000003B4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR46                                            0x000003B8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR47                                            0x000003BC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR48                                            0x000003C0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR49                                            0x000003C4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR50                                            0x000003C8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR51                                            0x000003CC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR52                                            0x000003D0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR53                                            0x000003D4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR54                                            0x000003D8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR55                                            0x000003DC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR56                                            0x000003E0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR57                                            0x000003E4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR58                                            0x000003E8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR59                                            0x000003EC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR60                                            0x000003F0

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR61                                            0x000003F4

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR62                                            0x000003F8

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR63                                            0x000003FC

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR64                                            0x00000400

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR65                                            0x00000404

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR66                                            0x00000408

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR67                                            0x0000040C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR68                                            0x00000410

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR69                                            0x00000414

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR70                                            0x00000418

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR71                                            0x0000041C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR72                                            0x00000420

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR73                                            0x00000424

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR74                                            0x00000428

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR75                                            0x0000042C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR76                                            0x00000430

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR77                                            0x00000434

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR78                                            0x00000438

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR79                                            0x0000043C

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR80                                            0x00000440

// Sets or reads interrupt priorities
#define CPU_NVIC_O_IPR81                                            0x00000444

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISER0
//
//*****************************************************************************
// Field:  [31:0] SETENA
//
// For SETENA[m] in NVIC_ISER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ISER0_SETENA_W                                             32
#define CPU_NVIC_ISER0_SETENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ISER0_SETENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISER1
//
//*****************************************************************************
// Field:  [31:0] SETENA
//
// For SETENA[m] in NVIC_ISER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ISER1_SETENA_W                                             32
#define CPU_NVIC_ISER1_SETENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ISER1_SETENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISER2
//
//*****************************************************************************
// Field:  [31:0] SETENA
//
// For SETENA[m] in NVIC_ISER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ISER2_SETENA_W                                             32
#define CPU_NVIC_ISER2_SETENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ISER2_SETENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICER0
//
//*****************************************************************************
// Field:  [31:0] CLRENA
//
// For CLRENA[m] in NVIC_ICER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ICER0_CLRENA_W                                             32
#define CPU_NVIC_ICER0_CLRENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ICER0_CLRENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICER1
//
//*****************************************************************************
// Field:  [31:0] CLRENA
//
// For CLRENA[m] in NVIC_ICER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ICER1_CLRENA_W                                             32
#define CPU_NVIC_ICER1_CLRENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ICER1_CLRENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICER2
//
//*****************************************************************************
// Field:  [31:0] CLRENA
//
// For CLRENA[m] in NVIC_ICER*n, indicates whether interrupt 32*n + m is
// enabled
#define CPU_NVIC_ICER2_CLRENA_W                                             32
#define CPU_NVIC_ICER2_CLRENA_M                                     0xFFFFFFFF
#define CPU_NVIC_ICER2_CLRENA_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISPR0
//
//*****************************************************************************
// Field:  [31:0] SETPEND
//
// For SETPEND[m] in NVIC_ISPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ISPR0_SETPEND_W                                            32
#define CPU_NVIC_ISPR0_SETPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ISPR0_SETPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISPR1
//
//*****************************************************************************
// Field:  [31:0] SETPEND
//
// For SETPEND[m] in NVIC_ISPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ISPR1_SETPEND_W                                            32
#define CPU_NVIC_ISPR1_SETPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ISPR1_SETPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ISPR2
//
//*****************************************************************************
// Field:  [31:0] SETPEND
//
// For SETPEND[m] in NVIC_ISPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ISPR2_SETPEND_W                                            32
#define CPU_NVIC_ISPR2_SETPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ISPR2_SETPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICPR0
//
//*****************************************************************************
// Field:  [31:0] CLRPEND
//
// For CLRPEND[m] in NVIC_ICPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ICPR0_CLRPEND_W                                            32
#define CPU_NVIC_ICPR0_CLRPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ICPR0_CLRPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICPR1
//
//*****************************************************************************
// Field:  [31:0] CLRPEND
//
// For CLRPEND[m] in NVIC_ICPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ICPR1_CLRPEND_W                                            32
#define CPU_NVIC_ICPR1_CLRPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ICPR1_CLRPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ICPR2
//
//*****************************************************************************
// Field:  [31:0] CLRPEND
//
// For CLRPEND[m] in NVIC_ICPR*n, indicates whether interrupt 32*n + m is
// pending
#define CPU_NVIC_ICPR2_CLRPEND_W                                            32
#define CPU_NVIC_ICPR2_CLRPEND_M                                    0xFFFFFFFF
#define CPU_NVIC_ICPR2_CLRPEND_S                                             0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IABR0
//
//*****************************************************************************
// Field:  [31:0] ACTIVE
//
// For ACTIVE[m] in NVIC_IABR*n, indicates the active state for interrupt
// 32*n+m
#define CPU_NVIC_IABR0_ACTIVE_W                                             32
#define CPU_NVIC_IABR0_ACTIVE_M                                     0xFFFFFFFF
#define CPU_NVIC_IABR0_ACTIVE_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IABR1
//
//*****************************************************************************
// Field:  [31:0] ACTIVE
//
// For ACTIVE[m] in NVIC_IABR*n, indicates the active state for interrupt
// 32*n+m
#define CPU_NVIC_IABR1_ACTIVE_W                                             32
#define CPU_NVIC_IABR1_ACTIVE_M                                     0xFFFFFFFF
#define CPU_NVIC_IABR1_ACTIVE_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IABR2
//
//*****************************************************************************
// Field:  [31:0] ACTIVE
//
// For ACTIVE[m] in NVIC_IABR*n, indicates the active state for interrupt
// 32*n+m
#define CPU_NVIC_IABR2_ACTIVE_W                                             32
#define CPU_NVIC_IABR2_ACTIVE_M                                     0xFFFFFFFF
#define CPU_NVIC_IABR2_ACTIVE_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ITNS0
//
//*****************************************************************************
// Field:  [31:0] ITNS
//
// For ITNS[m] in NVIC_ITNS*n, `IAAMO the target Security state for interrupt
// 32*n+m
#define CPU_NVIC_ITNS0_ITNS_W                                               32
#define CPU_NVIC_ITNS0_ITNS_M                                       0xFFFFFFFF
#define CPU_NVIC_ITNS0_ITNS_S                                                0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ITNS1
//
//*****************************************************************************
// Field:  [31:0] ITNS
//
// For ITNS[m] in NVIC_ITNS*n, `IAAMO the target Security state for interrupt
// 32*n+m
#define CPU_NVIC_ITNS1_ITNS_W                                               32
#define CPU_NVIC_ITNS1_ITNS_M                                       0xFFFFFFFF
#define CPU_NVIC_ITNS1_ITNS_S                                                0

//*****************************************************************************
//
// Register: CPU_NVIC_O_ITNS2
//
//*****************************************************************************
// Field:  [31:0] ITNS
//
// For ITNS[m] in NVIC_ITNS*n, `IAAMO the target Security state for interrupt
// 32*n+m
#define CPU_NVIC_ITNS2_ITNS_W                                               32
#define CPU_NVIC_ITNS2_ITNS_M                                       0xFFFFFFFF
#define CPU_NVIC_ITNS2_ITNS_S                                                0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR0
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*0, `IAAMO the priority of interrupt number 4*0+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR0_PRI_N3_W                                               8
#define CPU_NVIC_IPR0_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR0_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*0, `IAAMO the priority of interrupt number 4*0+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR0_PRI_N2_W                                               8
#define CPU_NVIC_IPR0_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR0_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*0, `IAAMO the priority of interrupt number 4*0+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR0_PRI_N1_W                                               8
#define CPU_NVIC_IPR0_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR0_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*0, `IAAMO the priority of interrupt number 4*0+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR0_PRI_N0_W                                               8
#define CPU_NVIC_IPR0_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR0_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR1
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*1, `IAAMO the priority of interrupt number 4*1+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR1_PRI_N3_W                                               8
#define CPU_NVIC_IPR1_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR1_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*1, `IAAMO the priority of interrupt number 4*1+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR1_PRI_N2_W                                               8
#define CPU_NVIC_IPR1_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR1_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*1, `IAAMO the priority of interrupt number 4*1+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR1_PRI_N1_W                                               8
#define CPU_NVIC_IPR1_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR1_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*1, `IAAMO the priority of interrupt number 4*1+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR1_PRI_N0_W                                               8
#define CPU_NVIC_IPR1_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR1_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR2
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*2, `IAAMO the priority of interrupt number 4*2+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR2_PRI_N3_W                                               8
#define CPU_NVIC_IPR2_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR2_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*2, `IAAMO the priority of interrupt number 4*2+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR2_PRI_N2_W                                               8
#define CPU_NVIC_IPR2_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR2_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*2, `IAAMO the priority of interrupt number 4*2+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR2_PRI_N1_W                                               8
#define CPU_NVIC_IPR2_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR2_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*2, `IAAMO the priority of interrupt number 4*2+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR2_PRI_N0_W                                               8
#define CPU_NVIC_IPR2_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR2_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR3
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*3, `IAAMO the priority of interrupt number 4*3+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR3_PRI_N3_W                                               8
#define CPU_NVIC_IPR3_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR3_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*3, `IAAMO the priority of interrupt number 4*3+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR3_PRI_N2_W                                               8
#define CPU_NVIC_IPR3_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR3_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*3, `IAAMO the priority of interrupt number 4*3+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR3_PRI_N1_W                                               8
#define CPU_NVIC_IPR3_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR3_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*3, `IAAMO the priority of interrupt number 4*3+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR3_PRI_N0_W                                               8
#define CPU_NVIC_IPR3_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR3_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR4
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*4, `IAAMO the priority of interrupt number 4*4+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR4_PRI_N3_W                                               8
#define CPU_NVIC_IPR4_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR4_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*4, `IAAMO the priority of interrupt number 4*4+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR4_PRI_N2_W                                               8
#define CPU_NVIC_IPR4_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR4_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*4, `IAAMO the priority of interrupt number 4*4+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR4_PRI_N1_W                                               8
#define CPU_NVIC_IPR4_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR4_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*4, `IAAMO the priority of interrupt number 4*4+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR4_PRI_N0_W                                               8
#define CPU_NVIC_IPR4_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR4_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR5
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*5, `IAAMO the priority of interrupt number 4*5+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR5_PRI_N3_W                                               8
#define CPU_NVIC_IPR5_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR5_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*5, `IAAMO the priority of interrupt number 4*5+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR5_PRI_N2_W                                               8
#define CPU_NVIC_IPR5_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR5_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*5, `IAAMO the priority of interrupt number 4*5+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR5_PRI_N1_W                                               8
#define CPU_NVIC_IPR5_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR5_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*5, `IAAMO the priority of interrupt number 4*5+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR5_PRI_N0_W                                               8
#define CPU_NVIC_IPR5_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR5_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR6
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*6, `IAAMO the priority of interrupt number 4*6+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR6_PRI_N3_W                                               8
#define CPU_NVIC_IPR6_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR6_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*6, `IAAMO the priority of interrupt number 4*6+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR6_PRI_N2_W                                               8
#define CPU_NVIC_IPR6_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR6_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*6, `IAAMO the priority of interrupt number 4*6+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR6_PRI_N1_W                                               8
#define CPU_NVIC_IPR6_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR6_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*6, `IAAMO the priority of interrupt number 4*6+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR6_PRI_N0_W                                               8
#define CPU_NVIC_IPR6_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR6_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR7
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*7, `IAAMO the priority of interrupt number 4*7+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR7_PRI_N3_W                                               8
#define CPU_NVIC_IPR7_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR7_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*7, `IAAMO the priority of interrupt number 4*7+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR7_PRI_N2_W                                               8
#define CPU_NVIC_IPR7_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR7_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*7, `IAAMO the priority of interrupt number 4*7+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR7_PRI_N1_W                                               8
#define CPU_NVIC_IPR7_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR7_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*7, `IAAMO the priority of interrupt number 4*7+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR7_PRI_N0_W                                               8
#define CPU_NVIC_IPR7_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR7_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR8
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*8, `IAAMO the priority of interrupt number 4*8+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR8_PRI_N3_W                                               8
#define CPU_NVIC_IPR8_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR8_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*8, `IAAMO the priority of interrupt number 4*8+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR8_PRI_N2_W                                               8
#define CPU_NVIC_IPR8_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR8_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*8, `IAAMO the priority of interrupt number 4*8+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR8_PRI_N1_W                                               8
#define CPU_NVIC_IPR8_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR8_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*8, `IAAMO the priority of interrupt number 4*8+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR8_PRI_N0_W                                               8
#define CPU_NVIC_IPR8_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR8_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR9
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*9, `IAAMO the priority of interrupt number 4*9+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR9_PRI_N3_W                                               8
#define CPU_NVIC_IPR9_PRI_N3_M                                      0xFF000000
#define CPU_NVIC_IPR9_PRI_N3_S                                              24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*9, `IAAMO the priority of interrupt number 4*9+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR9_PRI_N2_W                                               8
#define CPU_NVIC_IPR9_PRI_N2_M                                      0x00FF0000
#define CPU_NVIC_IPR9_PRI_N2_S                                              16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*9, `IAAMO the priority of interrupt number 4*9+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR9_PRI_N1_W                                               8
#define CPU_NVIC_IPR9_PRI_N1_M                                      0x0000FF00
#define CPU_NVIC_IPR9_PRI_N1_S                                               8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*9, `IAAMO the priority of interrupt number 4*9+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR9_PRI_N0_W                                               8
#define CPU_NVIC_IPR9_PRI_N0_M                                      0x000000FF
#define CPU_NVIC_IPR9_PRI_N0_S                                               0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR10
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*10, `IAAMO the priority of interrupt number 4*10+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR10_PRI_N3_W                                              8
#define CPU_NVIC_IPR10_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR10_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*10, `IAAMO the priority of interrupt number 4*10+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR10_PRI_N2_W                                              8
#define CPU_NVIC_IPR10_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR10_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*10, `IAAMO the priority of interrupt number 4*10+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR10_PRI_N1_W                                              8
#define CPU_NVIC_IPR10_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR10_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*10, `IAAMO the priority of interrupt number 4*10+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR10_PRI_N0_W                                              8
#define CPU_NVIC_IPR10_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR10_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR11
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*11, `IAAMO the priority of interrupt number 4*11+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR11_PRI_N3_W                                              8
#define CPU_NVIC_IPR11_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR11_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*11, `IAAMO the priority of interrupt number 4*11+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR11_PRI_N2_W                                              8
#define CPU_NVIC_IPR11_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR11_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*11, `IAAMO the priority of interrupt number 4*11+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR11_PRI_N1_W                                              8
#define CPU_NVIC_IPR11_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR11_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*11, `IAAMO the priority of interrupt number 4*11+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR11_PRI_N0_W                                              8
#define CPU_NVIC_IPR11_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR11_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR12
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*12, `IAAMO the priority of interrupt number 4*12+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR12_PRI_N3_W                                              8
#define CPU_NVIC_IPR12_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR12_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*12, `IAAMO the priority of interrupt number 4*12+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR12_PRI_N2_W                                              8
#define CPU_NVIC_IPR12_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR12_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*12, `IAAMO the priority of interrupt number 4*12+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR12_PRI_N1_W                                              8
#define CPU_NVIC_IPR12_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR12_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*12, `IAAMO the priority of interrupt number 4*12+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR12_PRI_N0_W                                              8
#define CPU_NVIC_IPR12_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR12_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR13
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*13, `IAAMO the priority of interrupt number 4*13+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR13_PRI_N3_W                                              8
#define CPU_NVIC_IPR13_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR13_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*13, `IAAMO the priority of interrupt number 4*13+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR13_PRI_N2_W                                              8
#define CPU_NVIC_IPR13_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR13_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*13, `IAAMO the priority of interrupt number 4*13+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR13_PRI_N1_W                                              8
#define CPU_NVIC_IPR13_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR13_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*13, `IAAMO the priority of interrupt number 4*13+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR13_PRI_N0_W                                              8
#define CPU_NVIC_IPR13_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR13_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR14
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*14, `IAAMO the priority of interrupt number 4*14+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR14_PRI_N3_W                                              8
#define CPU_NVIC_IPR14_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR14_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*14, `IAAMO the priority of interrupt number 4*14+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR14_PRI_N2_W                                              8
#define CPU_NVIC_IPR14_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR14_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*14, `IAAMO the priority of interrupt number 4*14+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR14_PRI_N1_W                                              8
#define CPU_NVIC_IPR14_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR14_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*14, `IAAMO the priority of interrupt number 4*14+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR14_PRI_N0_W                                              8
#define CPU_NVIC_IPR14_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR14_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR15
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*15, `IAAMO the priority of interrupt number 4*15+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR15_PRI_N3_W                                              8
#define CPU_NVIC_IPR15_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR15_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*15, `IAAMO the priority of interrupt number 4*15+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR15_PRI_N2_W                                              8
#define CPU_NVIC_IPR15_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR15_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*15, `IAAMO the priority of interrupt number 4*15+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR15_PRI_N1_W                                              8
#define CPU_NVIC_IPR15_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR15_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*15, `IAAMO the priority of interrupt number 4*15+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR15_PRI_N0_W                                              8
#define CPU_NVIC_IPR15_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR15_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR16
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*16, `IAAMO the priority of interrupt number 4*16+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR16_PRI_N3_W                                              8
#define CPU_NVIC_IPR16_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR16_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*16, `IAAMO the priority of interrupt number 4*16+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR16_PRI_N2_W                                              8
#define CPU_NVIC_IPR16_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR16_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*16, `IAAMO the priority of interrupt number 4*16+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR16_PRI_N1_W                                              8
#define CPU_NVIC_IPR16_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR16_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*16, `IAAMO the priority of interrupt number 4*16+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR16_PRI_N0_W                                              8
#define CPU_NVIC_IPR16_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR16_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR17
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*17, `IAAMO the priority of interrupt number 4*17+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR17_PRI_N3_W                                              8
#define CPU_NVIC_IPR17_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR17_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*17, `IAAMO the priority of interrupt number 4*17+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR17_PRI_N2_W                                              8
#define CPU_NVIC_IPR17_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR17_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*17, `IAAMO the priority of interrupt number 4*17+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR17_PRI_N1_W                                              8
#define CPU_NVIC_IPR17_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR17_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*17, `IAAMO the priority of interrupt number 4*17+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR17_PRI_N0_W                                              8
#define CPU_NVIC_IPR17_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR17_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR18
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*18, `IAAMO the priority of interrupt number 4*18+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR18_PRI_N3_W                                              8
#define CPU_NVIC_IPR18_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR18_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*18, `IAAMO the priority of interrupt number 4*18+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR18_PRI_N2_W                                              8
#define CPU_NVIC_IPR18_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR18_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*18, `IAAMO the priority of interrupt number 4*18+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR18_PRI_N1_W                                              8
#define CPU_NVIC_IPR18_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR18_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*18, `IAAMO the priority of interrupt number 4*18+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR18_PRI_N0_W                                              8
#define CPU_NVIC_IPR18_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR18_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR19
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*19, `IAAMO the priority of interrupt number 4*19+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR19_PRI_N3_W                                              8
#define CPU_NVIC_IPR19_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR19_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*19, `IAAMO the priority of interrupt number 4*19+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR19_PRI_N2_W                                              8
#define CPU_NVIC_IPR19_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR19_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*19, `IAAMO the priority of interrupt number 4*19+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR19_PRI_N1_W                                              8
#define CPU_NVIC_IPR19_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR19_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*19, `IAAMO the priority of interrupt number 4*19+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR19_PRI_N0_W                                              8
#define CPU_NVIC_IPR19_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR19_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR20
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*20, `IAAMO the priority of interrupt number 4*20+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR20_PRI_N3_W                                              8
#define CPU_NVIC_IPR20_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR20_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*20, `IAAMO the priority of interrupt number 4*20+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR20_PRI_N2_W                                              8
#define CPU_NVIC_IPR20_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR20_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*20, `IAAMO the priority of interrupt number 4*20+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR20_PRI_N1_W                                              8
#define CPU_NVIC_IPR20_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR20_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*20, `IAAMO the priority of interrupt number 4*20+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR20_PRI_N0_W                                              8
#define CPU_NVIC_IPR20_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR20_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR21
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*21, `IAAMO the priority of interrupt number 4*21+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR21_PRI_N3_W                                              8
#define CPU_NVIC_IPR21_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR21_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*21, `IAAMO the priority of interrupt number 4*21+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR21_PRI_N2_W                                              8
#define CPU_NVIC_IPR21_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR21_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*21, `IAAMO the priority of interrupt number 4*21+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR21_PRI_N1_W                                              8
#define CPU_NVIC_IPR21_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR21_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*21, `IAAMO the priority of interrupt number 4*21+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR21_PRI_N0_W                                              8
#define CPU_NVIC_IPR21_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR21_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR22
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*22, `IAAMO the priority of interrupt number 4*22+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR22_PRI_N3_W                                              8
#define CPU_NVIC_IPR22_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR22_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*22, `IAAMO the priority of interrupt number 4*22+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR22_PRI_N2_W                                              8
#define CPU_NVIC_IPR22_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR22_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*22, `IAAMO the priority of interrupt number 4*22+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR22_PRI_N1_W                                              8
#define CPU_NVIC_IPR22_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR22_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*22, `IAAMO the priority of interrupt number 4*22+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR22_PRI_N0_W                                              8
#define CPU_NVIC_IPR22_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR22_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR23
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*23, `IAAMO the priority of interrupt number 4*23+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR23_PRI_N3_W                                              8
#define CPU_NVIC_IPR23_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR23_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*23, `IAAMO the priority of interrupt number 4*23+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR23_PRI_N2_W                                              8
#define CPU_NVIC_IPR23_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR23_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*23, `IAAMO the priority of interrupt number 4*23+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR23_PRI_N1_W                                              8
#define CPU_NVIC_IPR23_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR23_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*23, `IAAMO the priority of interrupt number 4*23+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR23_PRI_N0_W                                              8
#define CPU_NVIC_IPR23_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR23_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR24
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*24, `IAAMO the priority of interrupt number 4*24+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR24_PRI_N3_W                                              8
#define CPU_NVIC_IPR24_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR24_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*24, `IAAMO the priority of interrupt number 4*24+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR24_PRI_N2_W                                              8
#define CPU_NVIC_IPR24_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR24_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*24, `IAAMO the priority of interrupt number 4*24+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR24_PRI_N1_W                                              8
#define CPU_NVIC_IPR24_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR24_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*24, `IAAMO the priority of interrupt number 4*24+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR24_PRI_N0_W                                              8
#define CPU_NVIC_IPR24_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR24_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR25
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*25, `IAAMO the priority of interrupt number 4*25+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR25_PRI_N3_W                                              8
#define CPU_NVIC_IPR25_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR25_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*25, `IAAMO the priority of interrupt number 4*25+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR25_PRI_N2_W                                              8
#define CPU_NVIC_IPR25_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR25_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*25, `IAAMO the priority of interrupt number 4*25+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR25_PRI_N1_W                                              8
#define CPU_NVIC_IPR25_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR25_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*25, `IAAMO the priority of interrupt number 4*25+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR25_PRI_N0_W                                              8
#define CPU_NVIC_IPR25_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR25_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR26
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*26, `IAAMO the priority of interrupt number 4*26+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR26_PRI_N3_W                                              8
#define CPU_NVIC_IPR26_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR26_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*26, `IAAMO the priority of interrupt number 4*26+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR26_PRI_N2_W                                              8
#define CPU_NVIC_IPR26_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR26_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*26, `IAAMO the priority of interrupt number 4*26+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR26_PRI_N1_W                                              8
#define CPU_NVIC_IPR26_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR26_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*26, `IAAMO the priority of interrupt number 4*26+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR26_PRI_N0_W                                              8
#define CPU_NVIC_IPR26_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR26_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR27
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*27, `IAAMO the priority of interrupt number 4*27+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR27_PRI_N3_W                                              8
#define CPU_NVIC_IPR27_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR27_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*27, `IAAMO the priority of interrupt number 4*27+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR27_PRI_N2_W                                              8
#define CPU_NVIC_IPR27_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR27_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*27, `IAAMO the priority of interrupt number 4*27+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR27_PRI_N1_W                                              8
#define CPU_NVIC_IPR27_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR27_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*27, `IAAMO the priority of interrupt number 4*27+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR27_PRI_N0_W                                              8
#define CPU_NVIC_IPR27_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR27_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR28
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*28, `IAAMO the priority of interrupt number 4*28+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR28_PRI_N3_W                                              8
#define CPU_NVIC_IPR28_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR28_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*28, `IAAMO the priority of interrupt number 4*28+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR28_PRI_N2_W                                              8
#define CPU_NVIC_IPR28_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR28_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*28, `IAAMO the priority of interrupt number 4*28+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR28_PRI_N1_W                                              8
#define CPU_NVIC_IPR28_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR28_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*28, `IAAMO the priority of interrupt number 4*28+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR28_PRI_N0_W                                              8
#define CPU_NVIC_IPR28_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR28_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR29
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*29, `IAAMO the priority of interrupt number 4*29+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR29_PRI_N3_W                                              8
#define CPU_NVIC_IPR29_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR29_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*29, `IAAMO the priority of interrupt number 4*29+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR29_PRI_N2_W                                              8
#define CPU_NVIC_IPR29_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR29_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*29, `IAAMO the priority of interrupt number 4*29+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR29_PRI_N1_W                                              8
#define CPU_NVIC_IPR29_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR29_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*29, `IAAMO the priority of interrupt number 4*29+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR29_PRI_N0_W                                              8
#define CPU_NVIC_IPR29_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR29_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR30
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*30, `IAAMO the priority of interrupt number 4*30+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR30_PRI_N3_W                                              8
#define CPU_NVIC_IPR30_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR30_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*30, `IAAMO the priority of interrupt number 4*30+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR30_PRI_N2_W                                              8
#define CPU_NVIC_IPR30_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR30_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*30, `IAAMO the priority of interrupt number 4*30+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR30_PRI_N1_W                                              8
#define CPU_NVIC_IPR30_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR30_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*30, `IAAMO the priority of interrupt number 4*30+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR30_PRI_N0_W                                              8
#define CPU_NVIC_IPR30_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR30_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR31
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*31, `IAAMO the priority of interrupt number 4*31+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR31_PRI_N3_W                                              8
#define CPU_NVIC_IPR31_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR31_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*31, `IAAMO the priority of interrupt number 4*31+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR31_PRI_N2_W                                              8
#define CPU_NVIC_IPR31_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR31_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*31, `IAAMO the priority of interrupt number 4*31+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR31_PRI_N1_W                                              8
#define CPU_NVIC_IPR31_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR31_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*31, `IAAMO the priority of interrupt number 4*31+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR31_PRI_N0_W                                              8
#define CPU_NVIC_IPR31_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR31_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR32
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*32, `IAAMO the priority of interrupt number 4*32+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR32_PRI_N3_W                                              8
#define CPU_NVIC_IPR32_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR32_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*32, `IAAMO the priority of interrupt number 4*32+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR32_PRI_N2_W                                              8
#define CPU_NVIC_IPR32_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR32_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*32, `IAAMO the priority of interrupt number 4*32+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR32_PRI_N1_W                                              8
#define CPU_NVIC_IPR32_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR32_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*32, `IAAMO the priority of interrupt number 4*32+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR32_PRI_N0_W                                              8
#define CPU_NVIC_IPR32_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR32_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR33
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*33, `IAAMO the priority of interrupt number 4*33+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR33_PRI_N3_W                                              8
#define CPU_NVIC_IPR33_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR33_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*33, `IAAMO the priority of interrupt number 4*33+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR33_PRI_N2_W                                              8
#define CPU_NVIC_IPR33_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR33_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*33, `IAAMO the priority of interrupt number 4*33+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR33_PRI_N1_W                                              8
#define CPU_NVIC_IPR33_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR33_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*33, `IAAMO the priority of interrupt number 4*33+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR33_PRI_N0_W                                              8
#define CPU_NVIC_IPR33_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR33_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR34
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*34, `IAAMO the priority of interrupt number 4*34+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR34_PRI_N3_W                                              8
#define CPU_NVIC_IPR34_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR34_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*34, `IAAMO the priority of interrupt number 4*34+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR34_PRI_N2_W                                              8
#define CPU_NVIC_IPR34_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR34_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*34, `IAAMO the priority of interrupt number 4*34+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR34_PRI_N1_W                                              8
#define CPU_NVIC_IPR34_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR34_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*34, `IAAMO the priority of interrupt number 4*34+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR34_PRI_N0_W                                              8
#define CPU_NVIC_IPR34_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR34_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR35
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*35, `IAAMO the priority of interrupt number 4*35+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR35_PRI_N3_W                                              8
#define CPU_NVIC_IPR35_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR35_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*35, `IAAMO the priority of interrupt number 4*35+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR35_PRI_N2_W                                              8
#define CPU_NVIC_IPR35_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR35_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*35, `IAAMO the priority of interrupt number 4*35+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR35_PRI_N1_W                                              8
#define CPU_NVIC_IPR35_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR35_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*35, `IAAMO the priority of interrupt number 4*35+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR35_PRI_N0_W                                              8
#define CPU_NVIC_IPR35_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR35_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR36
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*36, `IAAMO the priority of interrupt number 4*36+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR36_PRI_N3_W                                              8
#define CPU_NVIC_IPR36_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR36_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*36, `IAAMO the priority of interrupt number 4*36+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR36_PRI_N2_W                                              8
#define CPU_NVIC_IPR36_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR36_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*36, `IAAMO the priority of interrupt number 4*36+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR36_PRI_N1_W                                              8
#define CPU_NVIC_IPR36_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR36_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*36, `IAAMO the priority of interrupt number 4*36+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR36_PRI_N0_W                                              8
#define CPU_NVIC_IPR36_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR36_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR37
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*37, `IAAMO the priority of interrupt number 4*37+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR37_PRI_N3_W                                              8
#define CPU_NVIC_IPR37_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR37_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*37, `IAAMO the priority of interrupt number 4*37+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR37_PRI_N2_W                                              8
#define CPU_NVIC_IPR37_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR37_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*37, `IAAMO the priority of interrupt number 4*37+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR37_PRI_N1_W                                              8
#define CPU_NVIC_IPR37_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR37_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*37, `IAAMO the priority of interrupt number 4*37+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR37_PRI_N0_W                                              8
#define CPU_NVIC_IPR37_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR37_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR38
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*38, `IAAMO the priority of interrupt number 4*38+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR38_PRI_N3_W                                              8
#define CPU_NVIC_IPR38_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR38_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*38, `IAAMO the priority of interrupt number 4*38+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR38_PRI_N2_W                                              8
#define CPU_NVIC_IPR38_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR38_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*38, `IAAMO the priority of interrupt number 4*38+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR38_PRI_N1_W                                              8
#define CPU_NVIC_IPR38_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR38_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*38, `IAAMO the priority of interrupt number 4*38+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR38_PRI_N0_W                                              8
#define CPU_NVIC_IPR38_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR38_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR39
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*39, `IAAMO the priority of interrupt number 4*39+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR39_PRI_N3_W                                              8
#define CPU_NVIC_IPR39_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR39_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*39, `IAAMO the priority of interrupt number 4*39+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR39_PRI_N2_W                                              8
#define CPU_NVIC_IPR39_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR39_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*39, `IAAMO the priority of interrupt number 4*39+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR39_PRI_N1_W                                              8
#define CPU_NVIC_IPR39_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR39_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*39, `IAAMO the priority of interrupt number 4*39+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR39_PRI_N0_W                                              8
#define CPU_NVIC_IPR39_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR39_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR40
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*40, `IAAMO the priority of interrupt number 4*40+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR40_PRI_N3_W                                              8
#define CPU_NVIC_IPR40_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR40_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*40, `IAAMO the priority of interrupt number 4*40+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR40_PRI_N2_W                                              8
#define CPU_NVIC_IPR40_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR40_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*40, `IAAMO the priority of interrupt number 4*40+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR40_PRI_N1_W                                              8
#define CPU_NVIC_IPR40_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR40_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*40, `IAAMO the priority of interrupt number 4*40+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR40_PRI_N0_W                                              8
#define CPU_NVIC_IPR40_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR40_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR41
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*41, `IAAMO the priority of interrupt number 4*41+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR41_PRI_N3_W                                              8
#define CPU_NVIC_IPR41_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR41_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*41, `IAAMO the priority of interrupt number 4*41+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR41_PRI_N2_W                                              8
#define CPU_NVIC_IPR41_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR41_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*41, `IAAMO the priority of interrupt number 4*41+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR41_PRI_N1_W                                              8
#define CPU_NVIC_IPR41_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR41_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*41, `IAAMO the priority of interrupt number 4*41+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR41_PRI_N0_W                                              8
#define CPU_NVIC_IPR41_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR41_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR42
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*42, `IAAMO the priority of interrupt number 4*42+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR42_PRI_N3_W                                              8
#define CPU_NVIC_IPR42_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR42_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*42, `IAAMO the priority of interrupt number 4*42+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR42_PRI_N2_W                                              8
#define CPU_NVIC_IPR42_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR42_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*42, `IAAMO the priority of interrupt number 4*42+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR42_PRI_N1_W                                              8
#define CPU_NVIC_IPR42_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR42_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*42, `IAAMO the priority of interrupt number 4*42+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR42_PRI_N0_W                                              8
#define CPU_NVIC_IPR42_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR42_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR43
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*43, `IAAMO the priority of interrupt number 4*43+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR43_PRI_N3_W                                              8
#define CPU_NVIC_IPR43_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR43_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*43, `IAAMO the priority of interrupt number 4*43+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR43_PRI_N2_W                                              8
#define CPU_NVIC_IPR43_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR43_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*43, `IAAMO the priority of interrupt number 4*43+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR43_PRI_N1_W                                              8
#define CPU_NVIC_IPR43_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR43_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*43, `IAAMO the priority of interrupt number 4*43+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR43_PRI_N0_W                                              8
#define CPU_NVIC_IPR43_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR43_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR44
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*44, `IAAMO the priority of interrupt number 4*44+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR44_PRI_N3_W                                              8
#define CPU_NVIC_IPR44_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR44_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*44, `IAAMO the priority of interrupt number 4*44+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR44_PRI_N2_W                                              8
#define CPU_NVIC_IPR44_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR44_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*44, `IAAMO the priority of interrupt number 4*44+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR44_PRI_N1_W                                              8
#define CPU_NVIC_IPR44_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR44_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*44, `IAAMO the priority of interrupt number 4*44+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR44_PRI_N0_W                                              8
#define CPU_NVIC_IPR44_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR44_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR45
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*45, `IAAMO the priority of interrupt number 4*45+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR45_PRI_N3_W                                              8
#define CPU_NVIC_IPR45_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR45_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*45, `IAAMO the priority of interrupt number 4*45+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR45_PRI_N2_W                                              8
#define CPU_NVIC_IPR45_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR45_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*45, `IAAMO the priority of interrupt number 4*45+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR45_PRI_N1_W                                              8
#define CPU_NVIC_IPR45_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR45_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*45, `IAAMO the priority of interrupt number 4*45+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR45_PRI_N0_W                                              8
#define CPU_NVIC_IPR45_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR45_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR46
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*46, `IAAMO the priority of interrupt number 4*46+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR46_PRI_N3_W                                              8
#define CPU_NVIC_IPR46_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR46_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*46, `IAAMO the priority of interrupt number 4*46+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR46_PRI_N2_W                                              8
#define CPU_NVIC_IPR46_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR46_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*46, `IAAMO the priority of interrupt number 4*46+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR46_PRI_N1_W                                              8
#define CPU_NVIC_IPR46_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR46_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*46, `IAAMO the priority of interrupt number 4*46+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR46_PRI_N0_W                                              8
#define CPU_NVIC_IPR46_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR46_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR47
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*47, `IAAMO the priority of interrupt number 4*47+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR47_PRI_N3_W                                              8
#define CPU_NVIC_IPR47_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR47_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*47, `IAAMO the priority of interrupt number 4*47+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR47_PRI_N2_W                                              8
#define CPU_NVIC_IPR47_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR47_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*47, `IAAMO the priority of interrupt number 4*47+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR47_PRI_N1_W                                              8
#define CPU_NVIC_IPR47_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR47_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*47, `IAAMO the priority of interrupt number 4*47+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR47_PRI_N0_W                                              8
#define CPU_NVIC_IPR47_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR47_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR48
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*48, `IAAMO the priority of interrupt number 4*48+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR48_PRI_N3_W                                              8
#define CPU_NVIC_IPR48_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR48_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*48, `IAAMO the priority of interrupt number 4*48+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR48_PRI_N2_W                                              8
#define CPU_NVIC_IPR48_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR48_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*48, `IAAMO the priority of interrupt number 4*48+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR48_PRI_N1_W                                              8
#define CPU_NVIC_IPR48_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR48_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*48, `IAAMO the priority of interrupt number 4*48+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR48_PRI_N0_W                                              8
#define CPU_NVIC_IPR48_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR48_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR49
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*49, `IAAMO the priority of interrupt number 4*49+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR49_PRI_N3_W                                              8
#define CPU_NVIC_IPR49_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR49_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*49, `IAAMO the priority of interrupt number 4*49+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR49_PRI_N2_W                                              8
#define CPU_NVIC_IPR49_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR49_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*49, `IAAMO the priority of interrupt number 4*49+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR49_PRI_N1_W                                              8
#define CPU_NVIC_IPR49_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR49_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*49, `IAAMO the priority of interrupt number 4*49+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR49_PRI_N0_W                                              8
#define CPU_NVIC_IPR49_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR49_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR50
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*50, `IAAMO the priority of interrupt number 4*50+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR50_PRI_N3_W                                              8
#define CPU_NVIC_IPR50_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR50_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*50, `IAAMO the priority of interrupt number 4*50+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR50_PRI_N2_W                                              8
#define CPU_NVIC_IPR50_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR50_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*50, `IAAMO the priority of interrupt number 4*50+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR50_PRI_N1_W                                              8
#define CPU_NVIC_IPR50_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR50_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*50, `IAAMO the priority of interrupt number 4*50+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR50_PRI_N0_W                                              8
#define CPU_NVIC_IPR50_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR50_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR51
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*51, `IAAMO the priority of interrupt number 4*51+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR51_PRI_N3_W                                              8
#define CPU_NVIC_IPR51_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR51_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*51, `IAAMO the priority of interrupt number 4*51+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR51_PRI_N2_W                                              8
#define CPU_NVIC_IPR51_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR51_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*51, `IAAMO the priority of interrupt number 4*51+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR51_PRI_N1_W                                              8
#define CPU_NVIC_IPR51_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR51_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*51, `IAAMO the priority of interrupt number 4*51+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR51_PRI_N0_W                                              8
#define CPU_NVIC_IPR51_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR51_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR52
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*52, `IAAMO the priority of interrupt number 4*52+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR52_PRI_N3_W                                              8
#define CPU_NVIC_IPR52_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR52_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*52, `IAAMO the priority of interrupt number 4*52+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR52_PRI_N2_W                                              8
#define CPU_NVIC_IPR52_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR52_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*52, `IAAMO the priority of interrupt number 4*52+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR52_PRI_N1_W                                              8
#define CPU_NVIC_IPR52_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR52_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*52, `IAAMO the priority of interrupt number 4*52+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR52_PRI_N0_W                                              8
#define CPU_NVIC_IPR52_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR52_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR53
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*53, `IAAMO the priority of interrupt number 4*53+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR53_PRI_N3_W                                              8
#define CPU_NVIC_IPR53_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR53_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*53, `IAAMO the priority of interrupt number 4*53+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR53_PRI_N2_W                                              8
#define CPU_NVIC_IPR53_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR53_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*53, `IAAMO the priority of interrupt number 4*53+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR53_PRI_N1_W                                              8
#define CPU_NVIC_IPR53_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR53_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*53, `IAAMO the priority of interrupt number 4*53+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR53_PRI_N0_W                                              8
#define CPU_NVIC_IPR53_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR53_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR54
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*54, `IAAMO the priority of interrupt number 4*54+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR54_PRI_N3_W                                              8
#define CPU_NVIC_IPR54_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR54_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*54, `IAAMO the priority of interrupt number 4*54+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR54_PRI_N2_W                                              8
#define CPU_NVIC_IPR54_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR54_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*54, `IAAMO the priority of interrupt number 4*54+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR54_PRI_N1_W                                              8
#define CPU_NVIC_IPR54_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR54_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*54, `IAAMO the priority of interrupt number 4*54+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR54_PRI_N0_W                                              8
#define CPU_NVIC_IPR54_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR54_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR55
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*55, `IAAMO the priority of interrupt number 4*55+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR55_PRI_N3_W                                              8
#define CPU_NVIC_IPR55_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR55_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*55, `IAAMO the priority of interrupt number 4*55+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR55_PRI_N2_W                                              8
#define CPU_NVIC_IPR55_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR55_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*55, `IAAMO the priority of interrupt number 4*55+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR55_PRI_N1_W                                              8
#define CPU_NVIC_IPR55_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR55_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*55, `IAAMO the priority of interrupt number 4*55+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR55_PRI_N0_W                                              8
#define CPU_NVIC_IPR55_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR55_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR56
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*56, `IAAMO the priority of interrupt number 4*56+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR56_PRI_N3_W                                              8
#define CPU_NVIC_IPR56_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR56_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*56, `IAAMO the priority of interrupt number 4*56+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR56_PRI_N2_W                                              8
#define CPU_NVIC_IPR56_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR56_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*56, `IAAMO the priority of interrupt number 4*56+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR56_PRI_N1_W                                              8
#define CPU_NVIC_IPR56_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR56_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*56, `IAAMO the priority of interrupt number 4*56+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR56_PRI_N0_W                                              8
#define CPU_NVIC_IPR56_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR56_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR57
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*57, `IAAMO the priority of interrupt number 4*57+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR57_PRI_N3_W                                              8
#define CPU_NVIC_IPR57_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR57_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*57, `IAAMO the priority of interrupt number 4*57+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR57_PRI_N2_W                                              8
#define CPU_NVIC_IPR57_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR57_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*57, `IAAMO the priority of interrupt number 4*57+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR57_PRI_N1_W                                              8
#define CPU_NVIC_IPR57_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR57_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*57, `IAAMO the priority of interrupt number 4*57+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR57_PRI_N0_W                                              8
#define CPU_NVIC_IPR57_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR57_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR58
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*58, `IAAMO the priority of interrupt number 4*58+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR58_PRI_N3_W                                              8
#define CPU_NVIC_IPR58_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR58_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*58, `IAAMO the priority of interrupt number 4*58+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR58_PRI_N2_W                                              8
#define CPU_NVIC_IPR58_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR58_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*58, `IAAMO the priority of interrupt number 4*58+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR58_PRI_N1_W                                              8
#define CPU_NVIC_IPR58_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR58_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*58, `IAAMO the priority of interrupt number 4*58+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR58_PRI_N0_W                                              8
#define CPU_NVIC_IPR58_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR58_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR59
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*59, `IAAMO the priority of interrupt number 4*59+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR59_PRI_N3_W                                              8
#define CPU_NVIC_IPR59_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR59_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*59, `IAAMO the priority of interrupt number 4*59+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR59_PRI_N2_W                                              8
#define CPU_NVIC_IPR59_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR59_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*59, `IAAMO the priority of interrupt number 4*59+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR59_PRI_N1_W                                              8
#define CPU_NVIC_IPR59_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR59_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*59, `IAAMO the priority of interrupt number 4*59+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR59_PRI_N0_W                                              8
#define CPU_NVIC_IPR59_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR59_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR60
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*60, `IAAMO the priority of interrupt number 4*60+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR60_PRI_N3_W                                              8
#define CPU_NVIC_IPR60_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR60_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*60, `IAAMO the priority of interrupt number 4*60+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR60_PRI_N2_W                                              8
#define CPU_NVIC_IPR60_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR60_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*60, `IAAMO the priority of interrupt number 4*60+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR60_PRI_N1_W                                              8
#define CPU_NVIC_IPR60_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR60_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*60, `IAAMO the priority of interrupt number 4*60+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR60_PRI_N0_W                                              8
#define CPU_NVIC_IPR60_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR60_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR61
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*61, `IAAMO the priority of interrupt number 4*61+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR61_PRI_N3_W                                              8
#define CPU_NVIC_IPR61_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR61_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*61, `IAAMO the priority of interrupt number 4*61+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR61_PRI_N2_W                                              8
#define CPU_NVIC_IPR61_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR61_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*61, `IAAMO the priority of interrupt number 4*61+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR61_PRI_N1_W                                              8
#define CPU_NVIC_IPR61_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR61_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*61, `IAAMO the priority of interrupt number 4*61+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR61_PRI_N0_W                                              8
#define CPU_NVIC_IPR61_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR61_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR62
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*62, `IAAMO the priority of interrupt number 4*62+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR62_PRI_N3_W                                              8
#define CPU_NVIC_IPR62_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR62_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*62, `IAAMO the priority of interrupt number 4*62+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR62_PRI_N2_W                                              8
#define CPU_NVIC_IPR62_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR62_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*62, `IAAMO the priority of interrupt number 4*62+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR62_PRI_N1_W                                              8
#define CPU_NVIC_IPR62_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR62_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*62, `IAAMO the priority of interrupt number 4*62+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR62_PRI_N0_W                                              8
#define CPU_NVIC_IPR62_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR62_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR63
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*63, `IAAMO the priority of interrupt number 4*63+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR63_PRI_N3_W                                              8
#define CPU_NVIC_IPR63_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR63_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*63, `IAAMO the priority of interrupt number 4*63+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR63_PRI_N2_W                                              8
#define CPU_NVIC_IPR63_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR63_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*63, `IAAMO the priority of interrupt number 4*63+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR63_PRI_N1_W                                              8
#define CPU_NVIC_IPR63_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR63_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*63, `IAAMO the priority of interrupt number 4*63+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR63_PRI_N0_W                                              8
#define CPU_NVIC_IPR63_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR63_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR64
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*64, `IAAMO the priority of interrupt number 4*64+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR64_PRI_N3_W                                              8
#define CPU_NVIC_IPR64_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR64_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*64, `IAAMO the priority of interrupt number 4*64+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR64_PRI_N2_W                                              8
#define CPU_NVIC_IPR64_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR64_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*64, `IAAMO the priority of interrupt number 4*64+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR64_PRI_N1_W                                              8
#define CPU_NVIC_IPR64_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR64_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*64, `IAAMO the priority of interrupt number 4*64+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR64_PRI_N0_W                                              8
#define CPU_NVIC_IPR64_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR64_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR65
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*65, `IAAMO the priority of interrupt number 4*65+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR65_PRI_N3_W                                              8
#define CPU_NVIC_IPR65_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR65_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*65, `IAAMO the priority of interrupt number 4*65+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR65_PRI_N2_W                                              8
#define CPU_NVIC_IPR65_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR65_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*65, `IAAMO the priority of interrupt number 4*65+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR65_PRI_N1_W                                              8
#define CPU_NVIC_IPR65_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR65_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*65, `IAAMO the priority of interrupt number 4*65+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR65_PRI_N0_W                                              8
#define CPU_NVIC_IPR65_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR65_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR66
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*66, `IAAMO the priority of interrupt number 4*66+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR66_PRI_N3_W                                              8
#define CPU_NVIC_IPR66_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR66_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*66, `IAAMO the priority of interrupt number 4*66+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR66_PRI_N2_W                                              8
#define CPU_NVIC_IPR66_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR66_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*66, `IAAMO the priority of interrupt number 4*66+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR66_PRI_N1_W                                              8
#define CPU_NVIC_IPR66_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR66_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*66, `IAAMO the priority of interrupt number 4*66+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR66_PRI_N0_W                                              8
#define CPU_NVIC_IPR66_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR66_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR67
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*67, `IAAMO the priority of interrupt number 4*67+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR67_PRI_N3_W                                              8
#define CPU_NVIC_IPR67_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR67_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*67, `IAAMO the priority of interrupt number 4*67+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR67_PRI_N2_W                                              8
#define CPU_NVIC_IPR67_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR67_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*67, `IAAMO the priority of interrupt number 4*67+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR67_PRI_N1_W                                              8
#define CPU_NVIC_IPR67_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR67_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*67, `IAAMO the priority of interrupt number 4*67+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR67_PRI_N0_W                                              8
#define CPU_NVIC_IPR67_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR67_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR68
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*68, `IAAMO the priority of interrupt number 4*68+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR68_PRI_N3_W                                              8
#define CPU_NVIC_IPR68_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR68_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*68, `IAAMO the priority of interrupt number 4*68+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR68_PRI_N2_W                                              8
#define CPU_NVIC_IPR68_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR68_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*68, `IAAMO the priority of interrupt number 4*68+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR68_PRI_N1_W                                              8
#define CPU_NVIC_IPR68_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR68_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*68, `IAAMO the priority of interrupt number 4*68+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR68_PRI_N0_W                                              8
#define CPU_NVIC_IPR68_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR68_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR69
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*69, `IAAMO the priority of interrupt number 4*69+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR69_PRI_N3_W                                              8
#define CPU_NVIC_IPR69_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR69_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*69, `IAAMO the priority of interrupt number 4*69+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR69_PRI_N2_W                                              8
#define CPU_NVIC_IPR69_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR69_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*69, `IAAMO the priority of interrupt number 4*69+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR69_PRI_N1_W                                              8
#define CPU_NVIC_IPR69_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR69_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*69, `IAAMO the priority of interrupt number 4*69+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR69_PRI_N0_W                                              8
#define CPU_NVIC_IPR69_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR69_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR70
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*70, `IAAMO the priority of interrupt number 4*70+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR70_PRI_N3_W                                              8
#define CPU_NVIC_IPR70_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR70_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*70, `IAAMO the priority of interrupt number 4*70+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR70_PRI_N2_W                                              8
#define CPU_NVIC_IPR70_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR70_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*70, `IAAMO the priority of interrupt number 4*70+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR70_PRI_N1_W                                              8
#define CPU_NVIC_IPR70_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR70_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*70, `IAAMO the priority of interrupt number 4*70+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR70_PRI_N0_W                                              8
#define CPU_NVIC_IPR70_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR70_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR71
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*71, `IAAMO the priority of interrupt number 4*71+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR71_PRI_N3_W                                              8
#define CPU_NVIC_IPR71_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR71_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*71, `IAAMO the priority of interrupt number 4*71+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR71_PRI_N2_W                                              8
#define CPU_NVIC_IPR71_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR71_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*71, `IAAMO the priority of interrupt number 4*71+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR71_PRI_N1_W                                              8
#define CPU_NVIC_IPR71_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR71_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*71, `IAAMO the priority of interrupt number 4*71+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR71_PRI_N0_W                                              8
#define CPU_NVIC_IPR71_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR71_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR72
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*72, `IAAMO the priority of interrupt number 4*72+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR72_PRI_N3_W                                              8
#define CPU_NVIC_IPR72_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR72_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*72, `IAAMO the priority of interrupt number 4*72+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR72_PRI_N2_W                                              8
#define CPU_NVIC_IPR72_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR72_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*72, `IAAMO the priority of interrupt number 4*72+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR72_PRI_N1_W                                              8
#define CPU_NVIC_IPR72_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR72_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*72, `IAAMO the priority of interrupt number 4*72+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR72_PRI_N0_W                                              8
#define CPU_NVIC_IPR72_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR72_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR73
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*73, `IAAMO the priority of interrupt number 4*73+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR73_PRI_N3_W                                              8
#define CPU_NVIC_IPR73_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR73_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*73, `IAAMO the priority of interrupt number 4*73+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR73_PRI_N2_W                                              8
#define CPU_NVIC_IPR73_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR73_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*73, `IAAMO the priority of interrupt number 4*73+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR73_PRI_N1_W                                              8
#define CPU_NVIC_IPR73_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR73_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*73, `IAAMO the priority of interrupt number 4*73+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR73_PRI_N0_W                                              8
#define CPU_NVIC_IPR73_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR73_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR74
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*74, `IAAMO the priority of interrupt number 4*74+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR74_PRI_N3_W                                              8
#define CPU_NVIC_IPR74_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR74_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*74, `IAAMO the priority of interrupt number 4*74+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR74_PRI_N2_W                                              8
#define CPU_NVIC_IPR74_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR74_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*74, `IAAMO the priority of interrupt number 4*74+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR74_PRI_N1_W                                              8
#define CPU_NVIC_IPR74_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR74_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*74, `IAAMO the priority of interrupt number 4*74+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR74_PRI_N0_W                                              8
#define CPU_NVIC_IPR74_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR74_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR75
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*75, `IAAMO the priority of interrupt number 4*75+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR75_PRI_N3_W                                              8
#define CPU_NVIC_IPR75_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR75_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*75, `IAAMO the priority of interrupt number 4*75+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR75_PRI_N2_W                                              8
#define CPU_NVIC_IPR75_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR75_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*75, `IAAMO the priority of interrupt number 4*75+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR75_PRI_N1_W                                              8
#define CPU_NVIC_IPR75_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR75_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*75, `IAAMO the priority of interrupt number 4*75+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR75_PRI_N0_W                                              8
#define CPU_NVIC_IPR75_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR75_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR76
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*76, `IAAMO the priority of interrupt number 4*76+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR76_PRI_N3_W                                              8
#define CPU_NVIC_IPR76_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR76_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*76, `IAAMO the priority of interrupt number 4*76+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR76_PRI_N2_W                                              8
#define CPU_NVIC_IPR76_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR76_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*76, `IAAMO the priority of interrupt number 4*76+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR76_PRI_N1_W                                              8
#define CPU_NVIC_IPR76_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR76_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*76, `IAAMO the priority of interrupt number 4*76+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR76_PRI_N0_W                                              8
#define CPU_NVIC_IPR76_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR76_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR77
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*77, `IAAMO the priority of interrupt number 4*77+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR77_PRI_N3_W                                              8
#define CPU_NVIC_IPR77_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR77_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*77, `IAAMO the priority of interrupt number 4*77+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR77_PRI_N2_W                                              8
#define CPU_NVIC_IPR77_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR77_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*77, `IAAMO the priority of interrupt number 4*77+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR77_PRI_N1_W                                              8
#define CPU_NVIC_IPR77_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR77_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*77, `IAAMO the priority of interrupt number 4*77+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR77_PRI_N0_W                                              8
#define CPU_NVIC_IPR77_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR77_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR78
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*78, `IAAMO the priority of interrupt number 4*78+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR78_PRI_N3_W                                              8
#define CPU_NVIC_IPR78_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR78_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*78, `IAAMO the priority of interrupt number 4*78+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR78_PRI_N2_W                                              8
#define CPU_NVIC_IPR78_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR78_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*78, `IAAMO the priority of interrupt number 4*78+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR78_PRI_N1_W                                              8
#define CPU_NVIC_IPR78_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR78_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*78, `IAAMO the priority of interrupt number 4*78+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR78_PRI_N0_W                                              8
#define CPU_NVIC_IPR78_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR78_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR79
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*79, `IAAMO the priority of interrupt number 4*79+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR79_PRI_N3_W                                              8
#define CPU_NVIC_IPR79_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR79_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*79, `IAAMO the priority of interrupt number 4*79+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR79_PRI_N2_W                                              8
#define CPU_NVIC_IPR79_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR79_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*79, `IAAMO the priority of interrupt number 4*79+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR79_PRI_N1_W                                              8
#define CPU_NVIC_IPR79_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR79_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*79, `IAAMO the priority of interrupt number 4*79+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR79_PRI_N0_W                                              8
#define CPU_NVIC_IPR79_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR79_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR80
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*80, `IAAMO the priority of interrupt number 4*80+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR80_PRI_N3_W                                              8
#define CPU_NVIC_IPR80_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR80_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*80, `IAAMO the priority of interrupt number 4*80+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR80_PRI_N2_W                                              8
#define CPU_NVIC_IPR80_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR80_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*80, `IAAMO the priority of interrupt number 4*80+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR80_PRI_N1_W                                              8
#define CPU_NVIC_IPR80_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR80_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*80, `IAAMO the priority of interrupt number 4*80+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR80_PRI_N0_W                                              8
#define CPU_NVIC_IPR80_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR80_PRI_N0_S                                              0

//*****************************************************************************
//
// Register: CPU_NVIC_O_IPR81
//
//*****************************************************************************
// Field: [31:24] PRI_N3
//
// For register NVIC_IPR*81, `IAAMO the priority of interrupt number 4*81+3, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR81_PRI_N3_W                                              8
#define CPU_NVIC_IPR81_PRI_N3_M                                     0xFF000000
#define CPU_NVIC_IPR81_PRI_N3_S                                             24

// Field: [23:16] PRI_N2
//
// For register NVIC_IPR*81, `IAAMO the priority of interrupt number 4*81+2, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR81_PRI_N2_W                                              8
#define CPU_NVIC_IPR81_PRI_N2_M                                     0x00FF0000
#define CPU_NVIC_IPR81_PRI_N2_S                                             16

// Field:  [15:8] PRI_N1
//
// For register NVIC_IPR*81, `IAAMO the priority of interrupt number 4*81+1, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR81_PRI_N1_W                                              8
#define CPU_NVIC_IPR81_PRI_N1_M                                     0x0000FF00
#define CPU_NVIC_IPR81_PRI_N1_S                                              8

// Field:   [7:0] PRI_N0
//
// For register NVIC_IPR*81, `IAAMO the priority of interrupt number 4*81+0, or
// is RES0 if the PE does not implement this interrupt
#define CPU_NVIC_IPR81_PRI_N0_W                                              8
#define CPU_NVIC_IPR81_PRI_N0_M                                     0x000000FF
#define CPU_NVIC_IPR81_PRI_N0_S                                              0


#endif // __CPU_NVIC__
