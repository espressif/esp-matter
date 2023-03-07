/******************************************************************************
*  Filename:       hw_cpu_mpu_h
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

#ifndef __HW_CPU_MPU_H__
#define __HW_CPU_MPU_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_MPU component
//
//*****************************************************************************
// Along with MPU_MAIR1, provides the memory attribute encodings corresponding
// to the AttrIndex values
#define CPU_MPU_O_MAIR0                                             0x00000000

// Along with MPU_MAIR0, provides the memory attribute encodings corresponding
// to the AttrIndex values
#define CPU_MPU_O_MAIR1                                             0x00000004

//*****************************************************************************
//
// Register: CPU_MPU_O_MAIR0
//
//*****************************************************************************
// Field: [31:24] Attr3
//
// Memory attribute encoding for MPU regions with an AttrIndex of 3
#define CPU_MPU_MAIR0_ATTR3_W                                                8
#define CPU_MPU_MAIR0_ATTR3_M                                       0xFF000000
#define CPU_MPU_MAIR0_ATTR3_S                                               24

// Field: [23:16] Attr2
//
// Memory attribute encoding for MPU regions with an AttrIndex of 2
#define CPU_MPU_MAIR0_ATTR2_W                                                8
#define CPU_MPU_MAIR0_ATTR2_M                                       0x00FF0000
#define CPU_MPU_MAIR0_ATTR2_S                                               16

// Field:  [15:8] Attr1
//
// Memory attribute encoding for MPU regions with an AttrIndex of 1
#define CPU_MPU_MAIR0_ATTR1_W                                                8
#define CPU_MPU_MAIR0_ATTR1_M                                       0x0000FF00
#define CPU_MPU_MAIR0_ATTR1_S                                                8

// Field:  [31:5] LIMIT
//
// Contains bits [31:5] of the upper inclusive limit of the selected MPU memory
// region. This value is postfixed with 0x1F to provide the limit address to be
// checked against
#define CPU_MPU_MAIR0_LIMIT_W                                               27
#define CPU_MPU_MAIR0_LIMIT_M                                       0xFFFFFFE0
#define CPU_MPU_MAIR0_LIMIT_S                                                5

// Field:   [4:3] SH
//
// Defines the Shareability domain of this region for Normal memory
#define CPU_MPU_MAIR0_SH_W                                                   2
#define CPU_MPU_MAIR0_SH_M                                          0x00000018
#define CPU_MPU_MAIR0_SH_S                                                   3

// Field:     [2] PRIVDEFENA
//
// Controls whether the default memory map is enabled for privileged software
#define CPU_MPU_MAIR0_PRIVDEFENA                                    0x00000004
#define CPU_MPU_MAIR0_PRIVDEFENA_BITN                                        2
#define CPU_MPU_MAIR0_PRIVDEFENA_M                                  0x00000004
#define CPU_MPU_MAIR0_PRIVDEFENA_S                                           2

// Field:   [3:1] AttrIndx
//
// Associates a set of attributes in the MPU_MAIR0 and MPU_MAIR1 fields
#define CPU_MPU_MAIR0_ATTRINDX_W                                             3
#define CPU_MPU_MAIR0_ATTRINDX_M                                    0x0000000E
#define CPU_MPU_MAIR0_ATTRINDX_S                                             1

// Field:   [7:0] Attr0
//
// Memory attribute encoding for MPU regions with an AttrIndex of 0
#define CPU_MPU_MAIR0_ATTR0_W                                                8
#define CPU_MPU_MAIR0_ATTR0_M                                       0x000000FF
#define CPU_MPU_MAIR0_ATTR0_S                                                0

//*****************************************************************************
//
// Register: CPU_MPU_O_MAIR1
//
//*****************************************************************************
// Field: [31:24] Attr7
//
// Memory attribute encoding for MPU regions with an AttrIndex of 7
#define CPU_MPU_MAIR1_ATTR7_W                                                8
#define CPU_MPU_MAIR1_ATTR7_M                                       0xFF000000
#define CPU_MPU_MAIR1_ATTR7_S                                               24

// Field: [23:16] Attr6
//
// Memory attribute encoding for MPU regions with an AttrIndex of 6
#define CPU_MPU_MAIR1_ATTR6_W                                                8
#define CPU_MPU_MAIR1_ATTR6_M                                       0x00FF0000
#define CPU_MPU_MAIR1_ATTR6_S                                               16

// Field:  [15:8] Attr5
//
// Memory attribute encoding for MPU regions with an AttrIndex of 5
#define CPU_MPU_MAIR1_ATTR5_W                                                8
#define CPU_MPU_MAIR1_ATTR5_M                                       0x0000FF00
#define CPU_MPU_MAIR1_ATTR5_S                                                8

// Field:   [7:0] Attr4
//
// Memory attribute encoding for MPU regions with an AttrIndex of 4
#define CPU_MPU_MAIR1_ATTR4_W                                                8
#define CPU_MPU_MAIR1_ATTR4_M                                       0x000000FF
#define CPU_MPU_MAIR1_ATTR4_S                                                0


#endif // __CPU_MPU__
