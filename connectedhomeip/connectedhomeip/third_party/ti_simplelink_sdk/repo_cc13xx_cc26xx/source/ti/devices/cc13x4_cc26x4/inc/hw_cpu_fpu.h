/******************************************************************************
*  Filename:       hw_cpu_fpu_h
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

#ifndef __HW_CPU_FPU_H__
#define __HW_CPU_FPU_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_FPU component
//
//*****************************************************************************
// Holds control data for the Floating-point extension
#define CPU_FPU_O_FPCCR                                             0x00000004

// Holds the location of the unpopulated floating-point register space
// allocated on an exception stack frame
#define CPU_FPU_O_FPCAR                                             0x00000008

// Holds the default values for the floating-point status control data that the
// PE assigns to the FPSCR when it creates a new floating-point context
#define CPU_FPU_O_FPDSCR                                            0x0000000C

// Describes the features provided by the Floating-point Extension
#define CPU_FPU_O_MVFR0                                             0x00000010

// Describes the features provided by the Floating-point Extension
#define CPU_FPU_O_MVFR1                                             0x00000014

// Describes the features provided by the Floating-point Extension
#define CPU_FPU_O_MVFR2                                             0x00000018

//*****************************************************************************
//
// Register: CPU_FPU_O_FPCCR
//
//*****************************************************************************
// Field:    [31] ASPEN
//
// When this bit is set to 1, execution of a floating-point instruction sets
// the CONTROL.FPCA bit to 1
#define CPU_FPU_FPCCR_ASPEN                                         0x80000000
#define CPU_FPU_FPCCR_ASPEN_BITN                                            31
#define CPU_FPU_FPCCR_ASPEN_M                                       0x80000000
#define CPU_FPU_FPCCR_ASPEN_S                                               31

// Field:    [30] LSPEN
//
// Enables lazy context save of floating-point state
#define CPU_FPU_FPCCR_LSPEN                                         0x40000000
#define CPU_FPU_FPCCR_LSPEN_BITN                                            30
#define CPU_FPU_FPCCR_LSPEN_M                                       0x40000000
#define CPU_FPU_FPCCR_LSPEN_S                                               30

// Field:    [29] LSPENS
//
// This bit controls whether the LSPEN bit is writeable from the Non-secure
// state
#define CPU_FPU_FPCCR_LSPENS                                        0x20000000
#define CPU_FPU_FPCCR_LSPENS_BITN                                           29
#define CPU_FPU_FPCCR_LSPENS_M                                      0x20000000
#define CPU_FPU_FPCCR_LSPENS_S                                              29

// Field:    [28] CLRONRET
//
// Clear floating-point caller saved registers on exception return
#define CPU_FPU_FPCCR_CLRONRET                                      0x10000000
#define CPU_FPU_FPCCR_CLRONRET_BITN                                         28
#define CPU_FPU_FPCCR_CLRONRET_M                                    0x10000000
#define CPU_FPU_FPCCR_CLRONRET_S                                            28

// Field:    [27] CLRONRETS
//
// This bit controls whether the CLRONRET bit is writeable from the Non-secure
// state
#define CPU_FPU_FPCCR_CLRONRETS                                     0x08000000
#define CPU_FPU_FPCCR_CLRONRETS_BITN                                        27
#define CPU_FPU_FPCCR_CLRONRETS_M                                   0x08000000
#define CPU_FPU_FPCCR_CLRONRETS_S                                           27

// Field:    [26] TS
//
// Treat floating-point registers as Secure enable
#define CPU_FPU_FPCCR_TS                                            0x04000000
#define CPU_FPU_FPCCR_TS_BITN                                               26
#define CPU_FPU_FPCCR_TS_M                                          0x04000000
#define CPU_FPU_FPCCR_TS_S                                                  26

// Field:    [10] UFRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the UsageFault exception to
// pending
#define CPU_FPU_FPCCR_UFRDY                                         0x00000400
#define CPU_FPU_FPCCR_UFRDY_BITN                                            10
#define CPU_FPU_FPCCR_UFRDY_M                                       0x00000400
#define CPU_FPU_FPCCR_UFRDY_S                                               10

// Field:     [9] SPLIMVIOL
//
// This bit is banked between the Security states and indicates whether the
// floating-point context violates the stack pointer limit that was active when
// lazy state preservation was activated. SPLIMVIOL modifies the lazy
// floating-point state preservation behavior
#define CPU_FPU_FPCCR_SPLIMVIOL                                     0x00000200
#define CPU_FPU_FPCCR_SPLIMVIOL_BITN                                         9
#define CPU_FPU_FPCCR_SPLIMVIOL_M                                   0x00000200
#define CPU_FPU_FPCCR_SPLIMVIOL_S                                            9

// Field:     [8] MONRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the DebugMonitor exception to
// pending
#define CPU_FPU_FPCCR_MONRDY                                        0x00000100
#define CPU_FPU_FPCCR_MONRDY_BITN                                            8
#define CPU_FPU_FPCCR_MONRDY_M                                      0x00000100
#define CPU_FPU_FPCCR_MONRDY_S                                               8

// Field:     [7] SFRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the SecureFault exception to
// pending. This bit is only present in the Secure version of the register, and
// behaves as RAZ/WI when accessed from the Non-secure state
#define CPU_FPU_FPCCR_SFRDY                                         0x00000080
#define CPU_FPU_FPCCR_SFRDY_BITN                                             7
#define CPU_FPU_FPCCR_SFRDY_M                                       0x00000080
#define CPU_FPU_FPCCR_SFRDY_S                                                7

// Field:     [6] BFRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the BusFault exception to pending
#define CPU_FPU_FPCCR_BFRDY                                         0x00000040
#define CPU_FPU_FPCCR_BFRDY_BITN                                             6
#define CPU_FPU_FPCCR_BFRDY_M                                       0x00000040
#define CPU_FPU_FPCCR_BFRDY_S                                                6

// Field:     [5] MMRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the MemManage exception to
// pending
#define CPU_FPU_FPCCR_MMRDY                                         0x00000020
#define CPU_FPU_FPCCR_MMRDY_BITN                                             5
#define CPU_FPU_FPCCR_MMRDY_M                                       0x00000020
#define CPU_FPU_FPCCR_MMRDY_S                                                5

// Field:     [4] HFRDY
//
// Indicates whether the software executing when the PE allocated the
// floating-point stack frame was able to set the HardFault exception to
// pending
#define CPU_FPU_FPCCR_HFRDY                                         0x00000010
#define CPU_FPU_FPCCR_HFRDY_BITN                                             4
#define CPU_FPU_FPCCR_HFRDY_M                                       0x00000010
#define CPU_FPU_FPCCR_HFRDY_S                                                4

// Field:     [3] THREAD
//
// Indicates the PE mode when it allocated the floating-point stack frame
#define CPU_FPU_FPCCR_THREAD                                        0x00000008
#define CPU_FPU_FPCCR_THREAD_BITN                                            3
#define CPU_FPU_FPCCR_THREAD_M                                      0x00000008
#define CPU_FPU_FPCCR_THREAD_S                                               3

// Field:     [2] S
//
// Security status of the floating-point context. This bit is only present in
// the Secure version of the register, and behaves as RAZ/WI when accessed from
// the Non-secure state. This bit is updated whenever lazy state preservation
// is activated, or when a floating-point instruction is executed
#define CPU_FPU_FPCCR_S                                             0x00000004
#define CPU_FPU_FPCCR_S_BITN                                                 2
#define CPU_FPU_FPCCR_S_M                                           0x00000004
#define CPU_FPU_FPCCR_S_S                                                    2

// Field:     [1] USER
//
// Indicates the privilege level of the software executing when the PE
// allocated the floating-point stack frame
#define CPU_FPU_FPCCR_USER                                          0x00000002
#define CPU_FPU_FPCCR_USER_BITN                                              1
#define CPU_FPU_FPCCR_USER_M                                        0x00000002
#define CPU_FPU_FPCCR_USER_S                                                 1

// Field:     [0] LSPACT
//
// Indicates whether lazy preservation of the floating-point state is active
#define CPU_FPU_FPCCR_LSPACT                                        0x00000001
#define CPU_FPU_FPCCR_LSPACT_BITN                                            0
#define CPU_FPU_FPCCR_LSPACT_M                                      0x00000001
#define CPU_FPU_FPCCR_LSPACT_S                                               0

//*****************************************************************************
//
// Register: CPU_FPU_O_FPCAR
//
//*****************************************************************************
// Field:  [31:3] ADDRESS
//
// The location of the unpopulated floating-point register space allocated on
// an exception stack frame
#define CPU_FPU_FPCAR_ADDRESS_W                                             29
#define CPU_FPU_FPCAR_ADDRESS_M                                     0xFFFFFFF8
#define CPU_FPU_FPCAR_ADDRESS_S                                              3

//*****************************************************************************
//
// Register: CPU_FPU_O_FPDSCR
//
//*****************************************************************************
// Field:    [26] AHP
//
// Default value for FPSCR.AHP
#define CPU_FPU_FPDSCR_AHP                                          0x04000000
#define CPU_FPU_FPDSCR_AHP_BITN                                             26
#define CPU_FPU_FPDSCR_AHP_M                                        0x04000000
#define CPU_FPU_FPDSCR_AHP_S                                                26

// Field:    [25] DN
//
// Default value for FPSCR.DN
#define CPU_FPU_FPDSCR_DN                                           0x02000000
#define CPU_FPU_FPDSCR_DN_BITN                                              25
#define CPU_FPU_FPDSCR_DN_M                                         0x02000000
#define CPU_FPU_FPDSCR_DN_S                                                 25

// Field:    [24] FZ
//
// Default value for FPSCR.FZ
#define CPU_FPU_FPDSCR_FZ                                           0x01000000
#define CPU_FPU_FPDSCR_FZ_BITN                                              24
#define CPU_FPU_FPDSCR_FZ_M                                         0x01000000
#define CPU_FPU_FPDSCR_FZ_S                                                 24

// Field: [23:22] RMode
//
// Default value for FPSCR.RMode
#define CPU_FPU_FPDSCR_RMODE_W                                               2
#define CPU_FPU_FPDSCR_RMODE_M                                      0x00C00000
#define CPU_FPU_FPDSCR_RMODE_S                                              22

//*****************************************************************************
//
// Register: CPU_FPU_O_MVFR0
//
//*****************************************************************************
// Field: [31:28] FPRound
//
// Indicates the rounding modes supported by the FP Extension
#define CPU_FPU_MVFR0_FPROUND_W                                              4
#define CPU_FPU_MVFR0_FPROUND_M                                     0xF0000000
#define CPU_FPU_MVFR0_FPROUND_S                                             28

// Field: [23:20] FPSqrt
//
// Indicates the support for FP square root operations
#define CPU_FPU_MVFR0_FPSQRT_W                                               4
#define CPU_FPU_MVFR0_FPSQRT_M                                      0x00F00000
#define CPU_FPU_MVFR0_FPSQRT_S                                              20

// Field: [19:16] FPDivide
//
// Indicates the support for FP divide operations
#define CPU_FPU_MVFR0_FPDIVIDE_W                                             4
#define CPU_FPU_MVFR0_FPDIVIDE_M                                    0x000F0000
#define CPU_FPU_MVFR0_FPDIVIDE_S                                            16

// Field:  [11:8] FPDP
//
// Indicates support for FP double-precision operations
#define CPU_FPU_MVFR0_FPDP_W                                                 4
#define CPU_FPU_MVFR0_FPDP_M                                        0x00000F00
#define CPU_FPU_MVFR0_FPDP_S                                                 8

// Field:   [7:4] FPSP
//
// Indicates support for FP single-precision operations
#define CPU_FPU_MVFR0_FPSP_W                                                 4
#define CPU_FPU_MVFR0_FPSP_M                                        0x000000F0
#define CPU_FPU_MVFR0_FPSP_S                                                 4

// Field:   [3:0] SIMDReg
//
// Indicates size of FP register file
#define CPU_FPU_MVFR0_SIMDREG_W                                              4
#define CPU_FPU_MVFR0_SIMDREG_M                                     0x0000000F
#define CPU_FPU_MVFR0_SIMDREG_S                                              0

//*****************************************************************************
//
// Register: CPU_FPU_O_MVFR1
//
//*****************************************************************************
// Field: [31:28] FMAC
//
// Indicates whether the FP Extension implements the fused multiply accumulate
// instructions
#define CPU_FPU_MVFR1_FMAC_W                                                 4
#define CPU_FPU_MVFR1_FMAC_M                                        0xF0000000
#define CPU_FPU_MVFR1_FMAC_S                                                28

// Field: [27:24] FPHP
//
// Indicates whether the FP Extension implements half-precision FP conversion
// instructions
#define CPU_FPU_MVFR1_FPHP_W                                                 4
#define CPU_FPU_MVFR1_FPHP_M                                        0x0F000000
#define CPU_FPU_MVFR1_FPHP_S                                                24

// Field:   [7:4] FPDNaN
//
// Indicates whether the FP hardware implementation supports NaN propagation
#define CPU_FPU_MVFR1_FPDNAN_W                                               4
#define CPU_FPU_MVFR1_FPDNAN_M                                      0x000000F0
#define CPU_FPU_MVFR1_FPDNAN_S                                               4

// Field:   [3:0] FPFtZ
//
// Indicates whether subnormals are always flushed-to-zero
#define CPU_FPU_MVFR1_FPFTZ_W                                                4
#define CPU_FPU_MVFR1_FPFTZ_M                                       0x0000000F
#define CPU_FPU_MVFR1_FPFTZ_S                                                0

//*****************************************************************************
//
// Register: CPU_FPU_O_MVFR2
//
//*****************************************************************************
// Field:   [7:4] FPMisc
//
// Indicates support for miscellaneous FP features
#define CPU_FPU_MVFR2_FPMISC_W                                               4
#define CPU_FPU_MVFR2_FPMISC_M                                      0x000000F0
#define CPU_FPU_MVFR2_FPMISC_S                                               4


#endif // __CPU_FPU__
