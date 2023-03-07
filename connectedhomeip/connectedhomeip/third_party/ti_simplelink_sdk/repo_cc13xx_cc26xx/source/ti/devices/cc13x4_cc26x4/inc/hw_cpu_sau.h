/******************************************************************************
*  Filename:       hw_cpu_sau_h
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

#ifndef __HW_CPU_SAU_H__
#define __HW_CPU_SAU_H__

//*****************************************************************************
//
// This section defines the register offsets of
// CPU_SAU component
//
//*****************************************************************************
// Allows enabling of the Security Attribution Unit
#define CPU_SAU_O_CTRL                                              0x00000000

// Indicates the number of regions implemented by the Security Attribution Unit
#define CPU_SAU_O_TYPE                                              0x00000004

// Selects the region currently accessed by SAU_RBAR and SAU_RLAR
#define CPU_SAU_O_RNR                                               0x00000008

// Provides indirect read and write access to the base address of the currently
// selected SAU region
#define CPU_SAU_O_RBAR                                              0x0000000C

// Provides indirect read and write access to the limit address of the
// currently selected SAU region
#define CPU_SAU_O_RLAR                                              0x00000010

// Provides information about any security related faults
#define CPU_SAU_O_SFSR                                              0x00000014

// Shows the address of the memory location that caused a Security violation
#define CPU_SAU_O_SFAR                                              0x00000018

//*****************************************************************************
//
// Register: CPU_SAU_O_CTRL
//
//*****************************************************************************
// Field:     [1] ALLNS
//
// When SAU_CTRL.ENABLE is 0 this bit controls if the memory is marked as
// Non-secure or Secure
#define CPU_SAU_CTRL_ALLNS                                          0x00000002
#define CPU_SAU_CTRL_ALLNS_BITN                                              1
#define CPU_SAU_CTRL_ALLNS_M                                        0x00000002
#define CPU_SAU_CTRL_ALLNS_S                                                 1

// Field:     [0] ENABLE
//
// Enables the SAU
#define CPU_SAU_CTRL_ENABLE                                         0x00000001
#define CPU_SAU_CTRL_ENABLE_BITN                                             0
#define CPU_SAU_CTRL_ENABLE_M                                       0x00000001
#define CPU_SAU_CTRL_ENABLE_S                                                0

//*****************************************************************************
//
// Register: CPU_SAU_O_TYPE
//
//*****************************************************************************
// Field:   [7:0] SREGION
//
// The number of implemented SAU regions
#define CPU_SAU_TYPE_SREGION_W                                               8
#define CPU_SAU_TYPE_SREGION_M                                      0x000000FF
#define CPU_SAU_TYPE_SREGION_S                                               0

//*****************************************************************************
//
// Register: CPU_SAU_O_RNR
//
//*****************************************************************************
// Field:   [7:0] REGION
//
// Indicates the SAU region accessed by SAU_RBAR and SAU_RLAR
#define CPU_SAU_RNR_REGION_W                                                 8
#define CPU_SAU_RNR_REGION_M                                        0x000000FF
#define CPU_SAU_RNR_REGION_S                                                 0

//*****************************************************************************
//
// Register: CPU_SAU_O_RBAR
//
//*****************************************************************************
// Field:  [31:5] BADDR
//
// Holds bits [31:5] of the base address for the selected SAU region
#define CPU_SAU_RBAR_BADDR_W                                                27
#define CPU_SAU_RBAR_BADDR_M                                        0xFFFFFFE0
#define CPU_SAU_RBAR_BADDR_S                                                 5

//*****************************************************************************
//
// Register: CPU_SAU_O_RLAR
//
//*****************************************************************************
// Field:  [31:5] LADDR
//
// Holds bits [31:5] of the limit address for the selected SAU region
#define CPU_SAU_RLAR_LADDR_W                                                27
#define CPU_SAU_RLAR_LADDR_M                                        0xFFFFFFE0
#define CPU_SAU_RLAR_LADDR_S                                                 5

// Field:     [1] NSC
//
// Controls whether Non-secure state is permitted to execute an SG instruction
// from this region
#define CPU_SAU_RLAR_NSC                                            0x00000002
#define CPU_SAU_RLAR_NSC_BITN                                                1
#define CPU_SAU_RLAR_NSC_M                                          0x00000002
#define CPU_SAU_RLAR_NSC_S                                                   1

// Field:     [0] ENABLE
//
// SAU region enable
#define CPU_SAU_RLAR_ENABLE                                         0x00000001
#define CPU_SAU_RLAR_ENABLE_BITN                                             0
#define CPU_SAU_RLAR_ENABLE_M                                       0x00000001
#define CPU_SAU_RLAR_ENABLE_S                                                0

//*****************************************************************************
//
// Register: CPU_SAU_O_SFSR
//
//*****************************************************************************
// Field:     [7] LSERR
//
// Sticky flag indicating that an error occurred during lazy state activation
// or deactivation
#define CPU_SAU_SFSR_LSERR                                          0x00000080
#define CPU_SAU_SFSR_LSERR_BITN                                              7
#define CPU_SAU_SFSR_LSERR_M                                        0x00000080
#define CPU_SAU_SFSR_LSERR_S                                                 7

// Field:     [6] SFARVALID
//
// This bit is set when the SFAR register contains a valid value. As with
// similar fields, such as BFSR.BFARVALID and MMFSR.MMARVALID, this bit can be
// cleared by other exceptions, such as BusFault
#define CPU_SAU_SFSR_SFARVALID                                      0x00000040
#define CPU_SAU_SFSR_SFARVALID_BITN                                          6
#define CPU_SAU_SFSR_SFARVALID_M                                    0x00000040
#define CPU_SAU_SFSR_SFARVALID_S                                             6

// Field:     [5] LSPERR
//
// Stick flag indicating that an SAU or IDAU violation occurred during the lazy
// preservation of floating-point state
#define CPU_SAU_SFSR_LSPERR                                         0x00000020
#define CPU_SAU_SFSR_LSPERR_BITN                                             5
#define CPU_SAU_SFSR_LSPERR_M                                       0x00000020
#define CPU_SAU_SFSR_LSPERR_S                                                5

// Field:     [4] INVTRAN
//
// Sticky flag indicating that an exception was raised due to a branch that was
// not flagged as being domain crossing causing a transition from Secure to
// Non-secure memory
#define CPU_SAU_SFSR_INVTRAN                                        0x00000010
#define CPU_SAU_SFSR_INVTRAN_BITN                                            4
#define CPU_SAU_SFSR_INVTRAN_M                                      0x00000010
#define CPU_SAU_SFSR_INVTRAN_S                                               4

// Field:     [3] AUVIOL
//
// Sticky flag indicating that an attempt was made to access parts of the
// address space that are marked as Secure with NS-Req for the transaction set
// to Non-secure. This bit is not set if the violation occurred during lazy
// state preservation. See LSPERR
#define CPU_SAU_SFSR_AUVIOL                                         0x00000008
#define CPU_SAU_SFSR_AUVIOL_BITN                                             3
#define CPU_SAU_SFSR_AUVIOL_M                                       0x00000008
#define CPU_SAU_SFSR_AUVIOL_S                                                3

// Field:     [2] INVER
//
// This can be caused by EXC_RETURN.DCRS being set to 0 when returning from an
// exception in the Non-secure state, or by EXC_RETURN.ES being set to 1 when
// returning from an exception in the Non-secure state
#define CPU_SAU_SFSR_INVER                                          0x00000004
#define CPU_SAU_SFSR_INVER_BITN                                              2
#define CPU_SAU_SFSR_INVER_M                                        0x00000004
#define CPU_SAU_SFSR_INVER_S                                                 2

// Field:     [1] INVIS
//
// This bit is set if the integrity signature in an exception stack frame is
// found to be invalid during the unstacking operation
#define CPU_SAU_SFSR_INVIS                                          0x00000002
#define CPU_SAU_SFSR_INVIS_BITN                                              1
#define CPU_SAU_SFSR_INVIS_M                                        0x00000002
#define CPU_SAU_SFSR_INVIS_S                                                 1

// Field:     [0] INVEP
//
// This bit is set if a function call from the Non-secure state or exception
// targets a non-SG instruction in the Secure state. This bit is also set if
// the target address is a SG instruction, but there is no matching SAU/IDAU
// region with the NSC flag set
#define CPU_SAU_SFSR_INVEP                                          0x00000001
#define CPU_SAU_SFSR_INVEP_BITN                                              0
#define CPU_SAU_SFSR_INVEP_M                                        0x00000001
#define CPU_SAU_SFSR_INVEP_S                                                 0

//*****************************************************************************
//
// Register: CPU_SAU_O_SFAR
//
//*****************************************************************************
// Field:  [31:0] ADDRESS
//
// The address of an access that caused a attribution unit violation. This
// field is only valid when SFSR.SFARVALID is set. This allows the actual flip
// flops associated with this register to be shared with other fault address
// registers. If an implementation chooses to share the storage in this way,
// care must be taken to not leak Secure address information to the Non-secure
// state. One way of achieving this is to share the SFAR register with the
// MMFAR_S register, which is not accessible to the Non-secure state
#define CPU_SAU_SFAR_ADDRESS_W                                              32
#define CPU_SAU_SFAR_ADDRESS_M                                      0xFFFFFFFF
#define CPU_SAU_SFAR_ADDRESS_S                                               0


#endif // __CPU_SAU__
