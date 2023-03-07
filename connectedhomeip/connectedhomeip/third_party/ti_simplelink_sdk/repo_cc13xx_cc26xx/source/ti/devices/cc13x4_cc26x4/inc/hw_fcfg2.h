/******************************************************************************
*  Filename:       hw_fcfg2_h
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

#ifndef __HW_FCFG2_H__
#define __HW_FCFG2_H__

//*****************************************************************************
//
// This section defines the register offsets of
// FCFG2 component
//
//*****************************************************************************
// Configuration register for bank0 trims
#define FCFG2_O_B0_TRIM_CFG_3                                       0x00000000

// Configuration register for bank0 trims
#define FCFG2_O_B0_TRIM_CFG_2                                       0x00000004

// Configuration register for bank0 trims
#define FCFG2_O_B0_TRIM_CFG_1                                       0x00000008

// Configuration register for bank0 trims
#define FCFG2_O_B0_TRIM_CFG_0                                       0x0000000C

// Configuration register for bank1 trims
#define FCFG2_O_B1_TRIM_CFG_3                                       0x00000010

// Configuration register for bank1 trims
#define FCFG2_O_B1_TRIM_CFG_2                                       0x00000014

// Configuration register for bank1 trims
#define FCFG2_O_B1_TRIM_CFG_1                                       0x00000018

// Configuration register for bank1 trims
#define FCFG2_O_B1_TRIM_CFG_0                                       0x0000001C

// Configuration register for flash pump trims
#define FCFG2_O_PMP_TRIM_CFG_2                                      0x00000020

// Configuration register for flash pump trims
#define FCFG2_O_PMP_TRIM_CFG_1                                      0x00000024

// Configuration register for flash pump trims
#define FCFG2_O_PMP_TRIM_CFG_0                                      0x00000028

// Configuration register for flash read wait cycles
#define FCFG2_O_RD_WAIT_CFG                                         0x0000002C

//*****************************************************************************
//
// Register: FCFG2_O_B0_TRIM_CFG_3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FCFG2_O_B0_TRIM_CFG_2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FCFG2_O_B0_TRIM_CFG_1
//
//*****************************************************************************
// Field: [27:22] REDSWSELW3
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWSELW3 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW3_W                                     6
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW3_M                            0x0FC00000
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW3_S                                    22

// Field: [21:16] REDSWSELW2
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWSELW2 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW2_W                                     6
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW2_M                            0x003F0000
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW2_S                                    16

// Field: [15:10] REDSWSELW1
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWSELW1 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW1_W                                     6
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW1_M                            0x0000FC00
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW1_S                                    10

// Field:   [9:4] REDSWSELW0
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWSELW0 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW0_W                                     6
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW0_M                            0x000003F0
#define FCFG2_B0_TRIM_CFG_1_REDSWSELW0_S                                     4

// Field:     [3] REDSWENW3
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWENW3 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWENW3                               0x00000008
#define FCFG2_B0_TRIM_CFG_1_REDSWENW3_BITN                                   3
#define FCFG2_B0_TRIM_CFG_1_REDSWENW3_M                             0x00000008
#define FCFG2_B0_TRIM_CFG_1_REDSWENW3_S                                      3

// Field:     [2] REDSWENW2
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWENW2 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWENW2                               0x00000004
#define FCFG2_B0_TRIM_CFG_1_REDSWENW2_BITN                                   2
#define FCFG2_B0_TRIM_CFG_1_REDSWENW2_M                             0x00000004
#define FCFG2_B0_TRIM_CFG_1_REDSWENW2_S                                      2

// Field:     [1] REDSWENW1
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWENW1 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWENW1                               0x00000002
#define FCFG2_B0_TRIM_CFG_1_REDSWENW1_BITN                                   1
#define FCFG2_B0_TRIM_CFG_1_REDSWENW1_M                             0x00000002
#define FCFG2_B0_TRIM_CFG_1_REDSWENW1_S                                      1

// Field:     [0] REDSWENW0
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_1.REDSWENW0 by ROM boot
// FW
#define FCFG2_B0_TRIM_CFG_1_REDSWENW0                               0x00000001
#define FCFG2_B0_TRIM_CFG_1_REDSWENW0_BITN                                   0
#define FCFG2_B0_TRIM_CFG_1_REDSWENW0_M                             0x00000001
#define FCFG2_B0_TRIM_CFG_1_REDSWENW0_S                                      0

//*****************************************************************************
//
// Register: FCFG2_O_B0_TRIM_CFG_0
//
//*****************************************************************************
// Field:  [31:0] BANK0_TRIM_CFG_0
//
// Configuration value written to FLASH:BANK0_TRIM_CFG_0.BANK0_TRIM_CFG_0 by
// ROM boot FW
#define FCFG2_B0_TRIM_CFG_0_BANK0_TRIM_CFG_0_W                              32
#define FCFG2_B0_TRIM_CFG_0_BANK0_TRIM_CFG_0_M                      0xFFFFFFFF
#define FCFG2_B0_TRIM_CFG_0_BANK0_TRIM_CFG_0_S                               0

//*****************************************************************************
//
// Register: FCFG2_O_B1_TRIM_CFG_3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FCFG2_O_B1_TRIM_CFG_2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FCFG2_O_B1_TRIM_CFG_1
//
//*****************************************************************************
// Field: [27:22] REDSWSELW3
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWSELW3 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW3_W                                     6
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW3_M                            0x0FC00000
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW3_S                                    22

// Field: [21:16] REDSWSELW2
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWSELW2 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW2_W                                     6
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW2_M                            0x003F0000
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW2_S                                    16

// Field: [15:10] REDSWSELW1
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWSELW1 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW1_W                                     6
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW1_M                            0x0000FC00
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW1_S                                    10

// Field:   [9:4] REDSWSELW0
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWSELW0 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW0_W                                     6
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW0_M                            0x000003F0
#define FCFG2_B1_TRIM_CFG_1_REDSWSELW0_S                                     4

// Field:     [3] REDSWENW3
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWENW3 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWENW3                               0x00000008
#define FCFG2_B1_TRIM_CFG_1_REDSWENW3_BITN                                   3
#define FCFG2_B1_TRIM_CFG_1_REDSWENW3_M                             0x00000008
#define FCFG2_B1_TRIM_CFG_1_REDSWENW3_S                                      3

// Field:     [2] REDSWENW2
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWENW2 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWENW2                               0x00000004
#define FCFG2_B1_TRIM_CFG_1_REDSWENW2_BITN                                   2
#define FCFG2_B1_TRIM_CFG_1_REDSWENW2_M                             0x00000004
#define FCFG2_B1_TRIM_CFG_1_REDSWENW2_S                                      2

// Field:     [1] REDSWENW1
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWENW1 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWENW1                               0x00000002
#define FCFG2_B1_TRIM_CFG_1_REDSWENW1_BITN                                   1
#define FCFG2_B1_TRIM_CFG_1_REDSWENW1_M                             0x00000002
#define FCFG2_B1_TRIM_CFG_1_REDSWENW1_S                                      1

// Field:     [0] REDSWENW0
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_1.REDSWENW0 by ROM boot
// FW
#define FCFG2_B1_TRIM_CFG_1_REDSWENW0                               0x00000001
#define FCFG2_B1_TRIM_CFG_1_REDSWENW0_BITN                                   0
#define FCFG2_B1_TRIM_CFG_1_REDSWENW0_M                             0x00000001
#define FCFG2_B1_TRIM_CFG_1_REDSWENW0_S                                      0

//*****************************************************************************
//
// Register: FCFG2_O_B1_TRIM_CFG_0
//
//*****************************************************************************
// Field:  [31:0] BANK1_TRIM_CFG_0
//
// Configuration value written to FLASH:BANK1_TRIM_CFG_0.BANK1_TRIM_CFG_0 by
// ROM boot FW
#define FCFG2_B1_TRIM_CFG_0_BANK1_TRIM_CFG_0_W                              32
#define FCFG2_B1_TRIM_CFG_0_BANK1_TRIM_CFG_0_M                      0xFFFFFFFF
#define FCFG2_B1_TRIM_CFG_0_BANK1_TRIM_CFG_0_S                               0

//*****************************************************************************
//
// Register: FCFG2_O_PMP_TRIM_CFG_2
//
//*****************************************************************************
// Field: [25:20] VWLCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_2.VWLCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_2_VWLCT_W                                         6
#define FCFG2_PMP_TRIM_CFG_2_VWLCT_M                                0x03F00000
#define FCFG2_PMP_TRIM_CFG_2_VWLCT_S                                        20

// Field: [19:14] VSLCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_2.VSLCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_2_VSLCT_W                                         6
#define FCFG2_PMP_TRIM_CFG_2_VSLCT_M                                0x000FC000
#define FCFG2_PMP_TRIM_CFG_2_VSLCT_S                                        14

// Field:  [13:9] VREADCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_2.VREADCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_2_VREADCT_W                                       5
#define FCFG2_PMP_TRIM_CFG_2_VREADCT_M                              0x00003E00
#define FCFG2_PMP_TRIM_CFG_2_VREADCT_S                                       9

// Field:   [8:4] VINLOWCCORCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_2.VINLOWCCORCT by ROM
// boot FW
#define FCFG2_PMP_TRIM_CFG_2_VINLOWCCORCT_W                                  5
#define FCFG2_PMP_TRIM_CFG_2_VINLOWCCORCT_M                         0x000001F0
#define FCFG2_PMP_TRIM_CFG_2_VINLOWCCORCT_S                                  4

// Field:   [3:0] VINHICCORCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_2.VINHICCORCT by ROM boot
// FW
#define FCFG2_PMP_TRIM_CFG_2_VINHICCORCT_W                                   4
#define FCFG2_PMP_TRIM_CFG_2_VINHICCORCT_M                          0x0000000F
#define FCFG2_PMP_TRIM_CFG_2_VINHICCORCT_S                                   0

//*****************************************************************************
//
// Register: FCFG2_O_PMP_TRIM_CFG_1
//
//*****************************************************************************
// Field:    [31] VINHICCORCTLSB
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.VINHICCORCTLSB by ROM
// boot FW
#define FCFG2_PMP_TRIM_CFG_1_VINHICCORCTLSB                         0x80000000
#define FCFG2_PMP_TRIM_CFG_1_VINHICCORCTLSB_BITN                            31
#define FCFG2_PMP_TRIM_CFG_1_VINHICCORCTLSB_M                       0x80000000
#define FCFG2_PMP_TRIM_CFG_1_VINHICCORCTLSB_S                               31

// Field: [30:25] VINHCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.VINHCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_1_VINHCT_W                                        6
#define FCFG2_PMP_TRIM_CFG_1_VINHCT_M                               0x7E000000
#define FCFG2_PMP_TRIM_CFG_1_VINHCT_S                                       25

// Field: [24:20] VCGCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.VCGCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_1_VCGCT_W                                         5
#define FCFG2_PMP_TRIM_CFG_1_VCGCT_M                                0x01F00000
#define FCFG2_PMP_TRIM_CFG_1_VCGCT_S                                        20

// Field: [19:15] IREFVRDCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.IREFVRDCT by ROM boot
// FW
#define FCFG2_PMP_TRIM_CFG_1_IREFVRDCT_W                                     5
#define FCFG2_PMP_TRIM_CFG_1_IREFVRDCT_M                            0x000F8000
#define FCFG2_PMP_TRIM_CFG_1_IREFVRDCT_S                                    15

// Field: [14:10] IREFTCCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.IREFTCCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_1_IREFTCCT_W                                      5
#define FCFG2_PMP_TRIM_CFG_1_IREFTCCT_M                             0x00007C00
#define FCFG2_PMP_TRIM_CFG_1_IREFTCCT_S                                     10

// Field:   [9:6] IREFCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.IREFCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_1_IREFCT_W                                        4
#define FCFG2_PMP_TRIM_CFG_1_IREFCT_M                               0x000003C0
#define FCFG2_PMP_TRIM_CFG_1_IREFCT_S                                        6

// Field:   [5:0] FOSCCT
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_1.FOSCCT by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_1_FOSCCT_W                                        6
#define FCFG2_PMP_TRIM_CFG_1_FOSCCT_M                               0x0000003F
#define FCFG2_PMP_TRIM_CFG_1_FOSCCT_S                                        0

//*****************************************************************************
//
// Register: FCFG2_O_PMP_TRIM_CFG_0
//
//*****************************************************************************
// Field: [29:20] VHVCT_PV
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_0.VHVCT_PV by ROM boot FW
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PV_W                                     10
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PV_M                             0x3FF00000
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PV_S                                     20

// Field: [19:10] VHVCT_PGM
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_0.VHVCT_PGM by ROM boot
// FW
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PGM_W                                    10
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PGM_M                            0x000FFC00
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_PGM_S                                    10

// Field:   [9:0] VHVCT_ERS
//
// Configuration value written to FLASH:PUMP_TRIM_CFG_0.VHVCT_ERS by ROM boot
// FW
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_ERS_W                                    10
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_ERS_M                            0x000003FF
#define FCFG2_PMP_TRIM_CFG_0_VHVCT_ERS_S                                     0

//*****************************************************************************
//
// Register: FCFG2_O_RD_WAIT_CFG
//
//*****************************************************************************
// Field:   [7:4] WAIT2T
//
// Configuration value written to FLASH:READWAITCFG.WAIT2T by ROM boot FW
#define FCFG2_RD_WAIT_CFG_WAIT2T_W                                           4
#define FCFG2_RD_WAIT_CFG_WAIT2T_M                                  0x000000F0
#define FCFG2_RD_WAIT_CFG_WAIT2T_S                                           4

// Field:   [3:0] WAIT1T
//
// Configuration value written to FLASH:READWAITCFG.WAIT1T by ROM boot FW
#define FCFG2_RD_WAIT_CFG_WAIT1T_W                                           4
#define FCFG2_RD_WAIT_CFG_WAIT1T_M                                  0x0000000F
#define FCFG2_RD_WAIT_CFG_WAIT1T_S                                           0


#endif // __FCFG2__
