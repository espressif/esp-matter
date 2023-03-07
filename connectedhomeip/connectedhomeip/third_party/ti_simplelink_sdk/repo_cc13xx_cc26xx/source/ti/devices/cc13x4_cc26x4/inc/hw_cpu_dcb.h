/******************************************************************************
*  Filename:       hw_cpu_dcb_h
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

#ifndef __HW_CPU_DCB_H__
#define __HW_CPU_DCB_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_DCB component
//
//*****************************************************************************
// Controls halting debug
#define CPU_DCB_O_DHCSR                                             0x00000010

// With the DCRDR, provides debug access to the general-purpose registers,
// special-purpose registers, and the FP extension registers. A write to the
// DCRSR specifies the register to transfer, whether the transfer is a read or
// write, and starts the transfer
#define CPU_DCB_O_DCRSR                                             0x00000014

// With the DCRSR, provides debug access to the general-purpose registers,
// special-purpose registers, and the FP Extension registers. If the Main
// Extension is implemented, it can also be used for message passing between an
// external debugger and a debug agent running on the PE
#define CPU_DCB_O_DCRDR                                             0x00000018

// Manages vector catch behavior and DebugMonitor handling when debugging
#define CPU_DCB_O_DEMCR                                             0x0000001C

// This register allows the external authentication interface to be
#define CPU_DCB_O_DAUTHCTRL                                         0x00000024

// Provides control and status information for Secure debug
#define CPU_DCB_O_DSCSR                                             0x00000028

//*****************************************************************************
//
// Register: CPU_DCB_O_DHCSR
//
//*****************************************************************************
// Field:    [26] S_RESTART_ST
//
// Indicates the PE has processed a request to clear DHCSR.C_HALT to 0. That
// is, either a write to DHCSR that clears DHCSR.C_HALT from 1 to 0, or an
// External Restart Request
#define CPU_DCB_DHCSR_S_RESTART_ST                                  0x04000000
#define CPU_DCB_DHCSR_S_RESTART_ST_BITN                                     26
#define CPU_DCB_DHCSR_S_RESTART_ST_M                                0x04000000
#define CPU_DCB_DHCSR_S_RESTART_ST_S                                        26

// Field:    [25] S_RESET_ST
//
// Indicates whether the PE has been reset since the last read of the DHCSR
#define CPU_DCB_DHCSR_S_RESET_ST                                    0x02000000
#define CPU_DCB_DHCSR_S_RESET_ST_BITN                                       25
#define CPU_DCB_DHCSR_S_RESET_ST_M                                  0x02000000
#define CPU_DCB_DHCSR_S_RESET_ST_S                                          25

// Field:    [24] S_RETIRE_ST
//
// Set to 1 every time the PE retires one of more instructions
#define CPU_DCB_DHCSR_S_RETIRE_ST                                   0x01000000
#define CPU_DCB_DHCSR_S_RETIRE_ST_BITN                                      24
#define CPU_DCB_DHCSR_S_RETIRE_ST_M                                 0x01000000
#define CPU_DCB_DHCSR_S_RETIRE_ST_S                                         24

// Field:    [20] S_SDE
//
// Indicates whether Secure invasive debug is allowed
#define CPU_DCB_DHCSR_S_SDE                                         0x00100000
#define CPU_DCB_DHCSR_S_SDE_BITN                                            20
#define CPU_DCB_DHCSR_S_SDE_M                                       0x00100000
#define CPU_DCB_DHCSR_S_SDE_S                                               20

// Field:    [19] S_LOCKUP
//
// Indicates whether the PE is in Lockup state
#define CPU_DCB_DHCSR_S_LOCKUP                                      0x00080000
#define CPU_DCB_DHCSR_S_LOCKUP_BITN                                         19
#define CPU_DCB_DHCSR_S_LOCKUP_M                                    0x00080000
#define CPU_DCB_DHCSR_S_LOCKUP_S                                            19

// Field:    [18] S_SLEEP
//
// Indicates whether the PE is sleeping
#define CPU_DCB_DHCSR_S_SLEEP                                       0x00040000
#define CPU_DCB_DHCSR_S_SLEEP_BITN                                          18
#define CPU_DCB_DHCSR_S_SLEEP_M                                     0x00040000
#define CPU_DCB_DHCSR_S_SLEEP_S                                             18

// Field:    [17] S_HALT
//
// Indicates whether the PE is in Debug state
#define CPU_DCB_DHCSR_S_HALT                                        0x00020000
#define CPU_DCB_DHCSR_S_HALT_BITN                                           17
#define CPU_DCB_DHCSR_S_HALT_M                                      0x00020000
#define CPU_DCB_DHCSR_S_HALT_S                                              17

// Field: [31:16] DBGKEY
//
// A debugger must write 0xA05F to this field to enable write access to the
// remaining bits, otherwise the PE ignores the write access
#define CPU_DCB_DHCSR_DBGKEY_W                                              16
#define CPU_DCB_DHCSR_DBGKEY_M                                      0xFFFF0000
#define CPU_DCB_DHCSR_DBGKEY_S                                              16

// Field:     [5] C_SNAPSTALL
//
// Allow imprecise entry to Debug state
#define CPU_DCB_DHCSR_C_SNAPSTALL                                   0x00000020
#define CPU_DCB_DHCSR_C_SNAPSTALL_BITN                                       5
#define CPU_DCB_DHCSR_C_SNAPSTALL_M                                 0x00000020
#define CPU_DCB_DHCSR_C_SNAPSTALL_S                                          5

// Field:     [3] C_MASKINTS
//
// When debug is enabled, the debugger can write to this bit to mask PendSV,
// SysTick and external configurable interrupts
#define CPU_DCB_DHCSR_C_MASKINTS                                    0x00000008
#define CPU_DCB_DHCSR_C_MASKINTS_BITN                                        3
#define CPU_DCB_DHCSR_C_MASKINTS_M                                  0x00000008
#define CPU_DCB_DHCSR_C_MASKINTS_S                                           3

// Field:     [2] C_STEP
//
// Enable single instruction step
#define CPU_DCB_DHCSR_C_STEP                                        0x00000004
#define CPU_DCB_DHCSR_C_STEP_BITN                                            2
#define CPU_DCB_DHCSR_C_STEP_M                                      0x00000004
#define CPU_DCB_DHCSR_C_STEP_S                                               2

// Field:     [1] C_HALT
//
// PE enter Debug state halt request
#define CPU_DCB_DHCSR_C_HALT                                        0x00000002
#define CPU_DCB_DHCSR_C_HALT_BITN                                            1
#define CPU_DCB_DHCSR_C_HALT_M                                      0x00000002
#define CPU_DCB_DHCSR_C_HALT_S                                               1

// Field:     [0] C_DEBUGEN
//
// Enable Halting debug
#define CPU_DCB_DHCSR_C_DEBUGEN                                     0x00000001
#define CPU_DCB_DHCSR_C_DEBUGEN_BITN                                         0
#define CPU_DCB_DHCSR_C_DEBUGEN_M                                   0x00000001
#define CPU_DCB_DHCSR_C_DEBUGEN_S                                            0

//*****************************************************************************
//
// Register: CPU_DCB_O_DCRSR
//
//*****************************************************************************
// Field:    [16] REGWnR
//
// Specifies the access type for the transfer
#define CPU_DCB_DCRSR_REGWNR                                        0x00010000
#define CPU_DCB_DCRSR_REGWNR_BITN                                           16
#define CPU_DCB_DCRSR_REGWNR_M                                      0x00010000
#define CPU_DCB_DCRSR_REGWNR_S                                              16

// Field:   [6:0] REGSEL
//
// Specifies the general-purpose register, special-purpose register, or FP
// register to transfer
#define CPU_DCB_DCRSR_REGSEL_W                                               7
#define CPU_DCB_DCRSR_REGSEL_M                                      0x0000007F
#define CPU_DCB_DCRSR_REGSEL_S                                               0

//*****************************************************************************
//
// Register: CPU_DCB_O_DCRDR
//
//*****************************************************************************
// Field:  [31:0] DBGTMP
//
// Provides debug access for reading and writing the general-purpose registers,
// special-purpose registers, and Floating-point Extension registers
#define CPU_DCB_DCRDR_DBGTMP_W                                              32
#define CPU_DCB_DCRDR_DBGTMP_M                                      0xFFFFFFFF
#define CPU_DCB_DCRDR_DBGTMP_S                                               0

//*****************************************************************************
//
// Register: CPU_DCB_O_DEMCR
//
//*****************************************************************************
// Field:    [24] TRCENA
//
// Global enable for all DWT and ITM features
#define CPU_DCB_DEMCR_TRCENA                                        0x01000000
#define CPU_DCB_DEMCR_TRCENA_BITN                                           24
#define CPU_DCB_DEMCR_TRCENA_M                                      0x01000000
#define CPU_DCB_DEMCR_TRCENA_S                                              24

// Field:    [20] SDME
//
// Indicates whether the DebugMonitor targets the Secure or the Non-secure
// state and whether debug events are allowed in Secure state
#define CPU_DCB_DEMCR_SDME                                          0x00100000
#define CPU_DCB_DEMCR_SDME_BITN                                             20
#define CPU_DCB_DEMCR_SDME_M                                        0x00100000
#define CPU_DCB_DEMCR_SDME_S                                                20

// Field:    [19] MON_REQ
//
// DebugMonitor semaphore bit
#define CPU_DCB_DEMCR_MON_REQ                                       0x00080000
#define CPU_DCB_DEMCR_MON_REQ_BITN                                          19
#define CPU_DCB_DEMCR_MON_REQ_M                                     0x00080000
#define CPU_DCB_DEMCR_MON_REQ_S                                             19

// Field:    [18] MON_STEP
//
// Enable DebugMonitor stepping
#define CPU_DCB_DEMCR_MON_STEP                                      0x00040000
#define CPU_DCB_DEMCR_MON_STEP_BITN                                         18
#define CPU_DCB_DEMCR_MON_STEP_M                                    0x00040000
#define CPU_DCB_DEMCR_MON_STEP_S                                            18

// Field:    [17] MON_PEND
//
// Sets or clears the pending state of the DebugMonitor exception
#define CPU_DCB_DEMCR_MON_PEND                                      0x00020000
#define CPU_DCB_DEMCR_MON_PEND_BITN                                         17
#define CPU_DCB_DEMCR_MON_PEND_M                                    0x00020000
#define CPU_DCB_DEMCR_MON_PEND_S                                            17

// Field:    [16] MON_EN
//
// Enable the DebugMonitor exception
#define CPU_DCB_DEMCR_MON_EN                                        0x00010000
#define CPU_DCB_DEMCR_MON_EN_BITN                                           16
#define CPU_DCB_DEMCR_MON_EN_M                                      0x00010000
#define CPU_DCB_DEMCR_MON_EN_S                                              16

// Field:    [11] VC_SFERR
//
// SecureFault exception halting debug vector catch enable
#define CPU_DCB_DEMCR_VC_SFERR                                      0x00000800
#define CPU_DCB_DEMCR_VC_SFERR_BITN                                         11
#define CPU_DCB_DEMCR_VC_SFERR_M                                    0x00000800
#define CPU_DCB_DEMCR_VC_SFERR_S                                            11

// Field:    [10] VC_HARDERR
//
// HardFault exception halting debug vector catch enable
#define CPU_DCB_DEMCR_VC_HARDERR                                    0x00000400
#define CPU_DCB_DEMCR_VC_HARDERR_BITN                                       10
#define CPU_DCB_DEMCR_VC_HARDERR_M                                  0x00000400
#define CPU_DCB_DEMCR_VC_HARDERR_S                                          10

// Field:     [9] VC_INTERR
//
// Enable halting debug vector catch for faults during exception entry and
// return
#define CPU_DCB_DEMCR_VC_INTERR                                     0x00000200
#define CPU_DCB_DEMCR_VC_INTERR_BITN                                         9
#define CPU_DCB_DEMCR_VC_INTERR_M                                   0x00000200
#define CPU_DCB_DEMCR_VC_INTERR_S                                            9

// Field:     [8] VC_BUSERR
//
// BusFault exception halting debug vector catch enable
#define CPU_DCB_DEMCR_VC_BUSERR                                     0x00000100
#define CPU_DCB_DEMCR_VC_BUSERR_BITN                                         8
#define CPU_DCB_DEMCR_VC_BUSERR_M                                   0x00000100
#define CPU_DCB_DEMCR_VC_BUSERR_S                                            8

// Field:     [7] VC_STATERR
//
// Enable halting debug trap on a UsageFault exception caused by a state
// information error, for example an Undefined Instruction exception
#define CPU_DCB_DEMCR_VC_STATERR                                    0x00000080
#define CPU_DCB_DEMCR_VC_STATERR_BITN                                        7
#define CPU_DCB_DEMCR_VC_STATERR_M                                  0x00000080
#define CPU_DCB_DEMCR_VC_STATERR_S                                           7

// Field:     [6] VC_CHKERR
//
// Enable halting debug trap on a UsageFault exception caused by a checking
// error, for example an alignment check error
#define CPU_DCB_DEMCR_VC_CHKERR                                     0x00000040
#define CPU_DCB_DEMCR_VC_CHKERR_BITN                                         6
#define CPU_DCB_DEMCR_VC_CHKERR_M                                   0x00000040
#define CPU_DCB_DEMCR_VC_CHKERR_S                                            6

// Field:     [5] VC_NOCPERR
//
// Enable halting debug trap on a UsageFault caused by an access to a
// coprocessor
#define CPU_DCB_DEMCR_VC_NOCPERR                                    0x00000020
#define CPU_DCB_DEMCR_VC_NOCPERR_BITN                                        5
#define CPU_DCB_DEMCR_VC_NOCPERR_M                                  0x00000020
#define CPU_DCB_DEMCR_VC_NOCPERR_S                                           5

// Field:     [4] VC_MMERR
//
// Enable halting debug trap on a MemManage exception
#define CPU_DCB_DEMCR_VC_MMERR                                      0x00000010
#define CPU_DCB_DEMCR_VC_MMERR_BITN                                          4
#define CPU_DCB_DEMCR_VC_MMERR_M                                    0x00000010
#define CPU_DCB_DEMCR_VC_MMERR_S                                             4

// Field:     [0] VC_CORERESET
//
// Enable Reset Vector Catch. This causes a warm reset to halt a running system
#define CPU_DCB_DEMCR_VC_CORERESET                                  0x00000001
#define CPU_DCB_DEMCR_VC_CORERESET_BITN                                      0
#define CPU_DCB_DEMCR_VC_CORERESET_M                                0x00000001
#define CPU_DCB_DEMCR_VC_CORERESET_S                                         0

//*****************************************************************************
//
// Register: CPU_DCB_O_DAUTHCTRL
//
//*****************************************************************************
// Field:     [3] INTSPNIDEN
//
// Internal Secure non-invasive debug enable. Overrides the external Secure
// non-invasive debug authentication interface
#define CPU_DCB_DAUTHCTRL_INTSPNIDEN                                0x00000008
#define CPU_DCB_DAUTHCTRL_INTSPNIDEN_BITN                                    3
#define CPU_DCB_DAUTHCTRL_INTSPNIDEN_M                              0x00000008
#define CPU_DCB_DAUTHCTRL_INTSPNIDEN_S                                       3

// Field:     [2] SPNIDENSEL
//
// Secure non-invasive debug enable select. Selects between DAUTHCTRL and the
// external authentication interface for control of Secure non-invasive debug
#define CPU_DCB_DAUTHCTRL_SPNIDENSEL                                0x00000004
#define CPU_DCB_DAUTHCTRL_SPNIDENSEL_BITN                                    2
#define CPU_DCB_DAUTHCTRL_SPNIDENSEL_M                              0x00000004
#define CPU_DCB_DAUTHCTRL_SPNIDENSEL_S                                       2

// Field:     [1] INTSPIDEN
//
// Internal Secure invasive debug enable. Overrides the external Secure
// invasive debug authentication Interfaces.
#define CPU_DCB_DAUTHCTRL_INTSPIDEN                                 0x00000002
#define CPU_DCB_DAUTHCTRL_INTSPIDEN_BITN                                     1
#define CPU_DCB_DAUTHCTRL_INTSPIDEN_M                               0x00000002
#define CPU_DCB_DAUTHCTRL_INTSPIDEN_S                                        1

// Field:     [0] SPIDENSEL
//
// Secure invasive debug enable select. Selects between DAUTHCTRL and the
// external authentication interface for control of Secure invasive debug.
#define CPU_DCB_DAUTHCTRL_SPIDENSEL                                 0x00000001
#define CPU_DCB_DAUTHCTRL_SPIDENSEL_BITN                                     0
#define CPU_DCB_DAUTHCTRL_SPIDENSEL_M                               0x00000001
#define CPU_DCB_DAUTHCTRL_SPIDENSEL_S                                        0

//*****************************************************************************
//
// Register: CPU_DCB_O_DSCSR
//
//*****************************************************************************
// Field:    [17] CDSKEY
//
// Writes to the CDS bit are ignored unless CDSKEY is concurrently written to
// zero
#define CPU_DCB_DSCSR_CDSKEY                                        0x00020000
#define CPU_DCB_DSCSR_CDSKEY_BITN                                           17
#define CPU_DCB_DSCSR_CDSKEY_M                                      0x00020000
#define CPU_DCB_DSCSR_CDSKEY_S                                              17

// Field:    [16] CDS
//
// This field indicates the current Security state of the processor
#define CPU_DCB_DSCSR_CDS                                           0x00010000
#define CPU_DCB_DSCSR_CDS_BITN                                              16
#define CPU_DCB_DSCSR_CDS_M                                         0x00010000
#define CPU_DCB_DSCSR_CDS_S                                                 16

// Field:     [1] SBRSEL
//
// If SBRSELEN is 1 this bit selects whether the Non-secure or the Secure
// version of the memory-mapped Banked registers are accessible to the debugger
#define CPU_DCB_DSCSR_SBRSEL                                        0x00000002
#define CPU_DCB_DSCSR_SBRSEL_BITN                                            1
#define CPU_DCB_DSCSR_SBRSEL_M                                      0x00000002
#define CPU_DCB_DSCSR_SBRSEL_S                                               1

// Field:     [0] SBRSELEN
//
// Controls whether the SBRSEL field or the current Security state of the
// processor selects which version of the memory-mapped Banked registers are
// accessed to the debugger
#define CPU_DCB_DSCSR_SBRSELEN                                      0x00000001
#define CPU_DCB_DSCSR_SBRSELEN_BITN                                          0
#define CPU_DCB_DSCSR_SBRSELEN_M                                    0x00000001
#define CPU_DCB_DSCSR_SBRSELEN_S                                             0


#endif // __CPU_DCB__
