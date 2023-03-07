/******************************************************************************
*  Filename:       hw_cpu_dwt_h
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

#ifndef __HW_CPU_DWT_H__
#define __HW_CPU_DWT_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_DWT component
//
//*****************************************************************************
// Provides configuration and status information for the DWT unit, and used to
// control features of the unit
#define CPU_DWT_O_CTRL                                              0x00000000

// Shows or sets the value of the processor cycle counter, CYCCNT
#define CPU_DWT_O_CYCCNT                                            0x00000004

// CPI Count Register
#define CPU_DWT_O_CPICNT                                            0x00000008

// Counts the total cycles spent in exception processing
#define CPU_DWT_O_EXCCNT                                            0x0000000C

// Sleep Count Register
#define CPU_DWT_O_SLEEPCNT                                          0x00000010

// Increments on the additional cycles required to execute all load or store
// instructions
#define CPU_DWT_O_LSUCNT                                            0x00000014

// Increments on the additional cycles required to execute all load or store
// instructions
#define CPU_DWT_O_FOLDCNT                                           0x00000018

// Program Counter Sample Register
#define CPU_DWT_O_PCSR                                              0x0000001C

// Provides a reference value for use by watchpoint comparator 0
#define CPU_DWT_O_COMP0                                             0x00000020

// Controls the operation of watchpoint comparator 0
#define CPU_DWT_O_FUNCTION0                                         0x00000028

// Provides a reference value for use by watchpoint comparator 1
#define CPU_DWT_O_COMP1                                             0x00000030

// Controls the operation of watchpoint comparator 1
#define CPU_DWT_O_FUNCTION1                                         0x00000038

// Provides a reference value for use by watchpoint comparator 2
#define CPU_DWT_O_COMP2                                             0x00000040

// Controls the operation of watchpoint comparator 2
#define CPU_DWT_O_FUNCTION2                                         0x00000048

// Provides a reference value for use by watchpoint comparator 3
#define CPU_DWT_O_COMP3                                             0x00000050

// Controls the operation of watchpoint comparator 3
#define CPU_DWT_O_FUNCTION3                                         0x00000058

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_DEVARCH                                           0x00000FBC

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_DEVTYPE                                           0x00000FCC

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR4                                             0x00000FD0

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR5                                             0x00000FD4

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR6                                             0x00000FD8

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR7                                             0x00000FDC

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR0                                             0x00000FE0

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR1                                             0x00000FE4

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR2                                             0x00000FE8

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_PIDR3                                             0x00000FEC

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_CIDR0                                             0x00000FF0

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_CIDR1                                             0x00000FF4

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_CIDR2                                             0x00000FF8

// Provides CoreSight discovery information for the DWT
#define CPU_DWT_O_CIDR3                                             0x00000FFC

//*****************************************************************************
//
// Register: CPU_DWT_O_CTRL
//
//*****************************************************************************
// Field: [31:28] NUMCOMP
//
// Number of DWT comparators implemented
#define CPU_DWT_CTRL_NUMCOMP_W                                               4
#define CPU_DWT_CTRL_NUMCOMP_M                                      0xF0000000
#define CPU_DWT_CTRL_NUMCOMP_S                                              28

// Field:    [27] NOTRCPKT
//
// Indicates whether the implementation does not support trace
#define CPU_DWT_CTRL_NOTRCPKT                                       0x08000000
#define CPU_DWT_CTRL_NOTRCPKT_BITN                                          27
#define CPU_DWT_CTRL_NOTRCPKT_M                                     0x08000000
#define CPU_DWT_CTRL_NOTRCPKT_S                                             27

// Field:    [26] NOEXTTRIG
//
// Reserved, RAZ
#define CPU_DWT_CTRL_NOEXTTRIG                                      0x04000000
#define CPU_DWT_CTRL_NOEXTTRIG_BITN                                         26
#define CPU_DWT_CTRL_NOEXTTRIG_M                                    0x04000000
#define CPU_DWT_CTRL_NOEXTTRIG_S                                            26

// Field:    [25] NOCYCCNT
//
// Indicates whether the implementation does not include a cycle counter
#define CPU_DWT_CTRL_NOCYCCNT                                       0x02000000
#define CPU_DWT_CTRL_NOCYCCNT_BITN                                          25
#define CPU_DWT_CTRL_NOCYCCNT_M                                     0x02000000
#define CPU_DWT_CTRL_NOCYCCNT_S                                             25

// Field:    [24] NOPRFCNT
//
// Indicates whether the implementation does not include the profiling counters
#define CPU_DWT_CTRL_NOPRFCNT                                       0x01000000
#define CPU_DWT_CTRL_NOPRFCNT_BITN                                          24
#define CPU_DWT_CTRL_NOPRFCNT_M                                     0x01000000
#define CPU_DWT_CTRL_NOPRFCNT_S                                             24

// Field:    [23] CYCDISS
//
// Controls whether the cycle counter is disabled in Secure state
#define CPU_DWT_CTRL_CYCDISS                                        0x00800000
#define CPU_DWT_CTRL_CYCDISS_BITN                                           23
#define CPU_DWT_CTRL_CYCDISS_M                                      0x00800000
#define CPU_DWT_CTRL_CYCDISS_S                                              23

// Field:    [22] CYCEVTENA
//
// Enables Event Counter packet generation on POSTCNT underflow
#define CPU_DWT_CTRL_CYCEVTENA                                      0x00400000
#define CPU_DWT_CTRL_CYCEVTENA_BITN                                         22
#define CPU_DWT_CTRL_CYCEVTENA_M                                    0x00400000
#define CPU_DWT_CTRL_CYCEVTENA_S                                            22

// Field:    [21] FOLDEVTENA
//
// Enables DWT_FOLDCNT counter
#define CPU_DWT_CTRL_FOLDEVTENA                                     0x00200000
#define CPU_DWT_CTRL_FOLDEVTENA_BITN                                        21
#define CPU_DWT_CTRL_FOLDEVTENA_M                                   0x00200000
#define CPU_DWT_CTRL_FOLDEVTENA_S                                           21

// Field:    [20] LSUEVTENA
//
// Enables DWT_LSUCNT counter
#define CPU_DWT_CTRL_LSUEVTENA                                      0x00100000
#define CPU_DWT_CTRL_LSUEVTENA_BITN                                         20
#define CPU_DWT_CTRL_LSUEVTENA_M                                    0x00100000
#define CPU_DWT_CTRL_LSUEVTENA_S                                            20

// Field:    [19] SLEEPEVTENA
//
// Enable DWT_SLEEPCNT counter
#define CPU_DWT_CTRL_SLEEPEVTENA                                    0x00080000
#define CPU_DWT_CTRL_SLEEPEVTENA_BITN                                       19
#define CPU_DWT_CTRL_SLEEPEVTENA_M                                  0x00080000
#define CPU_DWT_CTRL_SLEEPEVTENA_S                                          19

// Field:    [18] EXCEVTENA
//
// Enables DWT_EXCCNT counter
#define CPU_DWT_CTRL_EXCEVTENA                                      0x00040000
#define CPU_DWT_CTRL_EXCEVTENA_BITN                                         18
#define CPU_DWT_CTRL_EXCEVTENA_M                                    0x00040000
#define CPU_DWT_CTRL_EXCEVTENA_S                                            18

// Field:    [17] CPIEVTENA
//
// Enables DWT_CPICNT counter
#define CPU_DWT_CTRL_CPIEVTENA                                      0x00020000
#define CPU_DWT_CTRL_CPIEVTENA_BITN                                         17
#define CPU_DWT_CTRL_CPIEVTENA_M                                    0x00020000
#define CPU_DWT_CTRL_CPIEVTENA_S                                            17

// Field:    [16] EXTTRCENA
//
// Enables generation of Exception Trace packets
#define CPU_DWT_CTRL_EXTTRCENA                                      0x00010000
#define CPU_DWT_CTRL_EXTTRCENA_BITN                                         16
#define CPU_DWT_CTRL_EXTTRCENA_M                                    0x00010000
#define CPU_DWT_CTRL_EXTTRCENA_S                                            16

// Field:    [12] PCSAMPLENA
//
// Enables use of POSTCNT counter as a timer for Periodic PC Sample packet
// generation
#define CPU_DWT_CTRL_PCSAMPLENA                                     0x00001000
#define CPU_DWT_CTRL_PCSAMPLENA_BITN                                        12
#define CPU_DWT_CTRL_PCSAMPLENA_M                                   0x00001000
#define CPU_DWT_CTRL_PCSAMPLENA_S                                           12

// Field: [11:10] SYNCTAP
//
// Selects the position of the synchronization packet counter tap on the CYCCNT
// counter. This determines the Synchronization packet rate
#define CPU_DWT_CTRL_SYNCTAP_W                                               2
#define CPU_DWT_CTRL_SYNCTAP_M                                      0x00000C00
#define CPU_DWT_CTRL_SYNCTAP_S                                              10

// Field:     [9] CYCTAP
//
// Selects the position of the POSTCNT tap on the CYCCNT counter
#define CPU_DWT_CTRL_CYCTAP                                         0x00000200
#define CPU_DWT_CTRL_CYCTAP_BITN                                             9
#define CPU_DWT_CTRL_CYCTAP_M                                       0x00000200
#define CPU_DWT_CTRL_CYCTAP_S                                                9

// Field:   [8:5] POSTINIT
//
// Initial value for the POSTCNT counter
#define CPU_DWT_CTRL_POSTINIT_W                                              4
#define CPU_DWT_CTRL_POSTINIT_M                                     0x000001E0
#define CPU_DWT_CTRL_POSTINIT_S                                              5

// Field:   [4:1] POSTPRESET
//
// Reload value for the POSTCNT counter
#define CPU_DWT_CTRL_POSTPRESET_W                                            4
#define CPU_DWT_CTRL_POSTPRESET_M                                   0x0000001E
#define CPU_DWT_CTRL_POSTPRESET_S                                            1

// Field:     [0] CYCCNTENA
//
// Enables CYCCNT
#define CPU_DWT_CTRL_CYCCNTENA                                      0x00000001
#define CPU_DWT_CTRL_CYCCNTENA_BITN                                          0
#define CPU_DWT_CTRL_CYCCNTENA_M                                    0x00000001
#define CPU_DWT_CTRL_CYCCNTENA_S                                             0

//*****************************************************************************
//
// Register: CPU_DWT_O_CYCCNT
//
//*****************************************************************************
// Field:  [31:0] CYCCNT
//
// Increments one on each processor clock cycle when DWT_CTRL.CYCCNTENA == 1
// and DEMCR.TRCENA == 1. On overflow, CYCCNT wraps to zero
#define CPU_DWT_CYCCNT_CYCCNT_W                                             32
#define CPU_DWT_CYCCNT_CYCCNT_M                                     0xFFFFFFFF
#define CPU_DWT_CYCCNT_CYCCNT_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_CPICNT
//
//*****************************************************************************
// Field:   [7:0] CPICNT
//
// Counts one on each cycle when all of the following are true:
// - DWT_CTRL.CPIEVTENA == 1 and DEMCR.TRCENA == 1.
// - No instruction is executed.
// - No load-store operation is in progress, see DWT_LSUCNT.
// - No exception-entry or exception-exit operation is in progress, see
// DWT_EXCCNT.
// - The PE is not in a power saving mode, see DWT_SLEEPCNT.
// - Either SecureNoninvasiveDebugAllowed() == TRUE, or the PE is in Non-secure
// state and NoninvasiveDebugAllowed() == TRUE.
#define CPU_DWT_CPICNT_CPICNT_W                                              8
#define CPU_DWT_CPICNT_CPICNT_M                                     0x000000FF
#define CPU_DWT_CPICNT_CPICNT_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_EXCCNT
//
//*****************************************************************************
// Field:   [7:0] EXCCNT
//
// Counts one on each cycle when all of the following are true:
//  - DWT_CTRL.EXCEVTENA == 1 and DEMCR.TRCENA == 1.
//  - No instruction is executed, see DWT_CPICNT.
//  - An exception-entry or exception-exit related operation is in progress.
//  - Either SecureNoninvasiveDebugAllowed() == TRUE, or NS-Req for the
// operation is set to Non-secure and NoninvasiveDebugAllowed() == TRUE.
#define CPU_DWT_EXCCNT_EXCCNT_W                                              8
#define CPU_DWT_EXCCNT_EXCCNT_M                                     0x000000FF
#define CPU_DWT_EXCCNT_EXCCNT_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_SLEEPCNT
//
//*****************************************************************************
// Field:   [7:0] SLEEPCNT
//
// Counts one on each cycle when all of the following are true:
// - DWT_CTRL.SLEEPEVTENA == 1 and DEMCR.TRCENA == 1.
// - No instruction is executed, see DWT_CPICNT.
// - No load-store operation is in progress, see DWT_LSUCNT.
// - No exception-entry or exception-exit operation is in progress, see
// DWT_EXCCNT.
// - The PE is in a power saving mode.
// - Either SecureNoninvasiveDebugAllowed() == TRUE, or the PE is in Non-secure
// state and NoninvasiveDebugAllowed() == TRUE.
#define CPU_DWT_SLEEPCNT_SLEEPCNT_W                                          8
#define CPU_DWT_SLEEPCNT_SLEEPCNT_M                                 0x000000FF
#define CPU_DWT_SLEEPCNT_SLEEPCNT_S                                          0

//*****************************************************************************
//
// Register: CPU_DWT_O_LSUCNT
//
//*****************************************************************************
// Field:   [7:0] LSUCNT
//
// Counts one on each cycle when all of the following are true:
//  - DWT_CTRL.LSUEVTENA == 1 and DEMCR.TRCENA == 1.
//  - No instruction is executed, see DWT_CPICNT.
//  - No exception-entry or exception-exit operation is in progress, see
// DWT_EXCCNT.
//  - A load-store operation is in progress.
//  - Either SecureNoninvasiveDebugAllowed() == TRUE, or NS-Req for the
// operation is set to Non-secure and NoninvasiveDebugAllowed() == TRUE.
#define CPU_DWT_LSUCNT_LSUCNT_W                                              8
#define CPU_DWT_LSUCNT_LSUCNT_M                                     0x000000FF
#define CPU_DWT_LSUCNT_LSUCNT_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_FOLDCNT
//
//*****************************************************************************
// Field:   [7:0] FOLDCNT
//
// Counts on each cycle when all of the following are true:
//  - DWT_CTRL.FOLDEVTENA == 1 and DEMCR.TRCENA == 1.
//  - At least two instructions are executed, see DWT_CPICNT.
//  - Either SecureNoninvasiveDebugAllowed() == TRUE, or the PE is in
// Non-secure state and NoninvasiveDebugAllowed() == TRUE.
//  The counter is incremented by the number of instructions executed, minus
// one
#define CPU_DWT_FOLDCNT_FOLDCNT_W                                            8
#define CPU_DWT_FOLDCNT_FOLDCNT_M                                   0x000000FF
#define CPU_DWT_FOLDCNT_FOLDCNT_S                                            0

//*****************************************************************************
//
// Register: CPU_DWT_O_PCSR
//
//*****************************************************************************
// Field:  [31:0] EIASAMPLE
//
// The possible values of this field are:
// 0xFFFFFFFF
//   One of the following is true:
//   - The PE is halted in Debug state.
//   - The Security Extension is implemented, the sampled instruction was
// executed in Secure state, and SecureNoninvasiveDebugAllowed() == FALSE.
//   - NoninvasiveDebugAllowed() == FALSE.
//   - DEMCR.TRCENA == 0.
//   - The address of a recently-executed instruction is not available.
// Not 0xFFFFFFFF
//   Instruction address of a recently executed instruction. Bit [0] of the
// sample instruction address is 0.
#define CPU_DWT_PCSR_EIASAMPLE_W                                            32
#define CPU_DWT_PCSR_EIASAMPLE_M                                    0xFFFFFFFF
#define CPU_DWT_PCSR_EIASAMPLE_S                                             0

//*****************************************************************************
//
// Register: CPU_DWT_O_COMP0
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_FUNCTION0
//
//*****************************************************************************
// Field: [31:27] ID
//
// Identifies the capabilities for MATCH for comparator *n
#define CPU_DWT_FUNCTION0_ID_W                                               5
#define CPU_DWT_FUNCTION0_ID_M                                      0xF8000000
#define CPU_DWT_FUNCTION0_ID_S                                              27

// Field:    [24] MATCHED
//
// Set to 1 when the comparator matches
#define CPU_DWT_FUNCTION0_MATCHED                                   0x01000000
#define CPU_DWT_FUNCTION0_MATCHED_BITN                                      24
#define CPU_DWT_FUNCTION0_MATCHED_M                                 0x01000000
#define CPU_DWT_FUNCTION0_MATCHED_S                                         24

// Field: [11:10] DATAVSIZE
//
// Defines the size of the object being watched for by Data Value and Data
// Address comparators
#define CPU_DWT_FUNCTION0_DATAVSIZE_W                                        2
#define CPU_DWT_FUNCTION0_DATAVSIZE_M                               0x00000C00
#define CPU_DWT_FUNCTION0_DATAVSIZE_S                                       10

// Field:   [5:4] ACTION
//
// Defines the action on a match. This field is ignored and the comparator
// generates no actions if it is disabled by MATCH
#define CPU_DWT_FUNCTION0_ACTION_W                                           2
#define CPU_DWT_FUNCTION0_ACTION_M                                  0x00000030
#define CPU_DWT_FUNCTION0_ACTION_S                                           4

// Field:   [3:0] MATCH
//
// Controls the type of match generated by this comparator
#define CPU_DWT_FUNCTION0_MATCH_W                                            4
#define CPU_DWT_FUNCTION0_MATCH_M                                   0x0000000F
#define CPU_DWT_FUNCTION0_MATCH_S                                            0

//*****************************************************************************
//
// Register: CPU_DWT_O_COMP1
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_FUNCTION1
//
//*****************************************************************************
// Field: [31:27] ID
//
// Identifies the capabilities for MATCH for comparator *n
#define CPU_DWT_FUNCTION1_ID_W                                               5
#define CPU_DWT_FUNCTION1_ID_M                                      0xF8000000
#define CPU_DWT_FUNCTION1_ID_S                                              27

// Field:    [24] MATCHED
//
// Set to 1 when the comparator matches
#define CPU_DWT_FUNCTION1_MATCHED                                   0x01000000
#define CPU_DWT_FUNCTION1_MATCHED_BITN                                      24
#define CPU_DWT_FUNCTION1_MATCHED_M                                 0x01000000
#define CPU_DWT_FUNCTION1_MATCHED_S                                         24

// Field: [11:10] DATAVSIZE
//
// Defines the size of the object being watched for by Data Value and Data
// Address comparators
#define CPU_DWT_FUNCTION1_DATAVSIZE_W                                        2
#define CPU_DWT_FUNCTION1_DATAVSIZE_M                               0x00000C00
#define CPU_DWT_FUNCTION1_DATAVSIZE_S                                       10

// Field:   [5:4] ACTION
//
// Defines the action on a match. This field is ignored and the comparator
// generates no actions if it is disabled by MATCH
#define CPU_DWT_FUNCTION1_ACTION_W                                           2
#define CPU_DWT_FUNCTION1_ACTION_M                                  0x00000030
#define CPU_DWT_FUNCTION1_ACTION_S                                           4

// Field:   [3:0] MATCH
//
// Controls the type of match generated by this comparator
#define CPU_DWT_FUNCTION1_MATCH_W                                            4
#define CPU_DWT_FUNCTION1_MATCH_M                                   0x0000000F
#define CPU_DWT_FUNCTION1_MATCH_S                                            0

//*****************************************************************************
//
// Register: CPU_DWT_O_COMP2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_FUNCTION2
//
//*****************************************************************************
// Field: [31:27] ID
//
// Identifies the capabilities for MATCH for comparator *n
#define CPU_DWT_FUNCTION2_ID_W                                               5
#define CPU_DWT_FUNCTION2_ID_M                                      0xF8000000
#define CPU_DWT_FUNCTION2_ID_S                                              27

// Field:    [24] MATCHED
//
// Set to 1 when the comparator matches
#define CPU_DWT_FUNCTION2_MATCHED                                   0x01000000
#define CPU_DWT_FUNCTION2_MATCHED_BITN                                      24
#define CPU_DWT_FUNCTION2_MATCHED_M                                 0x01000000
#define CPU_DWT_FUNCTION2_MATCHED_S                                         24

// Field: [11:10] DATAVSIZE
//
// Defines the size of the object being watched for by Data Value and Data
// Address comparators
#define CPU_DWT_FUNCTION2_DATAVSIZE_W                                        2
#define CPU_DWT_FUNCTION2_DATAVSIZE_M                               0x00000C00
#define CPU_DWT_FUNCTION2_DATAVSIZE_S                                       10

// Field:   [5:4] ACTION
//
// Defines the action on a match. This field is ignored and the comparator
// generates no actions if it is disabled by MATCH
#define CPU_DWT_FUNCTION2_ACTION_W                                           2
#define CPU_DWT_FUNCTION2_ACTION_M                                  0x00000030
#define CPU_DWT_FUNCTION2_ACTION_S                                           4

// Field:   [3:0] MATCH
//
// Controls the type of match generated by this comparator
#define CPU_DWT_FUNCTION2_MATCH_W                                            4
#define CPU_DWT_FUNCTION2_MATCH_M                                   0x0000000F
#define CPU_DWT_FUNCTION2_MATCH_S                                            0

//*****************************************************************************
//
// Register: CPU_DWT_O_COMP3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_FUNCTION3
//
//*****************************************************************************
// Field: [31:27] ID
//
// Identifies the capabilities for MATCH for comparator *n
#define CPU_DWT_FUNCTION3_ID_W                                               5
#define CPU_DWT_FUNCTION3_ID_M                                      0xF8000000
#define CPU_DWT_FUNCTION3_ID_S                                              27

// Field:    [24] MATCHED
//
// Set to 1 when the comparator matches
#define CPU_DWT_FUNCTION3_MATCHED                                   0x01000000
#define CPU_DWT_FUNCTION3_MATCHED_BITN                                      24
#define CPU_DWT_FUNCTION3_MATCHED_M                                 0x01000000
#define CPU_DWT_FUNCTION3_MATCHED_S                                         24

// Field: [11:10] DATAVSIZE
//
// Defines the size of the object being watched for by Data Value and Data
// Address comparators
#define CPU_DWT_FUNCTION3_DATAVSIZE_W                                        2
#define CPU_DWT_FUNCTION3_DATAVSIZE_M                               0x00000C00
#define CPU_DWT_FUNCTION3_DATAVSIZE_S                                       10

// Field:   [5:4] ACTION
//
// Defines the action on a match. This field is ignored and the comparator
// generates no actions if it is disabled by MATCH
#define CPU_DWT_FUNCTION3_ACTION_W                                           2
#define CPU_DWT_FUNCTION3_ACTION_M                                  0x00000030
#define CPU_DWT_FUNCTION3_ACTION_S                                           4

// Field:   [3:0] MATCH
//
// Controls the type of match generated by this comparator
#define CPU_DWT_FUNCTION3_MATCH_W                                            4
#define CPU_DWT_FUNCTION3_MATCH_M                                   0x0000000F
#define CPU_DWT_FUNCTION3_MATCH_S                                            0

//*****************************************************************************
//
// Register: CPU_DWT_O_DEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// Defines the architect of the component. Bits [31:28] are the JEP106
// continuation code (JEP106 bank ID, minus 1) and bits [27:21] are the JEP106
// ID code.
#define CPU_DWT_DEVARCH_ARCHITECT_W                                         11
#define CPU_DWT_DEVARCH_ARCHITECT_M                                 0xFFE00000
#define CPU_DWT_DEVARCH_ARCHITECT_S                                         21

// Field:    [20] PRESENT
//
// Defines that the DEVARCH register is present
#define CPU_DWT_DEVARCH_PRESENT                                     0x00100000
#define CPU_DWT_DEVARCH_PRESENT_BITN                                        20
#define CPU_DWT_DEVARCH_PRESENT_M                                   0x00100000
#define CPU_DWT_DEVARCH_PRESENT_S                                           20

// Field: [19:16] REVISION
//
// Defines the architecture revision of the component
#define CPU_DWT_DEVARCH_REVISION_W                                           4
#define CPU_DWT_DEVARCH_REVISION_M                                  0x000F0000
#define CPU_DWT_DEVARCH_REVISION_S                                          16

// Field: [15:12] ARCHVER
//
// Defines the architecture version of the component
#define CPU_DWT_DEVARCH_ARCHVER_W                                            4
#define CPU_DWT_DEVARCH_ARCHVER_M                                   0x0000F000
#define CPU_DWT_DEVARCH_ARCHVER_S                                           12

// Field:  [11:0] ARCHPART
//
// Defines the architecture of the component
#define CPU_DWT_DEVARCH_ARCHPART_W                                          12
#define CPU_DWT_DEVARCH_ARCHPART_M                                  0x00000FFF
#define CPU_DWT_DEVARCH_ARCHPART_S                                           0

//*****************************************************************************
//
// Register: CPU_DWT_O_DEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// Component sub-type
#define CPU_DWT_DEVTYPE_SUB_W                                                4
#define CPU_DWT_DEVTYPE_SUB_M                                       0x000000F0
#define CPU_DWT_DEVTYPE_SUB_S                                                4

// Field:   [3:0] MAJOR
//
// Component major type
#define CPU_DWT_DEVTYPE_MAJOR_W                                              4
#define CPU_DWT_DEVTYPE_MAJOR_M                                     0x0000000F
#define CPU_DWT_DEVTYPE_MAJOR_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR4_SIZE_W                                                 4
#define CPU_DWT_PIDR4_SIZE_M                                        0x000000F0
#define CPU_DWT_PIDR4_SIZE_S                                                 4

// Field:   [3:0] DES_2
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR4_DES_2_W                                                4
#define CPU_DWT_PIDR4_DES_2_M                                       0x0000000F
#define CPU_DWT_PIDR4_DES_2_S                                                0

//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR0_PART_0_W                                               8
#define CPU_DWT_PIDR0_PART_0_M                                      0x000000FF
#define CPU_DWT_PIDR0_PART_0_S                                               0

//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR1_DES_0_W                                                4
#define CPU_DWT_PIDR1_DES_0_M                                       0x000000F0
#define CPU_DWT_PIDR1_DES_0_S                                                4

// Field:   [3:0] PART_1
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR1_PART_1_W                                               4
#define CPU_DWT_PIDR1_PART_1_M                                      0x0000000F
#define CPU_DWT_PIDR1_PART_1_S                                               0

//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR2_REVISION_W                                             4
#define CPU_DWT_PIDR2_REVISION_M                                    0x000000F0
#define CPU_DWT_PIDR2_REVISION_S                                             4

// Field:     [3] JEDEC
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR2_JEDEC                                         0x00000008
#define CPU_DWT_PIDR2_JEDEC_BITN                                             3
#define CPU_DWT_PIDR2_JEDEC_M                                       0x00000008
#define CPU_DWT_PIDR2_JEDEC_S                                                3

// Field:   [2:0] DES_1
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR2_DES_1_W                                                3
#define CPU_DWT_PIDR2_DES_1_M                                       0x00000007
#define CPU_DWT_PIDR2_DES_1_S                                                0

//*****************************************************************************
//
// Register: CPU_DWT_O_PIDR3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR3_REVAND_W                                               4
#define CPU_DWT_PIDR3_REVAND_M                                      0x000000F0
#define CPU_DWT_PIDR3_REVAND_S                                               4

// Field:   [3:0] CMOD
//
// See CoreSight Architecture Specification
#define CPU_DWT_PIDR3_CMOD_W                                                 4
#define CPU_DWT_PIDR3_CMOD_M                                        0x0000000F
#define CPU_DWT_PIDR3_CMOD_S                                                 0

//*****************************************************************************
//
// Register: CPU_DWT_O_CIDR0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// See CoreSight Architecture Specification
#define CPU_DWT_CIDR0_PRMBL_0_W                                              8
#define CPU_DWT_CIDR0_PRMBL_0_M                                     0x000000FF
#define CPU_DWT_CIDR0_PRMBL_0_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_CIDR1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// See CoreSight Architecture Specification
#define CPU_DWT_CIDR1_CLASS_W                                                4
#define CPU_DWT_CIDR1_CLASS_M                                       0x000000F0
#define CPU_DWT_CIDR1_CLASS_S                                                4

// Field:   [3:0] PRMBL_1
//
// See CoreSight Architecture Specification
#define CPU_DWT_CIDR1_PRMBL_1_W                                              4
#define CPU_DWT_CIDR1_PRMBL_1_M                                     0x0000000F
#define CPU_DWT_CIDR1_PRMBL_1_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_CIDR2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// See CoreSight Architecture Specification
#define CPU_DWT_CIDR2_PRMBL_2_W                                              8
#define CPU_DWT_CIDR2_PRMBL_2_M                                     0x000000FF
#define CPU_DWT_CIDR2_PRMBL_2_S                                              0

//*****************************************************************************
//
// Register: CPU_DWT_O_CIDR3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// See CoreSight Architecture Specification
#define CPU_DWT_CIDR3_PRMBL_3_W                                              8
#define CPU_DWT_CIDR3_PRMBL_3_M                                     0x000000FF
#define CPU_DWT_CIDR3_PRMBL_3_S                                              0


#endif // __CPU_DWT__
