/******************************************************************************
*  Filename:       hw_cpu_etm_h
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

#ifndef __HW_CPU_ETM_H__
#define __HW_CPU_ETM_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_ETM component
//
//*****************************************************************************
// Programming Control Register
#define CPU_ETM_O_TRCPRGCTLR                                        0x00000004

// The TRCSTATR indicates the ETM-Teal status
#define CPU_ETM_O_TRCSTATR                                          0x0000000C

// The TRCCONFIGR sets the basic tracing options for the trace unit
#define CPU_ETM_O_TRCCONFIGR                                        0x00000010

// The TRCEVENTCTL0R controls the tracing of events in the trace stream. The
// events also drive the ETM-Teal external outputs.
#define CPU_ETM_O_TRCEVENTCTL0R                                     0x00000020

// The TRCEVENTCTL1R controls how the events selected by TRCEVENTCTL0R behave
#define CPU_ETM_O_TRCEVENTCTL1R                                     0x00000024

// The TRCSTALLCTLR enables ETM-Teal to stall the processor if the ETM-Teal
// FIFO goes over the programmed level to minimize risk of overflow
#define CPU_ETM_O_TRCSTALLCTLR                                      0x0000002C

// The TRCTSCTLR controls the insertion of global timestamps into the trace
// stream. A timestamp is always inserted into the instruction trace stream
#define CPU_ETM_O_TRCTSCTLR                                         0x00000030

// The TRCSYNCPR specifies the period of trace synchronization of the trace
// streams. TRCSYNCPR defines a number of bytes of trace between requests for
// trace synchronization. This value is always a power of two
#define CPU_ETM_O_TRCSYNCPR                                         0x00000034

// The TRCCCCTLR sets the threshold value for instruction trace cycle counting.
// The threshold represents the minimum interval between cycle count trace
// packets
#define CPU_ETM_O_TRCCCCTLR                                         0x00000038

// The TRCVICTLR controls instruction trace filtering
#define CPU_ETM_O_TRCVICTLR                                         0x00000080

// The TRCCNTRLDVR defines the reload value for the reduced function counter
#define CPU_ETM_O_TRCCNTRLDVR0                                      0x00000140

// The TRCCNTRLDVR defines the reload value for the reduced function counter
#define CPU_ETM_O_TRCCNTRLDVR1                                      0x00000144

// The TRCCNTRLDVR defines the reload value for the reduced function counter
#define CPU_ETM_O_TRCCNTRLDVR2                                      0x00000148

// TRCIDR8
#define CPU_ETM_O_TRCIDR8                                           0x00000180

// TRCIDR9
#define CPU_ETM_O_TRCIDR9                                           0x00000184

// TRCIDR10
#define CPU_ETM_O_TRCIDR10                                          0x00000188

// TRCIDR11
#define CPU_ETM_O_TRCIDR11                                          0x0000018C

// TRCIDR12
#define CPU_ETM_O_TRCIDR12                                          0x00000190

// TRCIDR13
#define CPU_ETM_O_TRCIDR13                                          0x00000194

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC0                                        0x000001C0

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC1                                        0x000001C4

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC2                                        0x000001C8

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC3                                        0x000001CC

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC4                                        0x000001D0

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC5                                        0x000001D4

// The TRCIMSPEC shows the presence of any IMPLEMENTATION SPECIFIC features,
// and enables any features that are provided
#define CPU_ETM_O_TRCIMSPEC6                                        0x000001D8

// TRCIDR0
#define CPU_ETM_O_TRCIDR0                                           0x000001E0

// TRCIDR1
#define CPU_ETM_O_TRCIDR1                                           0x000001E4

// TRCIDR2
#define CPU_ETM_O_TRCIDR2                                           0x000001E8

// TRCIDR3
#define CPU_ETM_O_TRCIDR3                                           0x000001EC

// TRCIDR4
#define CPU_ETM_O_TRCIDR4                                           0x000001F0

// TRCIDR5
#define CPU_ETM_O_TRCIDR5                                           0x000001F4

// TRCIDR6
#define CPU_ETM_O_TRCIDR6                                           0x000001F8

// TRCIDR7
#define CPU_ETM_O_TRCIDR7                                           0x000001FC

// The TRCRSCTLR controls the trace resources
#define CPU_ETM_O_TRCRSCTLR2                                        0x00000208

// The TRCRSCTLR controls the trace resources
#define CPU_ETM_O_TRCRSCTLR3                                        0x0000020C

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR0                                         0x000002A0

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR1                                         0x000002A4

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR2                                         0x000002A8

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR3                                         0x000002AC

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR4                                         0x000002B0

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR5                                         0x000002B4

// Controls the corresponding single-shot comparator resource
#define CPU_ETM_O_TRCSSCSR6                                         0x000002B8

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR0                                       0x000002C0

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR1                                       0x000002C4

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR2                                       0x000002C8

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR3                                       0x000002CC

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR4                                       0x000002D0

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR5                                       0x000002D4

// Selects the PE comparator inputs for Single-shot control
#define CPU_ETM_O_TRCSSPCICR6                                       0x000002D8

// Requests the system to provide power to the trace unit
#define CPU_ETM_O_TRCPDCR                                           0x00000310

// Returns the following information about the trace unit: - OS Lock status.  -
// Core power domain status.  - Power interruption status
#define CPU_ETM_O_TRCPDSR                                           0x00000314

// Trace Intergration ATB Identification Register
#define CPU_ETM_O_TRCITATBIDR                                       0x00000EE4

// Trace Integration Instruction ATB In Register
#define CPU_ETM_O_TRCITIATBINR                                      0x00000EF4

// Trace Integration Instruction ATB Out Register
#define CPU_ETM_O_TRCITIATBOUTR                                     0x00000EFC

// Claim Tag Set Register
#define CPU_ETM_O_TRCCLAIMSET                                       0x00000FA0

// Claim Tag Clear Register
#define CPU_ETM_O_TRCCLAIMCLR                                       0x00000FA4

// Returns the level of tracing that the trace unit can support
#define CPU_ETM_O_TRCAUTHSTATUS                                     0x00000FB8

// TRCDEVARCH
#define CPU_ETM_O_TRCDEVARCH                                        0x00000FBC

// TRCDEVID
#define CPU_ETM_O_TRCDEVID                                          0x00000FC8

// TRCDEVTYPE
#define CPU_ETM_O_TRCDEVTYPE                                        0x00000FCC

// TRCPIDR4
#define CPU_ETM_O_TRCPIDR4                                          0x00000FD0

// TRCPIDR5
#define CPU_ETM_O_TRCPIDR5                                          0x00000FD4

// TRCPIDR6
#define CPU_ETM_O_TRCPIDR6                                          0x00000FD8

// TRCPIDR7
#define CPU_ETM_O_TRCPIDR7                                          0x00000FDC

// TRCPIDR0
#define CPU_ETM_O_TRCPIDR0                                          0x00000FE0

// TRCPIDR1
#define CPU_ETM_O_TRCPIDR1                                          0x00000FE4

// TRCPIDR2
#define CPU_ETM_O_TRCPIDR2                                          0x00000FE8

// TRCPIDR3
#define CPU_ETM_O_TRCPIDR3                                          0x00000FEC

// TRCCIDR0
#define CPU_ETM_O_TRCCIDR0                                          0x00000FF0

// TRCCIDR1
#define CPU_ETM_O_TRCCIDR1                                          0x00000FF4

// TRCCIDR2
#define CPU_ETM_O_TRCCIDR2                                          0x00000FF8

// TRCCIDR3
#define CPU_ETM_O_TRCCIDR3                                          0x00000FFC

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPRGCTLR
//
//*****************************************************************************
// Field:     [0] EN
//
// Trace Unit Enable
#define CPU_ETM_TRCPRGCTLR_EN                                       0x00000001
#define CPU_ETM_TRCPRGCTLR_EN_BITN                                           0
#define CPU_ETM_TRCPRGCTLR_EN_M                                     0x00000001
#define CPU_ETM_TRCPRGCTLR_EN_S                                              0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSTATR
//
//*****************************************************************************
// Field:     [1] PMSTABLE
//
// Indicates whether the ETM-Teal registers are stable and can be read
#define CPU_ETM_TRCSTATR_PMSTABLE                                   0x00000002
#define CPU_ETM_TRCSTATR_PMSTABLE_BITN                                       1
#define CPU_ETM_TRCSTATR_PMSTABLE_M                                 0x00000002
#define CPU_ETM_TRCSTATR_PMSTABLE_S                                          1

// Field:     [0] IDLE
//
// Indicates that the trace unit is inactive
#define CPU_ETM_TRCSTATR_IDLE                                       0x00000001
#define CPU_ETM_TRCSTATR_IDLE_BITN                                           0
#define CPU_ETM_TRCSTATR_IDLE_M                                     0x00000001
#define CPU_ETM_TRCSTATR_IDLE_S                                              0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCONFIGR
//
//*****************************************************************************
// Field:    [17] DV
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_DV                                       0x00020000
#define CPU_ETM_TRCCONFIGR_DV_BITN                                          17
#define CPU_ETM_TRCCONFIGR_DV_M                                     0x00020000
#define CPU_ETM_TRCCONFIGR_DV_S                                             17

// Field:    [16] DA
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_DA                                       0x00010000
#define CPU_ETM_TRCCONFIGR_DA_BITN                                          16
#define CPU_ETM_TRCCONFIGR_DA_M                                     0x00010000
#define CPU_ETM_TRCCONFIGR_DA_S                                             16

// Field: [14:13] QE
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_QE_W                                              2
#define CPU_ETM_TRCCONFIGR_QE_M                                     0x00006000
#define CPU_ETM_TRCCONFIGR_QE_S                                             13

// Field:    [12] RS
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_RS                                       0x00001000
#define CPU_ETM_TRCCONFIGR_RS_BITN                                          12
#define CPU_ETM_TRCCONFIGR_RS_M                                     0x00001000
#define CPU_ETM_TRCCONFIGR_RS_S                                             12

// Field:    [11] TS
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_TS                                       0x00000800
#define CPU_ETM_TRCCONFIGR_TS_BITN                                          11
#define CPU_ETM_TRCCONFIGR_TS_M                                     0x00000800
#define CPU_ETM_TRCCONFIGR_TS_S                                             11

// Field:  [10:8] COND
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_COND_W                                            3
#define CPU_ETM_TRCCONFIGR_COND_M                                   0x00000700
#define CPU_ETM_TRCCONFIGR_COND_S                                            8

// Field:     [7] VMID
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_VMID                                     0x00000080
#define CPU_ETM_TRCCONFIGR_VMID_BITN                                         7
#define CPU_ETM_TRCCONFIGR_VMID_M                                   0x00000080
#define CPU_ETM_TRCCONFIGR_VMID_S                                            7

// Field:     [6] CID
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_CID                                      0x00000040
#define CPU_ETM_TRCCONFIGR_CID_BITN                                          6
#define CPU_ETM_TRCCONFIGR_CID_M                                    0x00000040
#define CPU_ETM_TRCCONFIGR_CID_S                                             6

// Field:     [4] CCI
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_CCI                                      0x00000010
#define CPU_ETM_TRCCONFIGR_CCI_BITN                                          4
#define CPU_ETM_TRCCONFIGR_CCI_M                                    0x00000010
#define CPU_ETM_TRCCONFIGR_CCI_S                                             4

// Field:     [3] BB
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_BB                                       0x00000008
#define CPU_ETM_TRCCONFIGR_BB_BITN                                           3
#define CPU_ETM_TRCCONFIGR_BB_M                                     0x00000008
#define CPU_ETM_TRCCONFIGR_BB_S                                              3

// Field:   [2:1] INSTP0
//
// Reserved, `ImpDefRES0
#define CPU_ETM_TRCCONFIGR_INSTP0_W                                          2
#define CPU_ETM_TRCCONFIGR_INSTP0_M                                 0x00000006
#define CPU_ETM_TRCCONFIGR_INSTP0_S                                          1

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCEVENTCTL0R
//
//*****************************************************************************
// Field:    [15] TYPE1
//
// Selects the resource type for event 1
#define CPU_ETM_TRCEVENTCTL0R_TYPE1                                 0x00008000
#define CPU_ETM_TRCEVENTCTL0R_TYPE1_BITN                                    15
#define CPU_ETM_TRCEVENTCTL0R_TYPE1_M                               0x00008000
#define CPU_ETM_TRCEVENTCTL0R_TYPE1_S                                       15

// Field:  [10:8] SEL1
//
// Selects the resource number, based on the value of TYPE1: When TYPE1 is 0,
// selects a single selected resource from 0-15 defined by SEL1[2:0].  When
// TYPE1 is 1, selects a Boolean combined resource pair from 0-7 defined by
// SEL1[2:0]
#define CPU_ETM_TRCEVENTCTL0R_SEL1_W                                         3
#define CPU_ETM_TRCEVENTCTL0R_SEL1_M                                0x00000700
#define CPU_ETM_TRCEVENTCTL0R_SEL1_S                                         8

// Field:     [7] TYPE0
//
// Selects the resource type for event 0
#define CPU_ETM_TRCEVENTCTL0R_TYPE0                                 0x00000080
#define CPU_ETM_TRCEVENTCTL0R_TYPE0_BITN                                     7
#define CPU_ETM_TRCEVENTCTL0R_TYPE0_M                               0x00000080
#define CPU_ETM_TRCEVENTCTL0R_TYPE0_S                                        7

// Field:   [2:0] SEL0
//
// Selects the resource number, based on the value of TYPE0: When TYPE1 is 0,
// selects a single selected resource from 0-15 defined by SEL0[2:0].  When
// TYPE1 is 1, selects a Boolean combined resource pair from 0-7 defined by
// SEL0[2:0]
#define CPU_ETM_TRCEVENTCTL0R_SEL0_W                                         3
#define CPU_ETM_TRCEVENTCTL0R_SEL0_M                                0x00000007
#define CPU_ETM_TRCEVENTCTL0R_SEL0_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCEVENTCTL1R
//
//*****************************************************************************
// Field:    [12] LPOVERRIDE
//
// Low power state behavior override
#define CPU_ETM_TRCEVENTCTL1R_LPOVERRIDE                            0x00001000
#define CPU_ETM_TRCEVENTCTL1R_LPOVERRIDE_BITN                               12
#define CPU_ETM_TRCEVENTCTL1R_LPOVERRIDE_M                          0x00001000
#define CPU_ETM_TRCEVENTCTL1R_LPOVERRIDE_S                                  12

// Field:    [11] ATB
//
// ATB enabled
#define CPU_ETM_TRCEVENTCTL1R_ATB                                   0x00000800
#define CPU_ETM_TRCEVENTCTL1R_ATB_BITN                                      11
#define CPU_ETM_TRCEVENTCTL1R_ATB_M                                 0x00000800
#define CPU_ETM_TRCEVENTCTL1R_ATB_S                                         11

// Field:     [1] INSTEN1
//
// One bit per event, to enable generation of an event element in the
// instruction trace stream when the selected event occurs
#define CPU_ETM_TRCEVENTCTL1R_INSTEN1                               0x00000002
#define CPU_ETM_TRCEVENTCTL1R_INSTEN1_BITN                                   1
#define CPU_ETM_TRCEVENTCTL1R_INSTEN1_M                             0x00000002
#define CPU_ETM_TRCEVENTCTL1R_INSTEN1_S                                      1

// Field:     [0] INSTEN0
//
// One bit per event, to enable generation of an event element in the
// instruction trace stream when the selected event occurs
#define CPU_ETM_TRCEVENTCTL1R_INSTEN0                               0x00000001
#define CPU_ETM_TRCEVENTCTL1R_INSTEN0_BITN                                   0
#define CPU_ETM_TRCEVENTCTL1R_INSTEN0_M                             0x00000001
#define CPU_ETM_TRCEVENTCTL1R_INSTEN0_S                                      0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSTALLCTLR
//
//*****************************************************************************
// Field:    [10] INSTPRIORITY
//
// Prioritize instruction trace if instruction trace buffer space is less than
// LEVEL
#define CPU_ETM_TRCSTALLCTLR_INSTPRIORITY                           0x00000400
#define CPU_ETM_TRCSTALLCTLR_INSTPRIORITY_BITN                              10
#define CPU_ETM_TRCSTALLCTLR_INSTPRIORITY_M                         0x00000400
#define CPU_ETM_TRCSTALLCTLR_INSTPRIORITY_S                                 10

// Field:     [8] ISTALL
//
// Stall processor based on instruction trace buffer space
#define CPU_ETM_TRCSTALLCTLR_ISTALL                                 0x00000100
#define CPU_ETM_TRCSTALLCTLR_ISTALL_BITN                                     8
#define CPU_ETM_TRCSTALLCTLR_ISTALL_M                               0x00000100
#define CPU_ETM_TRCSTALLCTLR_ISTALL_S                                        8

// Field:   [3:0] LEVEL
//
// Threshold at which stalling becomes active. This provides four levels. This
// level can be varied to optimize the level of invasion caused by stalling,
// balanced against the risk of a FIFO overflow
#define CPU_ETM_TRCSTALLCTLR_LEVEL_W                                         4
#define CPU_ETM_TRCSTALLCTLR_LEVEL_M                                0x0000000F
#define CPU_ETM_TRCSTALLCTLR_LEVEL_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCTSCTLR
//
//*****************************************************************************
// Field:   [7:0] EVENT
//
// An event selector. When the selected event is triggered, the trace unit
// inserts a global timestamp into the trace streams
#define CPU_ETM_TRCTSCTLR_EVENT_W                                            8
#define CPU_ETM_TRCTSCTLR_EVENT_M                                   0x000000FF
#define CPU_ETM_TRCTSCTLR_EVENT_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSYNCPR
//
//*****************************************************************************
// Field:   [4:0] PERIOD
//
// Defines the number of bytes of trace between trace synchronization requests
// as a total of the number of bytes generated by the instruction stream. The
// number of bytes is 2N where N is the value of this field: - A value of zero
// disables these periodic trace synchronization requests, but does not disable
// other trace synchronization requests.  - The minimum value that can be
// programmed, other than zero, is 8, providing a minimum trace synchronization
// period of 256 bytes.  - The maximum value is 20, providing a maximum trace
// synchronization period of 2^20 bytes
#define CPU_ETM_TRCSYNCPR_PERIOD_W                                           5
#define CPU_ETM_TRCSYNCPR_PERIOD_M                                  0x0000001F
#define CPU_ETM_TRCSYNCPR_PERIOD_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCCCTLR
//
//*****************************************************************************
// Field:  [11:0] THRESHOLD
//
// Instruction trace cycle count threshold
#define CPU_ETM_TRCCCCTLR_THRESHOLD_W                                       12
#define CPU_ETM_TRCCCCTLR_THRESHOLD_M                               0x00000FFF
#define CPU_ETM_TRCCCCTLR_THRESHOLD_S                                        0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCVICTLR
//
//*****************************************************************************
// Field:    [19] EXLEVEL_S3
//
// In Secure state, each bit controls whether instruction tracing is enabled
// for the corresponding exception level
#define CPU_ETM_TRCVICTLR_EXLEVEL_S3                                0x00080000
#define CPU_ETM_TRCVICTLR_EXLEVEL_S3_BITN                                   19
#define CPU_ETM_TRCVICTLR_EXLEVEL_S3_M                              0x00080000
#define CPU_ETM_TRCVICTLR_EXLEVEL_S3_S                                      19

// Field:    [16] EXLEVEL_S0
//
// In Secure state, each bit controls whether instruction tracing is enabled
// for the corresponding exception level
#define CPU_ETM_TRCVICTLR_EXLEVEL_S0                                0x00010000
#define CPU_ETM_TRCVICTLR_EXLEVEL_S0_BITN                                   16
#define CPU_ETM_TRCVICTLR_EXLEVEL_S0_M                              0x00010000
#define CPU_ETM_TRCVICTLR_EXLEVEL_S0_S                                      16

// Field:    [11] TRCERR
//
// Selects whether a system error exception must always be traced
#define CPU_ETM_TRCVICTLR_TRCERR                                    0x00000800
#define CPU_ETM_TRCVICTLR_TRCERR_BITN                                       11
#define CPU_ETM_TRCVICTLR_TRCERR_M                                  0x00000800
#define CPU_ETM_TRCVICTLR_TRCERR_S                                          11

// Field:    [10] TRCRESET
//
// Selects whether a reset exception must always be traced
#define CPU_ETM_TRCVICTLR_TRCRESET                                  0x00000400
#define CPU_ETM_TRCVICTLR_TRCRESET_BITN                                     10
#define CPU_ETM_TRCVICTLR_TRCRESET_M                                0x00000400
#define CPU_ETM_TRCVICTLR_TRCRESET_S                                        10

// Field:     [9] SSSTATUS
//
// Indicates the current status of the start/stop logic
#define CPU_ETM_TRCVICTLR_SSSTATUS                                  0x00000200
#define CPU_ETM_TRCVICTLR_SSSTATUS_BITN                                      9
#define CPU_ETM_TRCVICTLR_SSSTATUS_M                                0x00000200
#define CPU_ETM_TRCVICTLR_SSSTATUS_S                                         9

// Field:   [7:0] EVENT
//
// An event selector
#define CPU_ETM_TRCVICTLR_EVENT_W                                            8
#define CPU_ETM_TRCVICTLR_EVENT_M                                   0x000000FF
#define CPU_ETM_TRCVICTLR_EVENT_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCNTRLDVR0
//
//*****************************************************************************
// Field:  [15:0] VALUE
//
// Defines the reload value for the counter. This value is loaded into the
// counter each time the reload event occurs
#define CPU_ETM_TRCCNTRLDVR0_VALUE_W                                        16
#define CPU_ETM_TRCCNTRLDVR0_VALUE_M                                0x0000FFFF
#define CPU_ETM_TRCCNTRLDVR0_VALUE_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCNTRLDVR1
//
//*****************************************************************************
// Field:  [15:0] VALUE
//
// Defines the reload value for the counter. This value is loaded into the
// counter each time the reload event occurs
#define CPU_ETM_TRCCNTRLDVR1_VALUE_W                                        16
#define CPU_ETM_TRCCNTRLDVR1_VALUE_M                                0x0000FFFF
#define CPU_ETM_TRCCNTRLDVR1_VALUE_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCNTRLDVR2
//
//*****************************************************************************
// Field:  [15:0] VALUE
//
// Defines the reload value for the counter. This value is loaded into the
// counter each time the reload event occurs
#define CPU_ETM_TRCCNTRLDVR2_VALUE_W                                        16
#define CPU_ETM_TRCCNTRLDVR2_VALUE_M                                0x0000FFFF
#define CPU_ETM_TRCCNTRLDVR2_VALUE_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR8
//
//*****************************************************************************
// Field:  [31:0] MAXSPEC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR8_MAXSPEC_W                                           32
#define CPU_ETM_TRCIDR8_MAXSPEC_M                                   0xFFFFFFFF
#define CPU_ETM_TRCIDR8_MAXSPEC_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR9
//
//*****************************************************************************
// Field:  [31:0] NUMP0KEY
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR9_NUMP0KEY_W                                          32
#define CPU_ETM_TRCIDR9_NUMP0KEY_M                                  0xFFFFFFFF
#define CPU_ETM_TRCIDR9_NUMP0KEY_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR10
//
//*****************************************************************************
// Field:  [31:0] NUMP1KEY
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR10_NUMP1KEY_W                                         32
#define CPU_ETM_TRCIDR10_NUMP1KEY_M                                 0xFFFFFFFF
#define CPU_ETM_TRCIDR10_NUMP1KEY_S                                          0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR11
//
//*****************************************************************************
// Field:  [31:0] NUMP1SPC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR11_NUMP1SPC_W                                         32
#define CPU_ETM_TRCIDR11_NUMP1SPC_M                                 0xFFFFFFFF
#define CPU_ETM_TRCIDR11_NUMP1SPC_S                                          0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR12
//
//*****************************************************************************
// Field:  [31:0] NUMCONDKEY
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR12_NUMCONDKEY_W                                       32
#define CPU_ETM_TRCIDR12_NUMCONDKEY_M                               0xFFFFFFFF
#define CPU_ETM_TRCIDR12_NUMCONDKEY_S                                        0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR13
//
//*****************************************************************************
// Field:  [31:0] NUMCONDSPC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR13_NUMCONDSPC_W                                       32
#define CPU_ETM_TRCIDR13_NUMCONDSPC_M                               0xFFFFFFFF
#define CPU_ETM_TRCIDR13_NUMCONDSPC_S                                        0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC0
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC0_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC0_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC0_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC1
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC1_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC1_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC1_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC2
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC2_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC2_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC2_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC3
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC3_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC3_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC3_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC4
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC4_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC4_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC4_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC5
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC5_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC5_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC5_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIMSPEC6
//
//*****************************************************************************
// Field:   [3:0] SUPPORT
//
// Set to 0. No IMPLEMENTATION SPECIFIC extensions are supported
#define CPU_ETM_TRCIMSPEC6_SUPPORT_W                                         4
#define CPU_ETM_TRCIMSPEC6_SUPPORT_M                                0x0000000F
#define CPU_ETM_TRCIMSPEC6_SUPPORT_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR0
//
//*****************************************************************************
// Field:    [29] COMMOPT
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_COMMOPT                                     0x20000000
#define CPU_ETM_TRCIDR0_COMMOPT_BITN                                        29
#define CPU_ETM_TRCIDR0_COMMOPT_M                                   0x20000000
#define CPU_ETM_TRCIDR0_COMMOPT_S                                           29

// Field: [28:24] TSSIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TSSIZE_W                                             5
#define CPU_ETM_TRCIDR0_TSSIZE_M                                    0x1F000000
#define CPU_ETM_TRCIDR0_TSSIZE_S                                            24

// Field:    [17] TRCEXDATA
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TRCEXDATA                                   0x00020000
#define CPU_ETM_TRCIDR0_TRCEXDATA_BITN                                      17
#define CPU_ETM_TRCIDR0_TRCEXDATA_M                                 0x00020000
#define CPU_ETM_TRCIDR0_TRCEXDATA_S                                         17

// Field: [16:15] QSUPP
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_QSUPP_W                                              2
#define CPU_ETM_TRCIDR0_QSUPP_M                                     0x00018000
#define CPU_ETM_TRCIDR0_QSUPP_S                                             15

// Field:    [14] QFILT
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_QFILT                                       0x00004000
#define CPU_ETM_TRCIDR0_QFILT_BITN                                          14
#define CPU_ETM_TRCIDR0_QFILT_M                                     0x00004000
#define CPU_ETM_TRCIDR0_QFILT_S                                             14

// Field: [13:12] CONDTYPE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_CONDTYPE_W                                           2
#define CPU_ETM_TRCIDR0_CONDTYPE_M                                  0x00003000
#define CPU_ETM_TRCIDR0_CONDTYPE_S                                          12

// Field: [11:10] NUMEVENT
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_NUMEVENT_W                                           2
#define CPU_ETM_TRCIDR0_NUMEVENT_M                                  0x00000C00
#define CPU_ETM_TRCIDR0_NUMEVENT_S                                          10

// Field:     [9] RETSTACK
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_RETSTACK                                    0x00000200
#define CPU_ETM_TRCIDR0_RETSTACK_BITN                                        9
#define CPU_ETM_TRCIDR0_RETSTACK_M                                  0x00000200
#define CPU_ETM_TRCIDR0_RETSTACK_S                                           9

// Field:     [7] TRCCCI
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TRCCCI                                      0x00000080
#define CPU_ETM_TRCIDR0_TRCCCI_BITN                                          7
#define CPU_ETM_TRCIDR0_TRCCCI_M                                    0x00000080
#define CPU_ETM_TRCIDR0_TRCCCI_S                                             7

// Field:     [6] TRCCOND
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TRCCOND                                     0x00000040
#define CPU_ETM_TRCIDR0_TRCCOND_BITN                                         6
#define CPU_ETM_TRCIDR0_TRCCOND_M                                   0x00000040
#define CPU_ETM_TRCIDR0_TRCCOND_S                                            6

// Field:     [5] TRCBB
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TRCBB                                       0x00000020
#define CPU_ETM_TRCIDR0_TRCBB_BITN                                           5
#define CPU_ETM_TRCIDR0_TRCBB_M                                     0x00000020
#define CPU_ETM_TRCIDR0_TRCBB_S                                              5

// Field:   [4:3] TRCDATA
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_TRCDATA_W                                            2
#define CPU_ETM_TRCIDR0_TRCDATA_M                                   0x00000018
#define CPU_ETM_TRCIDR0_TRCDATA_S                                            3

// Field:   [2:1] INSTP0
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR0_INSTP0_W                                             2
#define CPU_ETM_TRCIDR0_INSTP0_M                                    0x00000006
#define CPU_ETM_TRCIDR0_INSTP0_S                                             1

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR1
//
//*****************************************************************************
// Field: [31:24] DESIGNER
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR1_DESIGNER_W                                           8
#define CPU_ETM_TRCIDR1_DESIGNER_M                                  0xFF000000
#define CPU_ETM_TRCIDR1_DESIGNER_S                                          24

// Field:  [11:8] TRCARCHMAJ
//
// reads as 0b0100
#define CPU_ETM_TRCIDR1_TRCARCHMAJ_W                                         4
#define CPU_ETM_TRCIDR1_TRCARCHMAJ_M                                0x00000F00
#define CPU_ETM_TRCIDR1_TRCARCHMAJ_S                                         8

// Field:   [7:4] TRCARCHMIN
//
// reads as 0b0000
#define CPU_ETM_TRCIDR1_TRCARCHMIN_W                                         4
#define CPU_ETM_TRCIDR1_TRCARCHMIN_M                                0x000000F0
#define CPU_ETM_TRCIDR1_TRCARCHMIN_S                                         4

// Field:   [3:0] REVISION
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR1_REVISION_W                                           4
#define CPU_ETM_TRCIDR1_REVISION_M                                  0x0000000F
#define CPU_ETM_TRCIDR1_REVISION_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR2
//
//*****************************************************************************
// Field: [28:25] CCSIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_CCSIZE_W                                             4
#define CPU_ETM_TRCIDR2_CCSIZE_M                                    0x1E000000
#define CPU_ETM_TRCIDR2_CCSIZE_S                                            25

// Field: [24:20] DVSIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_DVSIZE_W                                             5
#define CPU_ETM_TRCIDR2_DVSIZE_M                                    0x01F00000
#define CPU_ETM_TRCIDR2_DVSIZE_S                                            20

// Field: [19:15] DASIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_DASIZE_W                                             5
#define CPU_ETM_TRCIDR2_DASIZE_M                                    0x000F8000
#define CPU_ETM_TRCIDR2_DASIZE_S                                            15

// Field: [14:10] VMIDSIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_VMIDSIZE_W                                           5
#define CPU_ETM_TRCIDR2_VMIDSIZE_M                                  0x00007C00
#define CPU_ETM_TRCIDR2_VMIDSIZE_S                                          10

// Field:   [9:5] CIDSIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_CIDSIZE_W                                            5
#define CPU_ETM_TRCIDR2_CIDSIZE_M                                   0x000003E0
#define CPU_ETM_TRCIDR2_CIDSIZE_S                                            5

// Field:   [4:0] IASIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR2_IASIZE_W                                             5
#define CPU_ETM_TRCIDR2_IASIZE_M                                    0x0000001F
#define CPU_ETM_TRCIDR2_IASIZE_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR3
//
//*****************************************************************************
// Field:    [31] NOOVERFLOW
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_NOOVERFLOW                                  0x80000000
#define CPU_ETM_TRCIDR3_NOOVERFLOW_BITN                                     31
#define CPU_ETM_TRCIDR3_NOOVERFLOW_M                                0x80000000
#define CPU_ETM_TRCIDR3_NOOVERFLOW_S                                        31

// Field: [30:28] NUMPROC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_NUMPROC_W                                            3
#define CPU_ETM_TRCIDR3_NUMPROC_M                                   0x70000000
#define CPU_ETM_TRCIDR3_NUMPROC_S                                           28

// Field:    [27] SYSSTALL
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_SYSSTALL                                    0x08000000
#define CPU_ETM_TRCIDR3_SYSSTALL_BITN                                       27
#define CPU_ETM_TRCIDR3_SYSSTALL_M                                  0x08000000
#define CPU_ETM_TRCIDR3_SYSSTALL_S                                          27

// Field:    [26] STALLCTL
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_STALLCTL                                    0x04000000
#define CPU_ETM_TRCIDR3_STALLCTL_BITN                                       26
#define CPU_ETM_TRCIDR3_STALLCTL_M                                  0x04000000
#define CPU_ETM_TRCIDR3_STALLCTL_S                                          26

// Field:    [25] SYNCPR
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_SYNCPR                                      0x02000000
#define CPU_ETM_TRCIDR3_SYNCPR_BITN                                         25
#define CPU_ETM_TRCIDR3_SYNCPR_M                                    0x02000000
#define CPU_ETM_TRCIDR3_SYNCPR_S                                            25

// Field:    [24] TRCERR
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_TRCERR                                      0x01000000
#define CPU_ETM_TRCIDR3_TRCERR_BITN                                         24
#define CPU_ETM_TRCIDR3_TRCERR_M                                    0x01000000
#define CPU_ETM_TRCIDR3_TRCERR_S                                            24

// Field: [23:20] EXLEVEL_NS
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_EXLEVEL_NS_W                                         4
#define CPU_ETM_TRCIDR3_EXLEVEL_NS_M                                0x00F00000
#define CPU_ETM_TRCIDR3_EXLEVEL_NS_S                                        20

// Field: [19:16] EXLEVEL_S
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_EXLEVEL_S_W                                          4
#define CPU_ETM_TRCIDR3_EXLEVEL_S_M                                 0x000F0000
#define CPU_ETM_TRCIDR3_EXLEVEL_S_S                                         16

// Field:  [11:0] CCITMIN
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR3_CCITMIN_W                                           12
#define CPU_ETM_TRCIDR3_CCITMIN_M                                   0x00000FFF
#define CPU_ETM_TRCIDR3_CCITMIN_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR4
//
//*****************************************************************************
// Field: [31:28] NUMVMIDC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMVMIDC_W                                           4
#define CPU_ETM_TRCIDR4_NUMVMIDC_M                                  0xF0000000
#define CPU_ETM_TRCIDR4_NUMVMIDC_S                                          28

// Field: [27:24] NUMCIDC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMCIDC_W                                            4
#define CPU_ETM_TRCIDR4_NUMCIDC_M                                   0x0F000000
#define CPU_ETM_TRCIDR4_NUMCIDC_S                                           24

// Field: [23:20] NUMSSCC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMSSCC_W                                            4
#define CPU_ETM_TRCIDR4_NUMSSCC_M                                   0x00F00000
#define CPU_ETM_TRCIDR4_NUMSSCC_S                                           20

// Field: [19:16] NUMRSPAIR
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMRSPAIR_W                                          4
#define CPU_ETM_TRCIDR4_NUMRSPAIR_M                                 0x000F0000
#define CPU_ETM_TRCIDR4_NUMRSPAIR_S                                         16

// Field: [15:12] NUMPC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMPC_W                                              4
#define CPU_ETM_TRCIDR4_NUMPC_M                                     0x0000F000
#define CPU_ETM_TRCIDR4_NUMPC_S                                             12

// Field:     [8] SUPPDAC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_SUPPDAC                                     0x00000100
#define CPU_ETM_TRCIDR4_SUPPDAC_BITN                                         8
#define CPU_ETM_TRCIDR4_SUPPDAC_M                                   0x00000100
#define CPU_ETM_TRCIDR4_SUPPDAC_S                                            8

// Field:   [7:4] NUMDVC
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMDVC_W                                             4
#define CPU_ETM_TRCIDR4_NUMDVC_M                                    0x000000F0
#define CPU_ETM_TRCIDR4_NUMDVC_S                                             4

// Field:   [3:0] NUMACPAIRS
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR4_NUMACPAIRS_W                                         4
#define CPU_ETM_TRCIDR4_NUMACPAIRS_M                                0x0000000F
#define CPU_ETM_TRCIDR4_NUMACPAIRS_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR5
//
//*****************************************************************************
// Field:    [31] REDFUNCNTR
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_REDFUNCNTR                                  0x80000000
#define CPU_ETM_TRCIDR5_REDFUNCNTR_BITN                                     31
#define CPU_ETM_TRCIDR5_REDFUNCNTR_M                                0x80000000
#define CPU_ETM_TRCIDR5_REDFUNCNTR_S                                        31

// Field: [30:28] NUMCNTR
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_NUMCNTR_W                                            3
#define CPU_ETM_TRCIDR5_NUMCNTR_M                                   0x70000000
#define CPU_ETM_TRCIDR5_NUMCNTR_S                                           28

// Field: [27:25] NUMSEQSTATE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_NUMSEQSTATE_W                                        3
#define CPU_ETM_TRCIDR5_NUMSEQSTATE_M                               0x0E000000
#define CPU_ETM_TRCIDR5_NUMSEQSTATE_S                                       25

// Field:    [23] LPOVERRIDE
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_LPOVERRIDE                                  0x00800000
#define CPU_ETM_TRCIDR5_LPOVERRIDE_BITN                                     23
#define CPU_ETM_TRCIDR5_LPOVERRIDE_M                                0x00800000
#define CPU_ETM_TRCIDR5_LPOVERRIDE_S                                        23

// Field:    [22] ATBTRIG
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_ATBTRIG                                     0x00400000
#define CPU_ETM_TRCIDR5_ATBTRIG_BITN                                        22
#define CPU_ETM_TRCIDR5_ATBTRIG_M                                   0x00400000
#define CPU_ETM_TRCIDR5_ATBTRIG_S                                           22

// Field: [21:16] TRACEIDSIZE
//
// reads as 0x07
#define CPU_ETM_TRCIDR5_TRACEIDSIZE_W                                        6
#define CPU_ETM_TRCIDR5_TRACEIDSIZE_M                               0x003F0000
#define CPU_ETM_TRCIDR5_TRACEIDSIZE_S                                       16

// Field:  [11:9] NUMEXTINSEL
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_NUMEXTINSEL_W                                        3
#define CPU_ETM_TRCIDR5_NUMEXTINSEL_M                               0x00000E00
#define CPU_ETM_TRCIDR5_NUMEXTINSEL_S                                        9

// Field:   [8:0] NUMEXTIN
//
// reads as `ImpDef
#define CPU_ETM_TRCIDR5_NUMEXTIN_W                                           9
#define CPU_ETM_TRCIDR5_NUMEXTIN_M                                  0x000001FF
#define CPU_ETM_TRCIDR5_NUMEXTIN_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCRSCTLR2
//
//*****************************************************************************
// Field:    [21] PAIRINV
//
// Inverts the result of a combined pair of resources.  This bit is only
// implemented on the lower register for a pair of resource selectors
#define CPU_ETM_TRCRSCTLR2_PAIRINV                                  0x00200000
#define CPU_ETM_TRCRSCTLR2_PAIRINV_BITN                                     21
#define CPU_ETM_TRCRSCTLR2_PAIRINV_M                                0x00200000
#define CPU_ETM_TRCRSCTLR2_PAIRINV_S                                        21

// Field:    [20] INV
//
// Inverts the selected resources
#define CPU_ETM_TRCRSCTLR2_INV                                      0x00100000
#define CPU_ETM_TRCRSCTLR2_INV_BITN                                         20
#define CPU_ETM_TRCRSCTLR2_INV_M                                    0x00100000
#define CPU_ETM_TRCRSCTLR2_INV_S                                            20

// Field: [18:16] GROUP
//
// Selects a group of resource
#define CPU_ETM_TRCRSCTLR2_GROUP_W                                           3
#define CPU_ETM_TRCRSCTLR2_GROUP_M                                  0x00070000
#define CPU_ETM_TRCRSCTLR2_GROUP_S                                          16

// Field:   [7:0] SELECT
//
// Selects one or more resources from the wanted group. One bit is provided per
// resource from the group
#define CPU_ETM_TRCRSCTLR2_SELECT_W                                          8
#define CPU_ETM_TRCRSCTLR2_SELECT_M                                 0x000000FF
#define CPU_ETM_TRCRSCTLR2_SELECT_S                                          0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCRSCTLR3
//
//*****************************************************************************
// Field:    [21] PAIRINV
//
// Inverts the result of a combined pair of resources.  This bit is only
// implemented on the lower register for a pair of resource selectors
#define CPU_ETM_TRCRSCTLR3_PAIRINV                                  0x00200000
#define CPU_ETM_TRCRSCTLR3_PAIRINV_BITN                                     21
#define CPU_ETM_TRCRSCTLR3_PAIRINV_M                                0x00200000
#define CPU_ETM_TRCRSCTLR3_PAIRINV_S                                        21

// Field:    [20] INV
//
// Inverts the selected resources
#define CPU_ETM_TRCRSCTLR3_INV                                      0x00100000
#define CPU_ETM_TRCRSCTLR3_INV_BITN                                         20
#define CPU_ETM_TRCRSCTLR3_INV_M                                    0x00100000
#define CPU_ETM_TRCRSCTLR3_INV_S                                            20

// Field: [18:16] GROUP
//
// Selects a group of resource
#define CPU_ETM_TRCRSCTLR3_GROUP_W                                           3
#define CPU_ETM_TRCRSCTLR3_GROUP_M                                  0x00070000
#define CPU_ETM_TRCRSCTLR3_GROUP_S                                          16

// Field:   [7:0] SELECT
//
// Selects one or more resources from the wanted group. One bit is provided per
// resource from the group
#define CPU_ETM_TRCRSCTLR3_SELECT_W                                          8
#define CPU_ETM_TRCRSCTLR3_SELECT_M                                 0x000000FF
#define CPU_ETM_TRCRSCTLR3_SELECT_S                                          0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR0
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR0_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR0_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR0_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR0_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR0_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR0_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR0_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR0_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR0_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR0_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR0_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR0_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR0_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR0_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR0_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR0_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR0_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR0_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR0_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR0_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR1
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR1_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR1_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR1_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR1_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR1_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR1_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR1_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR1_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR1_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR1_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR1_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR1_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR1_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR1_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR1_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR1_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR1_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR1_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR1_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR1_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR2
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR2_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR2_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR2_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR2_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR2_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR2_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR2_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR2_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR2_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR2_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR2_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR2_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR2_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR2_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR2_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR2_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR2_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR2_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR2_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR2_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR3
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR3_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR3_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR3_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR3_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR3_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR3_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR3_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR3_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR3_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR3_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR3_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR3_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR3_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR3_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR3_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR3_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR3_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR3_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR3_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR3_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR4
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR4_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR4_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR4_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR4_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR4_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR4_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR4_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR4_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR4_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR4_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR4_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR4_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR4_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR4_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR4_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR4_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR4_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR4_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR4_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR4_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR5
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR5_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR5_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR5_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR5_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR5_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR5_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR5_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR5_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR5_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR5_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR5_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR5_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR5_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR5_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR5_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR5_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR5_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR5_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR5_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR5_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSCSR6
//
//*****************************************************************************
// Field:    [31] STATUS
//
// Single-shot status bit. Indicates if any of the comparators, that
// TRCSSCCRn.SAC or TRCSSCCRn.ARC selects, have matched
#define CPU_ETM_TRCSSCSR6_STATUS                                    0x80000000
#define CPU_ETM_TRCSSCSR6_STATUS_BITN                                       31
#define CPU_ETM_TRCSSCSR6_STATUS_M                                  0x80000000
#define CPU_ETM_TRCSSCSR6_STATUS_S                                          31

// Field:     [3] PC
//
// PE comparator input support. Indicates if the trace unit supports
// Single-shot PE comparator inputs. This field is read-only
#define CPU_ETM_TRCSSCSR6_PC                                        0x00000008
#define CPU_ETM_TRCSSCSR6_PC_BITN                                            3
#define CPU_ETM_TRCSSCSR6_PC_M                                      0x00000008
#define CPU_ETM_TRCSSCSR6_PC_S                                               3

// Field:     [2] DV
//
// Data value comparator support bit. Indicates if the trace unit supports data
// address with data value comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR6_DV                                        0x00000004
#define CPU_ETM_TRCSSCSR6_DV_BITN                                            2
#define CPU_ETM_TRCSSCSR6_DV_M                                      0x00000004
#define CPU_ETM_TRCSSCSR6_DV_S                                               2

// Field:     [1] DA
//
// Data address comparator support bit. Indicates if the trace unit supports
// data address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR6_DA                                        0x00000002
#define CPU_ETM_TRCSSCSR6_DA_BITN                                            1
#define CPU_ETM_TRCSSCSR6_DA_M                                      0x00000002
#define CPU_ETM_TRCSSCSR6_DA_S                                               1

// Field:     [0] INST
//
// Instruction address comparator support bit. Indicates if the trace unit
// supports instruction address comparisons. This field is read-only:
#define CPU_ETM_TRCSSCSR6_INST                                      0x00000001
#define CPU_ETM_TRCSSCSR6_INST_BITN                                          0
#define CPU_ETM_TRCSSCSR6_INST_M                                    0x00000001
#define CPU_ETM_TRCSSCSR6_INST_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR0
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR0_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR0_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR0_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR0_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR0_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR0_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR0_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR0_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR1
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR1_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR1_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR1_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR1_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR1_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR1_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR1_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR1_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR2
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR2_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR2_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR2_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR2_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR2_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR2_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR2_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR2_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR3
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR3_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR3_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR3_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR3_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR3_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR3_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR3_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR3_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR4
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR4_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR4_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR4_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR4_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR4_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR4_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR4_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR4_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR5
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR5_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR5_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR5_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR5_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR5_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR5_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR5_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR5_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCSSPCICR6
//
//*****************************************************************************
// Field:     [1] PC1
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR6_PC1                                     0x00000002
#define CPU_ETM_TRCSSPCICR6_PC1_BITN                                         1
#define CPU_ETM_TRCSSPCICR6_PC1_M                                   0x00000002
#define CPU_ETM_TRCSSPCICR6_PC1_S                                            1

// Field:     [0] PC0
//
// Selects one or more PE comparator inputs for Single-shot control.
// TRCIDR4.NUMPC defines the size of the PC field.  1 bit is provided for each
// implemented PE comparator input.  For example, if bit[1] == 1 this selects
// PE comparator input 1 for Single-shot control
#define CPU_ETM_TRCSSPCICR6_PC0                                     0x00000001
#define CPU_ETM_TRCSSPCICR6_PC0_BITN                                         0
#define CPU_ETM_TRCSSPCICR6_PC0_M                                   0x00000001
#define CPU_ETM_TRCSSPCICR6_PC0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPDCR
//
//*****************************************************************************
// Field:     [3] PU
//
// Powerup request bit:
#define CPU_ETM_TRCPDCR_PU                                          0x00000008
#define CPU_ETM_TRCPDCR_PU_BITN                                              3
#define CPU_ETM_TRCPDCR_PU_M                                        0x00000008
#define CPU_ETM_TRCPDCR_PU_S                                                 3

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPDSR
//
//*****************************************************************************
// Field:     [5] OSLK
//
// OS Lock status bit:
#define CPU_ETM_TRCPDSR_OSLK                                        0x00000020
#define CPU_ETM_TRCPDSR_OSLK_BITN                                            5
#define CPU_ETM_TRCPDSR_OSLK_M                                      0x00000020
#define CPU_ETM_TRCPDSR_OSLK_S                                               5

// Field:     [1] STICKYPD
//
// Sticky powerdown status bit. Indicates whether the trace register state is
// valid:
#define CPU_ETM_TRCPDSR_STICKYPD                                    0x00000002
#define CPU_ETM_TRCPDSR_STICKYPD_BITN                                        1
#define CPU_ETM_TRCPDSR_STICKYPD_M                                  0x00000002
#define CPU_ETM_TRCPDSR_STICKYPD_S                                           1

// Field:     [0] POWER
//
// Power status bit:
#define CPU_ETM_TRCPDSR_POWER                                       0x00000001
#define CPU_ETM_TRCPDSR_POWER_BITN                                           0
#define CPU_ETM_TRCPDSR_POWER_M                                     0x00000001
#define CPU_ETM_TRCPDSR_POWER_S                                              0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCITATBIDR
//
//*****************************************************************************
// Field:   [6:0] ID
//
// Trace ID
#define CPU_ETM_TRCITATBIDR_ID_W                                             7
#define CPU_ETM_TRCITATBIDR_ID_M                                    0x0000007F
#define CPU_ETM_TRCITATBIDR_ID_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCITIATBINR
//
//*****************************************************************************
// Field:     [1] AFVALIDM
//
// Integration Mode instruction AFVALIDM in
#define CPU_ETM_TRCITIATBINR_AFVALIDM                               0x00000002
#define CPU_ETM_TRCITIATBINR_AFVALIDM_BITN                                   1
#define CPU_ETM_TRCITIATBINR_AFVALIDM_M                             0x00000002
#define CPU_ETM_TRCITIATBINR_AFVALIDM_S                                      1

// Field:     [0] ATREADYM
//
// Integration Mode instruction ATREADYM in
#define CPU_ETM_TRCITIATBINR_ATREADYM                               0x00000001
#define CPU_ETM_TRCITIATBINR_ATREADYM_BITN                                   0
#define CPU_ETM_TRCITIATBINR_ATREADYM_M                             0x00000001
#define CPU_ETM_TRCITIATBINR_ATREADYM_S                                      0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCITIATBOUTR
//
//*****************************************************************************
// Field:     [1] AFREADY
//
// Integration Mode instruction AFREADY out
#define CPU_ETM_TRCITIATBOUTR_AFREADY                               0x00000002
#define CPU_ETM_TRCITIATBOUTR_AFREADY_BITN                                   1
#define CPU_ETM_TRCITIATBOUTR_AFREADY_M                             0x00000002
#define CPU_ETM_TRCITIATBOUTR_AFREADY_S                                      1

// Field:     [0] ATVALID
//
// Integration Mode instruction ATVALID out
#define CPU_ETM_TRCITIATBOUTR_ATVALID                               0x00000001
#define CPU_ETM_TRCITIATBOUTR_ATVALID_BITN                                   0
#define CPU_ETM_TRCITIATBOUTR_ATVALID_M                             0x00000001
#define CPU_ETM_TRCITIATBOUTR_ATVALID_S                                      0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCLAIMSET
//
//*****************************************************************************
// Field:     [3] SET3
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMSET_SET3                                    0x00000008
#define CPU_ETM_TRCCLAIMSET_SET3_BITN                                        3
#define CPU_ETM_TRCCLAIMSET_SET3_M                                  0x00000008
#define CPU_ETM_TRCCLAIMSET_SET3_S                                           3

// Field:     [2] SET2
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMSET_SET2                                    0x00000004
#define CPU_ETM_TRCCLAIMSET_SET2_BITN                                        2
#define CPU_ETM_TRCCLAIMSET_SET2_M                                  0x00000004
#define CPU_ETM_TRCCLAIMSET_SET2_S                                           2

// Field:     [1] SET1
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMSET_SET1                                    0x00000002
#define CPU_ETM_TRCCLAIMSET_SET1_BITN                                        1
#define CPU_ETM_TRCCLAIMSET_SET1_M                                  0x00000002
#define CPU_ETM_TRCCLAIMSET_SET1_S                                           1

// Field:     [0] SET0
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMSET_SET0                                    0x00000001
#define CPU_ETM_TRCCLAIMSET_SET0_BITN                                        0
#define CPU_ETM_TRCCLAIMSET_SET0_M                                  0x00000001
#define CPU_ETM_TRCCLAIMSET_SET0_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCLAIMCLR
//
//*****************************************************************************
// Field:     [3] CLR3
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMCLR_CLR3                                    0x00000008
#define CPU_ETM_TRCCLAIMCLR_CLR3_BITN                                        3
#define CPU_ETM_TRCCLAIMCLR_CLR3_M                                  0x00000008
#define CPU_ETM_TRCCLAIMCLR_CLR3_S                                           3

// Field:     [2] CLR2
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMCLR_CLR2                                    0x00000004
#define CPU_ETM_TRCCLAIMCLR_CLR2_BITN                                        2
#define CPU_ETM_TRCCLAIMCLR_CLR2_M                                  0x00000004
#define CPU_ETM_TRCCLAIMCLR_CLR2_S                                           2

// Field:     [1] CLR1
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMCLR_CLR1                                    0x00000002
#define CPU_ETM_TRCCLAIMCLR_CLR1_BITN                                        1
#define CPU_ETM_TRCCLAIMCLR_CLR1_M                                  0x00000002
#define CPU_ETM_TRCCLAIMCLR_CLR1_S                                           1

// Field:     [0] CLR0
//
// When a write to one of these bits occurs, with the value:
#define CPU_ETM_TRCCLAIMCLR_CLR0                                    0x00000001
#define CPU_ETM_TRCCLAIMCLR_CLR0_BITN                                        0
#define CPU_ETM_TRCCLAIMCLR_CLR0_M                                  0x00000001
#define CPU_ETM_TRCCLAIMCLR_CLR0_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCAUTHSTATUS
//
//*****************************************************************************
// Field:   [7:6] SNID
//
// Indicates whether the system enables the trace unit to support Secure
// non-invasive debug:
#define CPU_ETM_TRCAUTHSTATUS_SNID_W                                         2
#define CPU_ETM_TRCAUTHSTATUS_SNID_M                                0x000000C0
#define CPU_ETM_TRCAUTHSTATUS_SNID_S                                         6

// Field:   [5:4] SID
//
// Indicates whether the trace unit supports Secure invasive debug:
#define CPU_ETM_TRCAUTHSTATUS_SID_W                                          2
#define CPU_ETM_TRCAUTHSTATUS_SID_M                                 0x00000030
#define CPU_ETM_TRCAUTHSTATUS_SID_S                                          4

// Field:   [3:2] NSNID
//
// Indicates whether the system enables the trace unit to support Non-secure
// non-invasive debug:
#define CPU_ETM_TRCAUTHSTATUS_NSNID_W                                        2
#define CPU_ETM_TRCAUTHSTATUS_NSNID_M                               0x0000000C
#define CPU_ETM_TRCAUTHSTATUS_NSNID_S                                        2

// Field:   [1:0] NSID
//
// Indicates whether the trace unit supports Non-secure invasive debug:
#define CPU_ETM_TRCAUTHSTATUS_NSID_W                                         2
#define CPU_ETM_TRCAUTHSTATUS_NSID_M                                0x00000003
#define CPU_ETM_TRCAUTHSTATUS_NSID_S                                         0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCDEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// reads as 0b01000111011
#define CPU_ETM_TRCDEVARCH_ARCHITECT_W                                      11
#define CPU_ETM_TRCDEVARCH_ARCHITECT_M                              0xFFE00000
#define CPU_ETM_TRCDEVARCH_ARCHITECT_S                                      21

// Field:    [20] PRESENT
//
// reads as 0b1
#define CPU_ETM_TRCDEVARCH_PRESENT                                  0x00100000
#define CPU_ETM_TRCDEVARCH_PRESENT_BITN                                     20
#define CPU_ETM_TRCDEVARCH_PRESENT_M                                0x00100000
#define CPU_ETM_TRCDEVARCH_PRESENT_S                                        20

// Field: [19:16] REVISION
//
// reads as 0b0000
#define CPU_ETM_TRCDEVARCH_REVISION_W                                        4
#define CPU_ETM_TRCDEVARCH_REVISION_M                               0x000F0000
#define CPU_ETM_TRCDEVARCH_REVISION_S                                       16

// Field:  [15:0] ARCHID
//
// reads as 0b0100101000010011
#define CPU_ETM_TRCDEVARCH_ARCHID_W                                         16
#define CPU_ETM_TRCDEVARCH_ARCHID_M                                 0x0000FFFF
#define CPU_ETM_TRCDEVARCH_ARCHID_S                                          0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCDEVID
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCDEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// reads as 0b0001
#define CPU_ETM_TRCDEVTYPE_SUB_W                                             4
#define CPU_ETM_TRCDEVTYPE_SUB_M                                    0x000000F0
#define CPU_ETM_TRCDEVTYPE_SUB_S                                             4

// Field:   [3:0] MAJOR
//
// reads as 0b0011
#define CPU_ETM_TRCDEVTYPE_MAJOR_W                                           4
#define CPU_ETM_TRCDEVTYPE_MAJOR_M                                  0x0000000F
#define CPU_ETM_TRCDEVTYPE_MAJOR_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR4_SIZE_W                                              4
#define CPU_ETM_TRCPIDR4_SIZE_M                                     0x000000F0
#define CPU_ETM_TRCPIDR4_SIZE_S                                              4

// Field:   [3:0] DES_2
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR4_DES_2_W                                             4
#define CPU_ETM_TRCPIDR4_DES_2_M                                    0x0000000F
#define CPU_ETM_TRCPIDR4_DES_2_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR0_PART_0_W                                            8
#define CPU_ETM_TRCPIDR0_PART_0_M                                   0x000000FF
#define CPU_ETM_TRCPIDR0_PART_0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR1_DES_0_W                                             4
#define CPU_ETM_TRCPIDR1_DES_0_M                                    0x000000F0
#define CPU_ETM_TRCPIDR1_DES_0_S                                             4

// Field:   [3:0] PART_0
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR1_PART_0_W                                            4
#define CPU_ETM_TRCPIDR1_PART_0_M                                   0x0000000F
#define CPU_ETM_TRCPIDR1_PART_0_S                                            0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR2_REVISION_W                                          4
#define CPU_ETM_TRCPIDR2_REVISION_M                                 0x000000F0
#define CPU_ETM_TRCPIDR2_REVISION_S                                          4

// Field:     [3] JEDEC
//
// reads as 0b1
#define CPU_ETM_TRCPIDR2_JEDEC                                      0x00000008
#define CPU_ETM_TRCPIDR2_JEDEC_BITN                                          3
#define CPU_ETM_TRCPIDR2_JEDEC_M                                    0x00000008
#define CPU_ETM_TRCPIDR2_JEDEC_S                                             3

// Field:   [2:0] DES_0
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR2_DES_0_W                                             3
#define CPU_ETM_TRCPIDR2_DES_0_M                                    0x00000007
#define CPU_ETM_TRCPIDR2_DES_0_S                                             0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCPIDR3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR3_REVAND_W                                            4
#define CPU_ETM_TRCPIDR3_REVAND_M                                   0x000000F0
#define CPU_ETM_TRCPIDR3_REVAND_S                                            4

// Field:   [3:0] CMOD
//
// reads as `ImpDef
#define CPU_ETM_TRCPIDR3_CMOD_W                                              4
#define CPU_ETM_TRCPIDR3_CMOD_M                                     0x0000000F
#define CPU_ETM_TRCPIDR3_CMOD_S                                              0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCIDR0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// reads as 0b00001101
#define CPU_ETM_TRCCIDR0_PRMBL_0_W                                           8
#define CPU_ETM_TRCCIDR0_PRMBL_0_M                                  0x000000FF
#define CPU_ETM_TRCCIDR0_PRMBL_0_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCIDR1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// reads as 0b1001
#define CPU_ETM_TRCCIDR1_CLASS_W                                             4
#define CPU_ETM_TRCCIDR1_CLASS_M                                    0x000000F0
#define CPU_ETM_TRCCIDR1_CLASS_S                                             4

// Field:   [3:0] PRMBL_1
//
// reads as 0b0000
#define CPU_ETM_TRCCIDR1_PRMBL_1_W                                           4
#define CPU_ETM_TRCCIDR1_PRMBL_1_M                                  0x0000000F
#define CPU_ETM_TRCCIDR1_PRMBL_1_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCIDR2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// reads as 0b00000101
#define CPU_ETM_TRCCIDR2_PRMBL_2_W                                           8
#define CPU_ETM_TRCCIDR2_PRMBL_2_M                                  0x000000FF
#define CPU_ETM_TRCCIDR2_PRMBL_2_S                                           0

//*****************************************************************************
//
// Register: CPU_ETM_O_TRCCIDR3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// reads as 0b10110001
#define CPU_ETM_TRCCIDR3_PRMBL_3_W                                           8
#define CPU_ETM_TRCCIDR3_PRMBL_3_M                                  0x000000FF
#define CPU_ETM_TRCCIDR3_PRMBL_3_S                                           0


#endif // __CPU_ETM__
