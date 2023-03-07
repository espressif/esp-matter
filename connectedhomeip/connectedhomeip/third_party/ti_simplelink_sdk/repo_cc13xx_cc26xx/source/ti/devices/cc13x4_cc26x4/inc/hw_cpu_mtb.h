/******************************************************************************
*  Filename:       hw_cpu_mtb_h
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

#ifndef __HW_CPU_MTB_H__
#define __HW_CPU_MTB_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_MTB component
//
//*****************************************************************************
// The MTB_POSITION register contains the trace write pointer and the wrap bit.
#define CPU_MTB_O_POSITION                                          0x00000000

// The MTB_MASTER register contains the main trace enable bit and other trace
// control fields.
#define CPU_MTB_O_MASTER                                            0x00000004

// The MTB_FLOW register contains the WATERMARK address and the AUTOSTOP and
// AUTOHALT control bits.
#define CPU_MTB_O_FLOW                                              0x00000008

// The MTB_BASE register indicates where the SRAM is located in the processor
// memory map.
#define CPU_MTB_O_BASE                                              0x0000000C

// The MTB_TSTART register controls the trace start events using the DWT
// CMPMATCH feature.
#define CPU_MTB_O_TSTART                                            0x00000010

// The MTB_TSTOP register controls the trace stop events using the DWT CMPMATCH
// feature.
#define CPU_MTB_O_TSTOP                                             0x00000014

// The MTB_SECURE register allows the SRAM region to be partitioned into two
// regions, with one region being defined as Secure and the other as
// Non-secure.
#define CPU_MTB_O_SECURE                                            0x00000020

// MTB_DEVARCH
#define CPU_MTB_O_DEVARCH                                           0x00000FBC

// MTB_DEVID
#define CPU_MTB_O_DEVID                                             0x00000FC8

// MTB_DEVTYPE
#define CPU_MTB_O_DEVTYPE                                           0x00000FCC

// MTB_PID4
#define CPU_MTB_O_PID4                                              0x00000FD0

// MTB_PID5
#define CPU_MTB_O_PID5                                              0x00000FD4

// MTB_PID6
#define CPU_MTB_O_PID6                                              0x00000FD8

// MTB_PID7
#define CPU_MTB_O_PID7                                              0x00000FDC

// MTB_PID0
#define CPU_MTB_O_PID0                                              0x00000FE0

// MTB_PID1
#define CPU_MTB_O_PID1                                              0x00000FE4

// MTB_PID2
#define CPU_MTB_O_PID2                                              0x00000FE8

// MTB_PID3
#define CPU_MTB_O_PID3                                              0x00000FEC

// MTB_CID0
#define CPU_MTB_O_CID0                                              0x00000FF0

// MTB_CID1
#define CPU_MTB_O_CID1                                              0x00000FF4

// MTB_CID2
#define CPU_MTB_O_CID2                                              0x00000FF8

// MTB_CID3
#define CPU_MTB_O_CID3                                              0x00000FFC

//*****************************************************************************
//
// Register: CPU_MTB_O_POSITION
//
//*****************************************************************************
// Field:  [31:3] POINTER
//
//
#define CPU_MTB_POSITION_POINTER_W                                          29
#define CPU_MTB_POSITION_POINTER_M                                  0xFFFFFFF8
#define CPU_MTB_POSITION_POINTER_S                                           3

// Field:     [2] WRAP
//
//
#define CPU_MTB_POSITION_WRAP                                       0x00000004
#define CPU_MTB_POSITION_WRAP_BITN                                           2
#define CPU_MTB_POSITION_WRAP_M                                     0x00000004
#define CPU_MTB_POSITION_WRAP_S                                              2

//*****************************************************************************
//
// Register: CPU_MTB_O_MASTER
//
//*****************************************************************************
// Field:    [31] EN
//
//
#define CPU_MTB_MASTER_EN                                           0x80000000
#define CPU_MTB_MASTER_EN_BITN                                              31
#define CPU_MTB_MASTER_EN_M                                         0x80000000
#define CPU_MTB_MASTER_EN_S                                                 31

// Field:    [30] NSEN
//
//
#define CPU_MTB_MASTER_NSEN                                         0x40000000
#define CPU_MTB_MASTER_NSEN_BITN                                            30
#define CPU_MTB_MASTER_NSEN_M                                       0x40000000
#define CPU_MTB_MASTER_NSEN_S                                               30

// Field:     [9] HALTREQ
//
//
#define CPU_MTB_MASTER_HALTREQ                                      0x00000200
#define CPU_MTB_MASTER_HALTREQ_BITN                                          9
#define CPU_MTB_MASTER_HALTREQ_M                                    0x00000200
#define CPU_MTB_MASTER_HALTREQ_S                                             9

// Field:     [8] RAMPRIV
//
//
#define CPU_MTB_MASTER_RAMPRIV                                      0x00000100
#define CPU_MTB_MASTER_RAMPRIV_BITN                                          8
#define CPU_MTB_MASTER_RAMPRIV_M                                    0x00000100
#define CPU_MTB_MASTER_RAMPRIV_S                                             8

// Field:     [6] TSTOPEN
//
//
#define CPU_MTB_MASTER_TSTOPEN                                      0x00000040
#define CPU_MTB_MASTER_TSTOPEN_BITN                                          6
#define CPU_MTB_MASTER_TSTOPEN_M                                    0x00000040
#define CPU_MTB_MASTER_TSTOPEN_S                                             6

// Field:     [5] TSTARTEN
//
//
#define CPU_MTB_MASTER_TSTARTEN                                     0x00000020
#define CPU_MTB_MASTER_TSTARTEN_BITN                                         5
#define CPU_MTB_MASTER_TSTARTEN_M                                   0x00000020
#define CPU_MTB_MASTER_TSTARTEN_S                                            5

// Field:   [4:0] MASK
//
//
#define CPU_MTB_MASTER_MASK_W                                                5
#define CPU_MTB_MASTER_MASK_M                                       0x0000001F
#define CPU_MTB_MASTER_MASK_S                                                0

//*****************************************************************************
//
// Register: CPU_MTB_O_FLOW
//
//*****************************************************************************
// Field:  [31:3] WATERMARK
//
//
#define CPU_MTB_FLOW_WATERMARK_W                                            29
#define CPU_MTB_FLOW_WATERMARK_M                                    0xFFFFFFF8
#define CPU_MTB_FLOW_WATERMARK_S                                             3

// Field:     [1] AUTOHALT
//
//
#define CPU_MTB_FLOW_AUTOHALT                                       0x00000002
#define CPU_MTB_FLOW_AUTOHALT_BITN                                           1
#define CPU_MTB_FLOW_AUTOHALT_M                                     0x00000002
#define CPU_MTB_FLOW_AUTOHALT_S                                              1

// Field:     [0] AUTOSTOP
//
//
#define CPU_MTB_FLOW_AUTOSTOP                                       0x00000001
#define CPU_MTB_FLOW_AUTOSTOP_BITN                                           0
#define CPU_MTB_FLOW_AUTOSTOP_M                                     0x00000001
#define CPU_MTB_FLOW_AUTOSTOP_S                                              0

//*****************************************************************************
//
// Register: CPU_MTB_O_BASE
//
//*****************************************************************************
// Field:  [31:5] BASE
//
//
#define CPU_MTB_BASE_BASE_W                                                 27
#define CPU_MTB_BASE_BASE_M                                         0xFFFFFFE0
#define CPU_MTB_BASE_BASE_S                                                  5

//*****************************************************************************
//
// Register: CPU_MTB_O_TSTART
//
//*****************************************************************************
// Field:     [3] CMPMATCH3
//
// Reserved, `ImpDefRES0
#define CPU_MTB_TSTART_CMPMATCH3                                    0x00000008
#define CPU_MTB_TSTART_CMPMATCH3_BITN                                        3
#define CPU_MTB_TSTART_CMPMATCH3_M                                  0x00000008
#define CPU_MTB_TSTART_CMPMATCH3_S                                           3

// Field:     [2] CMPMATCH2
//
// Reserved, `ImpDefRES0
#define CPU_MTB_TSTART_CMPMATCH2                                    0x00000004
#define CPU_MTB_TSTART_CMPMATCH2_BITN                                        2
#define CPU_MTB_TSTART_CMPMATCH2_M                                  0x00000004
#define CPU_MTB_TSTART_CMPMATCH2_S                                           2

// Field:     [1] CMPMATCH1
//
//
#define CPU_MTB_TSTART_CMPMATCH1                                    0x00000002
#define CPU_MTB_TSTART_CMPMATCH1_BITN                                        1
#define CPU_MTB_TSTART_CMPMATCH1_M                                  0x00000002
#define CPU_MTB_TSTART_CMPMATCH1_S                                           1

// Field:     [0] CMPMATCH0
//
//
#define CPU_MTB_TSTART_CMPMATCH0                                    0x00000001
#define CPU_MTB_TSTART_CMPMATCH0_BITN                                        0
#define CPU_MTB_TSTART_CMPMATCH0_M                                  0x00000001
#define CPU_MTB_TSTART_CMPMATCH0_S                                           0

//*****************************************************************************
//
// Register: CPU_MTB_O_TSTOP
//
//*****************************************************************************
// Field:     [3] CMPMATCH3
//
// Reserved, `ImpDefRES0
#define CPU_MTB_TSTOP_CMPMATCH3                                     0x00000008
#define CPU_MTB_TSTOP_CMPMATCH3_BITN                                         3
#define CPU_MTB_TSTOP_CMPMATCH3_M                                   0x00000008
#define CPU_MTB_TSTOP_CMPMATCH3_S                                            3

// Field:     [2] CMPMATCH2
//
// Reserved, `ImpDefRES0
#define CPU_MTB_TSTOP_CMPMATCH2                                     0x00000004
#define CPU_MTB_TSTOP_CMPMATCH2_BITN                                         2
#define CPU_MTB_TSTOP_CMPMATCH2_M                                   0x00000004
#define CPU_MTB_TSTOP_CMPMATCH2_S                                            2

// Field:     [1] CMPMATCH1
//
//
#define CPU_MTB_TSTOP_CMPMATCH1                                     0x00000002
#define CPU_MTB_TSTOP_CMPMATCH1_BITN                                         1
#define CPU_MTB_TSTOP_CMPMATCH1_M                                   0x00000002
#define CPU_MTB_TSTOP_CMPMATCH1_S                                            1

// Field:     [0] CMPMATCH0
//
//
#define CPU_MTB_TSTOP_CMPMATCH0                                     0x00000001
#define CPU_MTB_TSTOP_CMPMATCH0_BITN                                         0
#define CPU_MTB_TSTOP_CMPMATCH0_M                                   0x00000001
#define CPU_MTB_TSTOP_CMPMATCH0_S                                            0

//*****************************************************************************
//
// Register: CPU_MTB_O_SECURE
//
//*****************************************************************************
// Field:  [31:5] THRESHOLD
//
//
#define CPU_MTB_SECURE_THRESHOLD_W                                          27
#define CPU_MTB_SECURE_THRESHOLD_M                                  0xFFFFFFE0
#define CPU_MTB_SECURE_THRESHOLD_S                                           5

// Field:     [1] NS
//
//
#define CPU_MTB_SECURE_NS                                           0x00000002
#define CPU_MTB_SECURE_NS_BITN                                               1
#define CPU_MTB_SECURE_NS_M                                         0x00000002
#define CPU_MTB_SECURE_NS_S                                                  1

// Field:     [0] THRSEN
//
//
#define CPU_MTB_SECURE_THRSEN                                       0x00000001
#define CPU_MTB_SECURE_THRSEN_BITN                                           0
#define CPU_MTB_SECURE_THRSEN_M                                     0x00000001
#define CPU_MTB_SECURE_THRSEN_S                                              0

//*****************************************************************************
//
// Register: CPU_MTB_O_DEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// reads as 0x23B
#define CPU_MTB_DEVARCH_ARCHITECT_W                                         11
#define CPU_MTB_DEVARCH_ARCHITECT_M                                 0xFFE00000
#define CPU_MTB_DEVARCH_ARCHITECT_S                                         21

// Field:    [20] PRESENT
//
// reads as 0b1
#define CPU_MTB_DEVARCH_PRESENT                                     0x00100000
#define CPU_MTB_DEVARCH_PRESENT_BITN                                        20
#define CPU_MTB_DEVARCH_PRESENT_M                                   0x00100000
#define CPU_MTB_DEVARCH_PRESENT_S                                           20

// Field: [19:16] REVISION
//
// reads as 0x0
#define CPU_MTB_DEVARCH_REVISION_W                                           4
#define CPU_MTB_DEVARCH_REVISION_M                                  0x000F0000
#define CPU_MTB_DEVARCH_REVISION_S                                          16

// Field:  [15:0] ARCHID
//
// reads as 0x0A31
#define CPU_MTB_DEVARCH_ARCHID_W                                            16
#define CPU_MTB_DEVARCH_ARCHID_M                                    0x0000FFFF
#define CPU_MTB_DEVARCH_ARCHID_S                                             0

//*****************************************************************************
//
// Register: CPU_MTB_O_DEVID
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_MTB_O_DEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// reads as 0x3
#define CPU_MTB_DEVTYPE_SUB_W                                                4
#define CPU_MTB_DEVTYPE_SUB_M                                       0x000000F0
#define CPU_MTB_DEVTYPE_SUB_S                                                4

// Field:   [3:0] MAJOR
//
// reads as 0x1
#define CPU_MTB_DEVTYPE_MAJOR_W                                              4
#define CPU_MTB_DEVTYPE_MAJOR_M                                     0x0000000F
#define CPU_MTB_DEVTYPE_MAJOR_S                                              0

//*****************************************************************************
//
// Register: CPU_MTB_O_PID4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// reads as 0x0
#define CPU_MTB_PID4_SIZE_W                                                  4
#define CPU_MTB_PID4_SIZE_M                                         0x000000F0
#define CPU_MTB_PID4_SIZE_S                                                  4

// Field:   [3:0] DES_2
//
// reads as 0x4
#define CPU_MTB_PID4_DES_2_W                                                 4
#define CPU_MTB_PID4_DES_2_M                                        0x0000000F
#define CPU_MTB_PID4_DES_2_S                                                 0

//*****************************************************************************
//
// Register: CPU_MTB_O_PID5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_MTB_O_PID6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_MTB_O_PID7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_MTB_O_PID0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// reads as 0x8E
#define CPU_MTB_PID0_PART_0_W                                                8
#define CPU_MTB_PID0_PART_0_M                                       0x000000FF
#define CPU_MTB_PID0_PART_0_S                                                0

//*****************************************************************************
//
// Register: CPU_MTB_O_PID1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// reads as 0xB
#define CPU_MTB_PID1_DES_0_W                                                 4
#define CPU_MTB_PID1_DES_0_M                                        0x000000F0
#define CPU_MTB_PID1_DES_0_S                                                 4

// Field:   [3:0] PART_1
//
// reads as 0x9
#define CPU_MTB_PID1_PART_1_W                                                4
#define CPU_MTB_PID1_PART_1_M                                       0x0000000F
#define CPU_MTB_PID1_PART_1_S                                                0

//*****************************************************************************
//
// Register: CPU_MTB_O_PID2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// reads as 0x0
#define CPU_MTB_PID2_REVISION_W                                              4
#define CPU_MTB_PID2_REVISION_M                                     0x000000F0
#define CPU_MTB_PID2_REVISION_S                                              4

// Field:     [3] JEDEC
//
// reads as 0b1
#define CPU_MTB_PID2_JEDEC                                          0x00000008
#define CPU_MTB_PID2_JEDEC_BITN                                              3
#define CPU_MTB_PID2_JEDEC_M                                        0x00000008
#define CPU_MTB_PID2_JEDEC_S                                                 3

// Field:   [2:0] DES_1
//
// reads as 0b011
#define CPU_MTB_PID2_DES_1_W                                                 3
#define CPU_MTB_PID2_DES_1_M                                        0x00000007
#define CPU_MTB_PID2_DES_1_S                                                 0

//*****************************************************************************
//
// Register: CPU_MTB_O_PID3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// reads as 0x0
#define CPU_MTB_PID3_REVAND_W                                                4
#define CPU_MTB_PID3_REVAND_M                                       0x000000F0
#define CPU_MTB_PID3_REVAND_S                                                4

// Field:   [3:0] CMOD
//
// reads as 0x0
#define CPU_MTB_PID3_CMOD_W                                                  4
#define CPU_MTB_PID3_CMOD_M                                         0x0000000F
#define CPU_MTB_PID3_CMOD_S                                                  0

//*****************************************************************************
//
// Register: CPU_MTB_O_CID0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// reads as 0x0D
#define CPU_MTB_CID0_PRMBL_0_W                                               8
#define CPU_MTB_CID0_PRMBL_0_M                                      0x000000FF
#define CPU_MTB_CID0_PRMBL_0_S                                               0

//*****************************************************************************
//
// Register: CPU_MTB_O_CID1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// reads as 0x9
#define CPU_MTB_CID1_CLASS_W                                                 4
#define CPU_MTB_CID1_CLASS_M                                        0x000000F0
#define CPU_MTB_CID1_CLASS_S                                                 4

// Field:   [3:0] PRMBL_1
//
// reads as 0x0
#define CPU_MTB_CID1_PRMBL_1_W                                               4
#define CPU_MTB_CID1_PRMBL_1_M                                      0x0000000F
#define CPU_MTB_CID1_PRMBL_1_S                                               0

//*****************************************************************************
//
// Register: CPU_MTB_O_CID2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// reads as 0x05
#define CPU_MTB_CID2_PRMBL_2_W                                               8
#define CPU_MTB_CID2_PRMBL_2_M                                      0x000000FF
#define CPU_MTB_CID2_PRMBL_2_S                                               0

//*****************************************************************************
//
// Register: CPU_MTB_O_CID3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// reads as 0xB1
#define CPU_MTB_CID3_PRMBL_3_W                                               8
#define CPU_MTB_CID3_PRMBL_3_M                                      0x000000FF
#define CPU_MTB_CID3_PRMBL_3_S                                               0


#endif // __CPU_MTB__
