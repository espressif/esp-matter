/******************************************************************************
*  Filename:       hw_cpu_icb_h
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

#ifndef __HW_CPU_ICB_H__
#define __HW_CPU_ICB_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_ICB component
//
//*****************************************************************************
// Provides information about the interrupt controller
#define CPU_ICB_O_ICTR                                              0x00000004

// Provides IMPLEMENTATION DEFINED configuration and control options
#define CPU_ICB_O_ACTLR                                             0x00000008

//*****************************************************************************
//
// Register: CPU_ICB_O_ICTR
//
//*****************************************************************************
// Field:   [3:0] INTLINESNUM
//
// Indicates the number of the highest implemented register in each of the NVIC
// control register sets, or in the case of NVIC_IPR*n, 4xINTLINESNUM
#define CPU_ICB_ICTR_INTLINESNUM_W                                           4
#define CPU_ICB_ICTR_INTLINESNUM_M                                  0x0000000F
#define CPU_ICB_ICTR_INTLINESNUM_S                                           0

//*****************************************************************************
//
// Register: CPU_ICB_O_ACTLR
//
//*****************************************************************************
// Field:    [29] EXTEXCLALL
//
// External Exclusives Allowed with no MPU
#define CPU_ICB_ACTLR_EXTEXCLALL                                    0x20000000
#define CPU_ICB_ACTLR_EXTEXCLALL_BITN                                       29
#define CPU_ICB_ACTLR_EXTEXCLALL_M                                  0x20000000
#define CPU_ICB_ACTLR_EXTEXCLALL_S                                          29

// Field:    [12] DISITMATBFLUSH
//
// Disable ATB Flush
#define CPU_ICB_ACTLR_DISITMATBFLUSH                                0x00001000
#define CPU_ICB_ACTLR_DISITMATBFLUSH_BITN                                   12
#define CPU_ICB_ACTLR_DISITMATBFLUSH_M                              0x00001000
#define CPU_ICB_ACTLR_DISITMATBFLUSH_S                                      12

// Field:    [10] FPEXCODIS
//
// Disable FPU exception outputs
#define CPU_ICB_ACTLR_FPEXCODIS                                     0x00000400
#define CPU_ICB_ACTLR_FPEXCODIS_BITN                                        10
#define CPU_ICB_ACTLR_FPEXCODIS_M                                   0x00000400
#define CPU_ICB_ACTLR_FPEXCODIS_S                                           10

// Field:     [9] DISOOFP
//
// Disable out-of-order FP instruction completion
#define CPU_ICB_ACTLR_DISOOFP                                       0x00000200
#define CPU_ICB_ACTLR_DISOOFP_BITN                                           9
#define CPU_ICB_ACTLR_DISOOFP_M                                     0x00000200
#define CPU_ICB_ACTLR_DISOOFP_S                                              9

// Field:     [2] DISFOLD
//
// Disable dual-issue.
#define CPU_ICB_ACTLR_DISFOLD                                       0x00000004
#define CPU_ICB_ACTLR_DISFOLD_BITN                                           2
#define CPU_ICB_ACTLR_DISFOLD_M                                     0x00000004
#define CPU_ICB_ACTLR_DISFOLD_S                                              2

// Field:     [0] DISMCYCINT
//
// Disable dual-issue.
#define CPU_ICB_ACTLR_DISMCYCINT                                    0x00000001
#define CPU_ICB_ACTLR_DISMCYCINT_BITN                                        0
#define CPU_ICB_ACTLR_DISMCYCINT_M                                  0x00000001
#define CPU_ICB_ACTLR_DISMCYCINT_S                                           0


#endif // __CPU_ICB__
