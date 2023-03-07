/******************************************************************************
*  Filename:       hw_cpu_fpb_h
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

#ifndef __HW_CPU_FPB_H__
#define __HW_CPU_FPB_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_FPB component
//
//*****************************************************************************
// Provides FPB implementation information, and the global enable for the FPB
// unit
#define CPU_FPB_O_CTRL                                              0x00000000

// Indicates whether the implementation supports Flash Patch remap and, if it
// does, holds the target address for remap
#define CPU_FPB_O_REMAP                                             0x00000004

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP0                                             0x00000008

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP1                                             0x0000000C

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP2                                             0x00000010

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP3                                             0x00000014

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP4                                             0x00000018

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP5                                             0x0000001C

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP6                                             0x00000020

// Holds an address for comparison. The effect of the match depends on the
// configuration of the FPB and whether the comparator is an instruction
// address comparator or a literal address comparator
#define CPU_FPB_O_COMP7                                             0x00000024

// Provides CoreSight discovery information for the FPB
#define CPU_FPB_O_DEVARCH                                           0x00000FBC

// Provides CoreSight discovery information for the FPB
#define CPU_FPB_O_DEVTYPE                                           0x00000FCC

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR4                                             0x00000FD0

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR5                                             0x00000FD4

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR6                                             0x00000FD8

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR7                                             0x00000FDC

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR0                                             0x00000FE0

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR1                                             0x00000FE4

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR2                                             0x00000FE8

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_PIDR3                                             0x00000FEC

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_CIDR0                                             0x00000FF0

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_CIDR1                                             0x00000FF4

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_CIDR2                                             0x00000FF8

// Provides CoreSight discovery information for the FP
#define CPU_FPB_O_CIDR3                                             0x00000FFC

//*****************************************************************************
//
// Register: CPU_FPB_O_CTRL
//
//*****************************************************************************
// Field: [31:28] REV
//
// Flash Patch and Breakpoint Unit architecture revision
#define CPU_FPB_CTRL_REV_W                                                   4
#define CPU_FPB_CTRL_REV_M                                          0xF0000000
#define CPU_FPB_CTRL_REV_S                                                  28

// Field: [14:12] NUM_CODE_14_12_
//
// Indicates the number of implemented instruction address comparators. Zero
// indicates no Instruction Address comparators are implemented. The
// Instruction Address comparators are numbered from 0 to NUM_CODE - 1
#define CPU_FPB_CTRL_NUM_CODE_14_12__W                                       3
#define CPU_FPB_CTRL_NUM_CODE_14_12__M                              0x00007000
#define CPU_FPB_CTRL_NUM_CODE_14_12__S                                      12

// Field:  [11:8] NUM_LIT
//
// Indicates the number of implemented literal address comparators. The Literal
// Address comparators are numbered from NUM_CODE to NUM_CODE + NUM_LIT - 1
#define CPU_FPB_CTRL_NUM_LIT_W                                               4
#define CPU_FPB_CTRL_NUM_LIT_M                                      0x00000F00
#define CPU_FPB_CTRL_NUM_LIT_S                                               8

// Field:   [7:4] NUM_CODE_7_4_
//
// Indicates the number of implemented instruction address comparators. Zero
// indicates no Instruction Address comparators are implemented. The
// Instruction Address comparators are numbered from 0 to NUM_CODE - 1
#define CPU_FPB_CTRL_NUM_CODE_7_4__W                                         4
#define CPU_FPB_CTRL_NUM_CODE_7_4__M                                0x000000F0
#define CPU_FPB_CTRL_NUM_CODE_7_4__S                                         4

// Field:     [1] KEY
//
// Writes to the FP_CTRL are ignored unless KEY is concurrently written to one
#define CPU_FPB_CTRL_KEY                                            0x00000002
#define CPU_FPB_CTRL_KEY_BITN                                                1
#define CPU_FPB_CTRL_KEY_M                                          0x00000002
#define CPU_FPB_CTRL_KEY_S                                                   1

// Field:     [0] ENABLE
//
// Enables the FPB
#define CPU_FPB_CTRL_ENABLE                                         0x00000001
#define CPU_FPB_CTRL_ENABLE_BITN                                             0
#define CPU_FPB_CTRL_ENABLE_M                                       0x00000001
#define CPU_FPB_CTRL_ENABLE_S                                                0

//*****************************************************************************
//
// Register: CPU_FPB_O_REMAP
//
//*****************************************************************************
// Field:    [29] RMPSPT
//
// Indicates whether the FPB unit supports the Flash Patch remap function
#define CPU_FPB_REMAP_RMPSPT                                        0x20000000
#define CPU_FPB_REMAP_RMPSPT_BITN                                           29
#define CPU_FPB_REMAP_RMPSPT_M                                      0x20000000
#define CPU_FPB_REMAP_RMPSPT_S                                              29

// Field:  [28:5] REMAP
//
// Holds the bits[28:5] of the Flash Patch remap address
#define CPU_FPB_REMAP_REMAP_W                                               24
#define CPU_FPB_REMAP_REMAP_M                                       0x1FFFFFE0
#define CPU_FPB_REMAP_REMAP_S                                                5

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP0
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP0_BE                                            0x00000001
#define CPU_FPB_COMP0_BE_BITN                                                0
#define CPU_FPB_COMP0_BE_M                                          0x00000001
#define CPU_FPB_COMP0_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP1
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP1_BE                                            0x00000001
#define CPU_FPB_COMP1_BE_BITN                                                0
#define CPU_FPB_COMP1_BE_M                                          0x00000001
#define CPU_FPB_COMP1_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP2
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP2_BE                                            0x00000001
#define CPU_FPB_COMP2_BE_BITN                                                0
#define CPU_FPB_COMP2_BE_M                                          0x00000001
#define CPU_FPB_COMP2_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP3
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP3_BE                                            0x00000001
#define CPU_FPB_COMP3_BE_BITN                                                0
#define CPU_FPB_COMP3_BE_M                                          0x00000001
#define CPU_FPB_COMP3_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP4
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP4_BE                                            0x00000001
#define CPU_FPB_COMP4_BE_BITN                                                0
#define CPU_FPB_COMP4_BE_M                                          0x00000001
#define CPU_FPB_COMP4_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP5
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP5_BE                                            0x00000001
#define CPU_FPB_COMP5_BE_BITN                                                0
#define CPU_FPB_COMP5_BE_M                                          0x00000001
#define CPU_FPB_COMP5_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP6
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP6_BE                                            0x00000001
#define CPU_FPB_COMP6_BE_BITN                                                0
#define CPU_FPB_COMP6_BE_M                                          0x00000001
#define CPU_FPB_COMP6_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_COMP7
//
//*****************************************************************************
// Field:     [0] BE
//
// Selects between flashpatch and breakpoint functionality
#define CPU_FPB_COMP7_BE                                            0x00000001
#define CPU_FPB_COMP7_BE_BITN                                                0
#define CPU_FPB_COMP7_BE_M                                          0x00000001
#define CPU_FPB_COMP7_BE_S                                                   0

//*****************************************************************************
//
// Register: CPU_FPB_O_DEVARCH
//
//*****************************************************************************
// Field: [31:21] ARCHITECT
//
// Defines the architect of the component. Bits [31:28] are the JEP106
// continuation code (JEP106 bank ID, minus 1) and bits [27:21] are the JEP106
// ID code.
#define CPU_FPB_DEVARCH_ARCHITECT_W                                         11
#define CPU_FPB_DEVARCH_ARCHITECT_M                                 0xFFE00000
#define CPU_FPB_DEVARCH_ARCHITECT_S                                         21

// Field:    [20] PRESENT
//
// Defines that the DEVARCH register is present
#define CPU_FPB_DEVARCH_PRESENT                                     0x00100000
#define CPU_FPB_DEVARCH_PRESENT_BITN                                        20
#define CPU_FPB_DEVARCH_PRESENT_M                                   0x00100000
#define CPU_FPB_DEVARCH_PRESENT_S                                           20

// Field: [19:16] REVISION
//
// Defines the architecture revision of the component
#define CPU_FPB_DEVARCH_REVISION_W                                           4
#define CPU_FPB_DEVARCH_REVISION_M                                  0x000F0000
#define CPU_FPB_DEVARCH_REVISION_S                                          16

// Field: [15:12] ARCHVER
//
// Defines the architecture version of the component
#define CPU_FPB_DEVARCH_ARCHVER_W                                            4
#define CPU_FPB_DEVARCH_ARCHVER_M                                   0x0000F000
#define CPU_FPB_DEVARCH_ARCHVER_S                                           12

// Field:  [11:0] ARCHPART
//
// Defines the architecture of the component
#define CPU_FPB_DEVARCH_ARCHPART_W                                          12
#define CPU_FPB_DEVARCH_ARCHPART_M                                  0x00000FFF
#define CPU_FPB_DEVARCH_ARCHPART_S                                           0

//*****************************************************************************
//
// Register: CPU_FPB_O_DEVTYPE
//
//*****************************************************************************
// Field:   [7:4] SUB
//
// Component sub-type
#define CPU_FPB_DEVTYPE_SUB_W                                                4
#define CPU_FPB_DEVTYPE_SUB_M                                       0x000000F0
#define CPU_FPB_DEVTYPE_SUB_S                                                4

// Field:   [3:0] MAJOR
//
// Component major type
#define CPU_FPB_DEVTYPE_MAJOR_W                                              4
#define CPU_FPB_DEVTYPE_MAJOR_M                                     0x0000000F
#define CPU_FPB_DEVTYPE_MAJOR_S                                              0

//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR4
//
//*****************************************************************************
// Field:   [7:4] SIZE
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR4_SIZE_W                                                 4
#define CPU_FPB_PIDR4_SIZE_M                                        0x000000F0
#define CPU_FPB_PIDR4_SIZE_S                                                 4

// Field:   [3:0] DES_2
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR4_DES_2_W                                                4
#define CPU_FPB_PIDR4_DES_2_M                                       0x0000000F
#define CPU_FPB_PIDR4_DES_2_S                                                0

//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR5
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR6
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR7
//
//*****************************************************************************
//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR0
//
//*****************************************************************************
// Field:   [7:0] PART_0
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR0_PART_0_W                                               8
#define CPU_FPB_PIDR0_PART_0_M                                      0x000000FF
#define CPU_FPB_PIDR0_PART_0_S                                               0

//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR1
//
//*****************************************************************************
// Field:   [7:4] DES_0
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR1_DES_0_W                                                4
#define CPU_FPB_PIDR1_DES_0_M                                       0x000000F0
#define CPU_FPB_PIDR1_DES_0_S                                                4

// Field:   [3:0] PART_1
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR1_PART_1_W                                               4
#define CPU_FPB_PIDR1_PART_1_M                                      0x0000000F
#define CPU_FPB_PIDR1_PART_1_S                                               0

//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR2
//
//*****************************************************************************
// Field:   [7:4] REVISION
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR2_REVISION_W                                             4
#define CPU_FPB_PIDR2_REVISION_M                                    0x000000F0
#define CPU_FPB_PIDR2_REVISION_S                                             4

// Field:     [3] JEDEC
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR2_JEDEC                                         0x00000008
#define CPU_FPB_PIDR2_JEDEC_BITN                                             3
#define CPU_FPB_PIDR2_JEDEC_M                                       0x00000008
#define CPU_FPB_PIDR2_JEDEC_S                                                3

// Field:   [2:0] DES_1
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR2_DES_1_W                                                3
#define CPU_FPB_PIDR2_DES_1_M                                       0x00000007
#define CPU_FPB_PIDR2_DES_1_S                                                0

//*****************************************************************************
//
// Register: CPU_FPB_O_PIDR3
//
//*****************************************************************************
// Field:   [7:4] REVAND
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR3_REVAND_W                                               4
#define CPU_FPB_PIDR3_REVAND_M                                      0x000000F0
#define CPU_FPB_PIDR3_REVAND_S                                               4

// Field:   [3:0] CMOD
//
// See CoreSight Architecture Specification
#define CPU_FPB_PIDR3_CMOD_W                                                 4
#define CPU_FPB_PIDR3_CMOD_M                                        0x0000000F
#define CPU_FPB_PIDR3_CMOD_S                                                 0

//*****************************************************************************
//
// Register: CPU_FPB_O_CIDR0
//
//*****************************************************************************
// Field:   [7:0] PRMBL_0
//
// See CoreSight Architecture Specification
#define CPU_FPB_CIDR0_PRMBL_0_W                                              8
#define CPU_FPB_CIDR0_PRMBL_0_M                                     0x000000FF
#define CPU_FPB_CIDR0_PRMBL_0_S                                              0

//*****************************************************************************
//
// Register: CPU_FPB_O_CIDR1
//
//*****************************************************************************
// Field:   [7:4] CLASS
//
// See CoreSight Architecture Specification
#define CPU_FPB_CIDR1_CLASS_W                                                4
#define CPU_FPB_CIDR1_CLASS_M                                       0x000000F0
#define CPU_FPB_CIDR1_CLASS_S                                                4

// Field:   [3:0] PRMBL_1
//
// See CoreSight Architecture Specification
#define CPU_FPB_CIDR1_PRMBL_1_W                                              4
#define CPU_FPB_CIDR1_PRMBL_1_M                                     0x0000000F
#define CPU_FPB_CIDR1_PRMBL_1_S                                              0

//*****************************************************************************
//
// Register: CPU_FPB_O_CIDR2
//
//*****************************************************************************
// Field:   [7:0] PRMBL_2
//
// See CoreSight Architecture Specification
#define CPU_FPB_CIDR2_PRMBL_2_W                                              8
#define CPU_FPB_CIDR2_PRMBL_2_M                                     0x000000FF
#define CPU_FPB_CIDR2_PRMBL_2_S                                              0

//*****************************************************************************
//
// Register: CPU_FPB_O_CIDR3
//
//*****************************************************************************
// Field:   [7:0] PRMBL_3
//
// See CoreSight Architecture Specification
#define CPU_FPB_CIDR3_PRMBL_3_W                                              8
#define CPU_FPB_CIDR3_PRMBL_3_M                                     0x000000FF
#define CPU_FPB_CIDR3_PRMBL_3_S                                              0


#endif // __CPU_FPB__
