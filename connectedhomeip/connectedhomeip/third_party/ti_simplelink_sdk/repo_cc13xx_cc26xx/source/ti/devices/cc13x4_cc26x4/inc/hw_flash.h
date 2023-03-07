/******************************************************************************
*  Filename:       hw_flash_h
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

#ifndef __HW_FLASH_H__
#define __HW_FLASH_H__

//*****************************************************************************
//
// This section defines the register offsets of
// FLASH component
//
//*****************************************************************************
// Internal
#define FLASH_O_WEPROT_B0_31_0_BY1                                  0x00000000

// Internal
#define FLASH_O_WEPROT_AUX_BY1                                      0x00000004

// NW and Efuse Status
#define FLASH_O_STAT                                                0x0000001C

// Internal
#define FLASH_O_CFG                                                 0x00000024

// Internal
#define FLASH_O_FLASH_SIZE                                          0x0000002C

// Internal
#define FLASH_O_FWLOCK                                              0x0000003C

// Internal
#define FLASH_O_FWFLAG                                              0x00000040

// Internal
#define FLASH_O_BANK0_TRIM_CFG_3                                    0x00000050

// Internal
#define FLASH_O_BANK0_TRIM_CFG_2                                    0x00000054

// Internal
#define FLASH_O_BANK0_TRIM_CFG_1                                    0x00000058

// Internal
#define FLASH_O_BANK0_TRIM_CFG_0                                    0x0000005C

// Internal
#define FLASH_O_BANK1_TRIM_CFG_3                                    0x00000060

// Internal
#define FLASH_O_BANK1_TRIM_CFG_2                                    0x00000064

// Internal
#define FLASH_O_BANK1_TRIM_CFG_1                                    0x00000068

// Internal
#define FLASH_O_BANK1_TRIM_CFG_0                                    0x0000006C

// Internal
#define FLASH_O_PUMP_TRIM_CFG_2                                     0x00000070

// Internal
#define FLASH_O_PUMP_TRIM_CFG_1                                     0x00000074

// Internal
#define FLASH_O_PUMP_TRIM_CFG_0                                     0x00000078

// Internal
#define FLASH_O_EFUSE                                               0x00001000

// Internal
#define FLASH_O_EFUSEADDR                                           0x00001004

// Internal
#define FLASH_O_DATAUPPER                                           0x00001008

// Internal
#define FLASH_O_DATALOWER                                           0x0000100C

// Internal
#define FLASH_O_EFUSECFG                                            0x00001010

// Internal
#define FLASH_O_EFUSESTAT                                           0x00001014

// Internal
#define FLASH_O_ACC                                                 0x00001018

// Internal
#define FLASH_O_BOUNDARY                                            0x0000101C

// Internal
#define FLASH_O_EFUSEFLAG                                           0x00001020

// Internal
#define FLASH_O_EFUSEKEY                                            0x00001024

// Internal
#define FLASH_O_EFUSERELEASE                                        0x00001028

// Internal
#define FLASH_O_EFUSEPINS                                           0x0000102C

// Internal
#define FLASH_O_EFUSECRA                                            0x00001030

// Internal
#define FLASH_O_EFUSEREAD                                           0x00001034

// Internal
#define FLASH_O_EFUSEPROGRAM                                        0x00001038

// Internal
#define FLASH_O_EFUSEERROR                                          0x0000103C

// Internal
#define FLASH_O_SINGLEBIT                                           0x00001040

// Internal
#define FLASH_O_TWOBIT                                              0x00001044

// Internal
#define FLASH_O_SELFTESTCYC                                         0x00001048

// Internal
#define FLASH_O_SELFTESTSIGN                                        0x0000104C

//*****************************************************************************
//
// Register: FLASH_O_WEPROT_B0_31_0_BY1
//
//*****************************************************************************
// Field:  [31:0] WEPROT_B0_31_0_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_B0_31_0_BY1_WEPROT_B0_31_0_BY1_W                       32
#define FLASH_WEPROT_B0_31_0_BY1_WEPROT_B0_31_0_BY1_M               0xFFFFFFFF
#define FLASH_WEPROT_B0_31_0_BY1_WEPROT_B0_31_0_BY1_S                        0

//*****************************************************************************
//
// Register: FLASH_O_WEPROT_AUX_BY1
//
//*****************************************************************************
// Field:     [5] WEPROT_B1_ENGR_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_ENGR_BY1                     0x00000020
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_ENGR_BY1_BITN                         5
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_ENGR_BY1_M                   0x00000020
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_ENGR_BY1_S                            5

// Field:     [4] WEPROT_B0_ENGR_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_ENGR_BY1                     0x00000010
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_ENGR_BY1_BITN                         4
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_ENGR_BY1_M                   0x00000010
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_ENGR_BY1_S                            4

// Field:     [3] WEPROT_B1_TRIM_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_TRIM_BY1                     0x00000008
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_TRIM_BY1_BITN                         3
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_TRIM_BY1_M                   0x00000008
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_TRIM_BY1_S                            3

// Field:     [2] WEPROT_B0_TRIM_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_TRIM_BY1                     0x00000004
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_TRIM_BY1_BITN                         2
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_TRIM_BY1_M                   0x00000004
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_TRIM_BY1_S                            2

// Field:     [1] WEPROT_B1_FCFG_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_FCFG_BY1                     0x00000002
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_FCFG_BY1_BITN                         1
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_FCFG_BY1_M                   0x00000002
#define FLASH_WEPROT_AUX_BY1_WEPROT_B1_FCFG_BY1_S                            1

// Field:     [0] WEPROT_B0_CCFG_BY1
//
// Internal. Only to be used through TI provided API.
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1                     0x00000001
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1_BITN                         0
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1_M                   0x00000001
#define FLASH_WEPROT_AUX_BY1_WEPROT_B0_CCFG_BY1_S                            0

//*****************************************************************************
//
// Register: FLASH_O_STAT
//
//*****************************************************************************
// Field:    [16] STALLSTAT
//
// An ocp1 or ocp3 read stall has occurred.
// 0 : No stall or stall acknowledged by writing a 1
// 1 : Stall condition occurred/occurring
//
// This is a read/write-clear status bit. It will reset to 0. It will be set
// when either an ocp1 or ocp3 read occurs to a bank that is presently
// undergoing a program or write operation.
//
// An ocp2 write of 1 to this bit will clear the bit. The ocp2 write will take
// highest priority in the event an ocp1/ocp3 read is occurring concurrently to
// the ocp2 write.
// Clearing the bit should be done only after the ongoing program/erase
// operation is complete indicating that both banks are free.
// If clearing occurs while the stall condition persists, the field may get set
// back to one.
#define FLASH_STAT_STALLSTAT                                        0x00010000
#define FLASH_STAT_STALLSTAT_BITN                                           16
#define FLASH_STAT_STALLSTAT_M                                      0x00010000
#define FLASH_STAT_STALLSTAT_S                                              16

// Field:    [15] EFUSE_BLANK
//
// Efuse scanning detected if fuse ROM is blank:
// 0 : Not blank
// 1 : Blank
#define FLASH_STAT_EFUSE_BLANK                                      0x00008000
#define FLASH_STAT_EFUSE_BLANK_BITN                                         15
#define FLASH_STAT_EFUSE_BLANK_M                                    0x00008000
#define FLASH_STAT_EFUSE_BLANK_S                                            15

// Field:    [14] EFUSE_TIMEOUT
//
// Efuse scanning resulted in timeout error.
// 0 : No Timeout error
// 1 : Timeout Error
#define FLASH_STAT_EFUSE_TIMEOUT                                    0x00004000
#define FLASH_STAT_EFUSE_TIMEOUT_BITN                                       14
#define FLASH_STAT_EFUSE_TIMEOUT_M                                  0x00004000
#define FLASH_STAT_EFUSE_TIMEOUT_S                                          14

// Field:    [13] SPRS_BYTE_NOT_OK
//
// Efuse scanning resulted in scan chain Sparse byte error.
// 0 : No Sparse error
// 1 : Sparse Error
#define FLASH_STAT_SPRS_BYTE_NOT_OK                                 0x00002000
#define FLASH_STAT_SPRS_BYTE_NOT_OK_BITN                                    13
#define FLASH_STAT_SPRS_BYTE_NOT_OK_M                               0x00002000
#define FLASH_STAT_SPRS_BYTE_NOT_OK_S                                       13

// Field:  [12:8] EFUSE_ERRCODE
//
// Same as EFUSEERROR.CODE
#define FLASH_STAT_EFUSE_ERRCODE_W                                           5
#define FLASH_STAT_EFUSE_ERRCODE_M                                  0x00001F00
#define FLASH_STAT_EFUSE_ERRCODE_S                                           8

// Field:   [5:4] BUSY
//
// NW FW_SMSTAT.CMD_IN_PROGRESS bit.
// This flag is valid immediately after the operation setting it
// 0 : Not busy
// 1 : Busy
//
// Bit 4 is for the busy state for Bank0 which is at logical address 0x0
// Bit 5 for Bank1.
#define FLASH_STAT_BUSY_W                                                    2
#define FLASH_STAT_BUSY_M                                           0x00000030
#define FLASH_STAT_BUSY_S                                                    4

// Field:     [3] READY1T
//
// 1T access readiness status indicator from NW. Comes later than 2T readiness.
// 1: FLASH banks are ready for 1T accesses
// 0: FLASH banks are not ready for 1T accesses
#define FLASH_STAT_READY1T                                          0x00000008
#define FLASH_STAT_READY1T_BITN                                              3
#define FLASH_STAT_READY1T_M                                        0x00000008
#define FLASH_STAT_READY1T_S                                                 3

// Field:     [2] READY2T
//
// 2T access readiness status indicator from NW
// 1: FLASH banks are ready for 2T accesses
// 0: FLASH banks are not ready for 2T accesses
#define FLASH_STAT_READY2T                                          0x00000004
#define FLASH_STAT_READY2T_BITN                                              2
#define FLASH_STAT_READY2T_M                                        0x00000004
#define FLASH_STAT_READY2T_S                                                 2

// Field:   [1:0] POWER_MODE
//
// Power state of each of the 2 flash arbiter FSM instances in the flash
// sub-system. For Thor, these bits should mostly be in the same state since
// both banks are in the same power mode.
// 0 : Active
// 1 : Ready for Low power (The 2T readiness has gone low or the flash_off_req
// has been set=1, and flash_off_ack is ready to be asserted).
//
// Bit 0 is for the power state for Bank0 which is at logical address 0x0
// Bit 1 for Bank1
#define FLASH_STAT_POWER_MODE_W                                              2
#define FLASH_STAT_POWER_MODE_M                                     0x00000003
#define FLASH_STAT_POWER_MODE_S                                              0

//*****************************************************************************
//
// Register: FLASH_O_CFG
//
//*****************************************************************************
// Field:    [30] DIS_FWTEST
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_DIS_FWTEST                                        0x40000000
#define FLASH_CFG_DIS_FWTEST_BITN                                           30
#define FLASH_CFG_DIS_FWTEST_M                                      0x40000000
#define FLASH_CFG_DIS_FWTEST_S                                              30

// Field:    [11] MAIN_STICKY_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_MAIN_STICKY_EN                                    0x00000800
#define FLASH_CFG_MAIN_STICKY_EN_BITN                                       11
#define FLASH_CFG_MAIN_STICKY_EN_M                                  0x00000800
#define FLASH_CFG_MAIN_STICKY_EN_S                                          11

// Field:    [10] CCFG_STICKY_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_CCFG_STICKY_EN                                    0x00000400
#define FLASH_CFG_CCFG_STICKY_EN_BITN                                       10
#define FLASH_CFG_CCFG_STICKY_EN_M                                  0x00000400
#define FLASH_CFG_CCFG_STICKY_EN_S                                          10

// Field:     [9] FCFG_STICKY_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_FCFG_STICKY_EN                                    0x00000200
#define FLASH_CFG_FCFG_STICKY_EN_BITN                                        9
#define FLASH_CFG_FCFG_STICKY_EN_M                                  0x00000200
#define FLASH_CFG_FCFG_STICKY_EN_S                                           9

// Field:     [8] ENGR_TRIM_STICKY_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_ENGR_TRIM_STICKY_EN                               0x00000100
#define FLASH_CFG_ENGR_TRIM_STICKY_EN_BITN                                   8
#define FLASH_CFG_ENGR_TRIM_STICKY_EN_M                             0x00000100
#define FLASH_CFG_ENGR_TRIM_STICKY_EN_S                                      8

// Field:     [5] DIS_EFUSECLK
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_DIS_EFUSECLK                                      0x00000020
#define FLASH_CFG_DIS_EFUSECLK_BITN                                          5
#define FLASH_CFG_DIS_EFUSECLK_M                                    0x00000020
#define FLASH_CFG_DIS_EFUSECLK_S                                             5

// Field:     [4] DIS_READACCESS
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_DIS_READACCESS                                    0x00000010
#define FLASH_CFG_DIS_READACCESS_BITN                                        4
#define FLASH_CFG_DIS_READACCESS_M                                  0x00000010
#define FLASH_CFG_DIS_READACCESS_S                                           4

// Field:     [0] BP_TRIMCFG_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_CFG_BP_TRIMCFG_EN                                     0x00000001
#define FLASH_CFG_BP_TRIMCFG_EN_BITN                                         0
#define FLASH_CFG_BP_TRIMCFG_EN_M                                   0x00000001
#define FLASH_CFG_BP_TRIMCFG_EN_S                                            0

//*****************************************************************************
//
// Register: FLASH_O_FLASH_SIZE
//
//*****************************************************************************
// Field:   [9:7] SECTORS
//
// Internal. Only to be used through TI provided API.
#define FLASH_FLASH_SIZE_SECTORS_W                                           3
#define FLASH_FLASH_SIZE_SECTORS_M                                  0x00000380
#define FLASH_FLASH_SIZE_SECTORS_S                                           7

//*****************************************************************************
//
// Register: FLASH_O_FWLOCK
//
//*****************************************************************************
// Field:   [2:0] FWLOCK
//
// Internal. Only to be used through TI provided API.
#define FLASH_FWLOCK_FWLOCK_W                                                3
#define FLASH_FWLOCK_FWLOCK_M                                       0x00000007
#define FLASH_FWLOCK_FWLOCK_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_FWFLAG
//
//*****************************************************************************
// Field:   [2:0] FWFLAG
//
// Internal. Only to be used through TI provided API.
#define FLASH_FWFLAG_FWFLAG_W                                                3
#define FLASH_FWFLAG_FWFLAG_M                                       0x00000007
#define FLASH_FWFLAG_FWFLAG_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_BANK0_TRIM_CFG_3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FLASH_O_BANK0_TRIM_CFG_2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FLASH_O_BANK0_TRIM_CFG_1
//
//*****************************************************************************
// Field: [27:22] REDSWSELW3
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW3_W                                  6
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW3_M                         0x0FC00000
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW3_S                                 22

// Field: [21:16] REDSWSELW2
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW2_W                                  6
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW2_M                         0x003F0000
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW2_S                                 16

// Field: [15:10] REDSWSELW1
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW1_W                                  6
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW1_M                         0x0000FC00
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW1_S                                 10

// Field:   [9:4] REDSWSELW0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW0_W                                  6
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW0_M                         0x000003F0
#define FLASH_BANK0_TRIM_CFG_1_REDSWSELW0_S                                  4

// Field:     [3] REDSWENW3
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW3                            0x00000008
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW3_BITN                                3
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW3_M                          0x00000008
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW3_S                                   3

// Field:     [2] REDSWENW2
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW2                            0x00000004
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW2_BITN                                2
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW2_M                          0x00000004
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW2_S                                   2

// Field:     [1] REDSWENW1
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW1                            0x00000002
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW1_BITN                                1
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW1_M                          0x00000002
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW1_S                                   1

// Field:     [0] REDSWENW0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW0                            0x00000001
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW0_BITN                                0
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW0_M                          0x00000001
#define FLASH_BANK0_TRIM_CFG_1_REDSWENW0_S                                   0

//*****************************************************************************
//
// Register: FLASH_O_BANK0_TRIM_CFG_0
//
//*****************************************************************************
// Field:  [31:0] BANK0_TRIM_CFG_0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK0_TRIM_CFG_0_BANK0_TRIM_CFG_0_W                           32
#define FLASH_BANK0_TRIM_CFG_0_BANK0_TRIM_CFG_0_M                   0xFFFFFFFF
#define FLASH_BANK0_TRIM_CFG_0_BANK0_TRIM_CFG_0_S                            0

//*****************************************************************************
//
// Register: FLASH_O_BANK1_TRIM_CFG_3
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FLASH_O_BANK1_TRIM_CFG_2
//
//*****************************************************************************
//*****************************************************************************
//
// Register: FLASH_O_BANK1_TRIM_CFG_1
//
//*****************************************************************************
// Field: [27:22] REDSWSELW3
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW3_W                                  6
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW3_M                         0x0FC00000
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW3_S                                 22

// Field: [21:16] REDSWSELW2
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW2_W                                  6
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW2_M                         0x003F0000
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW2_S                                 16

// Field: [15:10] REDSWSELW1
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW1_W                                  6
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW1_M                         0x0000FC00
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW1_S                                 10

// Field:   [9:4] REDSWSELW0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW0_W                                  6
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW0_M                         0x000003F0
#define FLASH_BANK1_TRIM_CFG_1_REDSWSELW0_S                                  4

// Field:     [3] REDSWENW3
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW3                            0x00000008
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW3_BITN                                3
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW3_M                          0x00000008
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW3_S                                   3

// Field:     [2] REDSWENW2
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW2                            0x00000004
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW2_BITN                                2
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW2_M                          0x00000004
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW2_S                                   2

// Field:     [1] REDSWENW1
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW1                            0x00000002
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW1_BITN                                1
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW1_M                          0x00000002
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW1_S                                   1

// Field:     [0] REDSWENW0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW0                            0x00000001
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW0_BITN                                0
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW0_M                          0x00000001
#define FLASH_BANK1_TRIM_CFG_1_REDSWENW0_S                                   0

//*****************************************************************************
//
// Register: FLASH_O_BANK1_TRIM_CFG_0
//
//*****************************************************************************
// Field:  [31:0] BANK1_TRIM_CFG_0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BANK1_TRIM_CFG_0_BANK1_TRIM_CFG_0_W                           32
#define FLASH_BANK1_TRIM_CFG_0_BANK1_TRIM_CFG_0_M                   0xFFFFFFFF
#define FLASH_BANK1_TRIM_CFG_0_BANK1_TRIM_CFG_0_S                            0

//*****************************************************************************
//
// Register: FLASH_O_PUMP_TRIM_CFG_2
//
//*****************************************************************************
// Field: [25:20] VWLCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_2_VWLCT_W                                        6
#define FLASH_PUMP_TRIM_CFG_2_VWLCT_M                               0x03F00000
#define FLASH_PUMP_TRIM_CFG_2_VWLCT_S                                       20

// Field: [19:14] VSLCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_2_VSLCT_W                                        6
#define FLASH_PUMP_TRIM_CFG_2_VSLCT_M                               0x000FC000
#define FLASH_PUMP_TRIM_CFG_2_VSLCT_S                                       14

// Field:  [13:9] VREADCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_2_VREADCT_W                                      5
#define FLASH_PUMP_TRIM_CFG_2_VREADCT_M                             0x00003E00
#define FLASH_PUMP_TRIM_CFG_2_VREADCT_S                                      9

// Field:   [8:4] VINLOWCCORCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_2_VINLOWCCORCT_W                                 5
#define FLASH_PUMP_TRIM_CFG_2_VINLOWCCORCT_M                        0x000001F0
#define FLASH_PUMP_TRIM_CFG_2_VINLOWCCORCT_S                                 4

// Field:   [3:0] VINHICCORCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_2_VINHICCORCT_W                                  4
#define FLASH_PUMP_TRIM_CFG_2_VINHICCORCT_M                         0x0000000F
#define FLASH_PUMP_TRIM_CFG_2_VINHICCORCT_S                                  0

//*****************************************************************************
//
// Register: FLASH_O_PUMP_TRIM_CFG_1
//
//*****************************************************************************
// Field:    [31] VINHICCORCTLSB
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_VINHICCORCTLSB                        0x80000000
#define FLASH_PUMP_TRIM_CFG_1_VINHICCORCTLSB_BITN                           31
#define FLASH_PUMP_TRIM_CFG_1_VINHICCORCTLSB_M                      0x80000000
#define FLASH_PUMP_TRIM_CFG_1_VINHICCORCTLSB_S                              31

// Field: [30:25] VINHCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_VINHCT_W                                       6
#define FLASH_PUMP_TRIM_CFG_1_VINHCT_M                              0x7E000000
#define FLASH_PUMP_TRIM_CFG_1_VINHCT_S                                      25

// Field: [24:20] VCGCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_VCGCT_W                                        5
#define FLASH_PUMP_TRIM_CFG_1_VCGCT_M                               0x01F00000
#define FLASH_PUMP_TRIM_CFG_1_VCGCT_S                                       20

// Field: [19:15] IREFVRDCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_IREFVRDCT_W                                    5
#define FLASH_PUMP_TRIM_CFG_1_IREFVRDCT_M                           0x000F8000
#define FLASH_PUMP_TRIM_CFG_1_IREFVRDCT_S                                   15

// Field: [14:10] IREFTCCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_IREFTCCT_W                                     5
#define FLASH_PUMP_TRIM_CFG_1_IREFTCCT_M                            0x00007C00
#define FLASH_PUMP_TRIM_CFG_1_IREFTCCT_S                                    10

// Field:   [9:6] IREFCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_IREFCT_W                                       4
#define FLASH_PUMP_TRIM_CFG_1_IREFCT_M                              0x000003C0
#define FLASH_PUMP_TRIM_CFG_1_IREFCT_S                                       6

// Field:   [5:0] FOSCCT
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_1_FOSCCT_W                                       6
#define FLASH_PUMP_TRIM_CFG_1_FOSCCT_M                              0x0000003F
#define FLASH_PUMP_TRIM_CFG_1_FOSCCT_S                                       0

//*****************************************************************************
//
// Register: FLASH_O_PUMP_TRIM_CFG_0
//
//*****************************************************************************
// Field: [29:20] VHVCT_PV
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PV_W                                    10
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PV_M                            0x3FF00000
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PV_S                                    20

// Field: [19:10] VHVCT_PGM
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PGM_W                                   10
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PGM_M                           0x000FFC00
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_PGM_S                                   10

// Field:   [9:0] VHVCT_ERS
//
// Internal. Only to be used through TI provided API.
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_ERS_W                                   10
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_ERS_M                           0x000003FF
#define FLASH_PUMP_TRIM_CFG_0_VHVCT_ERS_S                                    0

//*****************************************************************************
//
// Register: FLASH_O_EFUSE
//
//*****************************************************************************
// Field: [28:24] INSTRUCTION
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSE_INSTRUCTION_W                                            5
#define FLASH_EFUSE_INSTRUCTION_M                                   0x1F000000
#define FLASH_EFUSE_INSTRUCTION_S                                           24

// Field:  [15:0] DUMPWORD
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSE_DUMPWORD_W                                              16
#define FLASH_EFUSE_DUMPWORD_M                                      0x0000FFFF
#define FLASH_EFUSE_DUMPWORD_S                                               0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEADDR
//
//*****************************************************************************
// Field: [15:11] BLOCK
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEADDR_BLOCK_W                                              5
#define FLASH_EFUSEADDR_BLOCK_M                                     0x0000F800
#define FLASH_EFUSEADDR_BLOCK_S                                             11

// Field:  [10:0] ROW
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEADDR_ROW_W                                               11
#define FLASH_EFUSEADDR_ROW_M                                       0x000007FF
#define FLASH_EFUSEADDR_ROW_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_DATAUPPER
//
//*****************************************************************************
// Field:   [7:3] SPARE
//
// Internal. Only to be used through TI provided API.
#define FLASH_DATAUPPER_SPARE_W                                              5
#define FLASH_DATAUPPER_SPARE_M                                     0x000000F8
#define FLASH_DATAUPPER_SPARE_S                                              3

// Field:     [2] P
//
// Internal. Only to be used through TI provided API.
#define FLASH_DATAUPPER_P                                           0x00000004
#define FLASH_DATAUPPER_P_BITN                                               2
#define FLASH_DATAUPPER_P_M                                         0x00000004
#define FLASH_DATAUPPER_P_S                                                  2

// Field:     [1] R
//
// Internal. Only to be used through TI provided API.
#define FLASH_DATAUPPER_R                                           0x00000002
#define FLASH_DATAUPPER_R_BITN                                               1
#define FLASH_DATAUPPER_R_M                                         0x00000002
#define FLASH_DATAUPPER_R_S                                                  1

// Field:     [0] EEN
//
// Internal. Only to be used through TI provided API.
#define FLASH_DATAUPPER_EEN                                         0x00000001
#define FLASH_DATAUPPER_EEN_BITN                                             0
#define FLASH_DATAUPPER_EEN_M                                       0x00000001
#define FLASH_DATAUPPER_EEN_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_DATALOWER
//
//*****************************************************************************
// Field:  [31:0] DATA
//
// Internal. Only to be used through TI provided API.
#define FLASH_DATALOWER_DATA_W                                              32
#define FLASH_DATALOWER_DATA_M                                      0xFFFFFFFF
#define FLASH_DATALOWER_DATA_S                                               0

//*****************************************************************************
//
// Register: FLASH_O_EFUSECFG
//
//*****************************************************************************
// Field:     [8] IDLEGATING
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSECFG_IDLEGATING                                   0x00000100
#define FLASH_EFUSECFG_IDLEGATING_BITN                                       8
#define FLASH_EFUSECFG_IDLEGATING_M                                 0x00000100
#define FLASH_EFUSECFG_IDLEGATING_S                                          8

// Field:   [4:3] SLAVEPOWER
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSECFG_SLAVEPOWER_W                                          2
#define FLASH_EFUSECFG_SLAVEPOWER_M                                 0x00000018
#define FLASH_EFUSECFG_SLAVEPOWER_S                                          3

// Field:     [0] GATING
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSECFG_GATING                                       0x00000001
#define FLASH_EFUSECFG_GATING_BITN                                           0
#define FLASH_EFUSECFG_GATING_M                                     0x00000001
#define FLASH_EFUSECFG_GATING_S                                              0

//*****************************************************************************
//
// Register: FLASH_O_EFUSESTAT
//
//*****************************************************************************
// Field:     [0] RESETDONE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSESTAT_RESETDONE                                   0x00000001
#define FLASH_EFUSESTAT_RESETDONE_BITN                                       0
#define FLASH_EFUSESTAT_RESETDONE_M                                 0x00000001
#define FLASH_EFUSESTAT_RESETDONE_S                                          0

//*****************************************************************************
//
// Register: FLASH_O_ACC
//
//*****************************************************************************
// Field:  [23:0] ACCUMULATOR
//
// Internal. Only to be used through TI provided API.
#define FLASH_ACC_ACCUMULATOR_W                                             24
#define FLASH_ACC_ACCUMULATOR_M                                     0x00FFFFFF
#define FLASH_ACC_ACCUMULATOR_S                                              0

//*****************************************************************************
//
// Register: FLASH_O_BOUNDARY
//
//*****************************************************************************
// Field:    [23] DISROW0
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_DISROW0                                      0x00800000
#define FLASH_BOUNDARY_DISROW0_BITN                                         23
#define FLASH_BOUNDARY_DISROW0_M                                    0x00800000
#define FLASH_BOUNDARY_DISROW0_S                                            23

// Field:    [22] SPARE
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SPARE                                        0x00400000
#define FLASH_BOUNDARY_SPARE_BITN                                           22
#define FLASH_BOUNDARY_SPARE_M                                      0x00400000
#define FLASH_BOUNDARY_SPARE_S                                              22

// Field:    [21] EFC_SELF_TEST_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_EFC_SELF_TEST_ERROR                          0x00200000
#define FLASH_BOUNDARY_EFC_SELF_TEST_ERROR_BITN                             21
#define FLASH_BOUNDARY_EFC_SELF_TEST_ERROR_M                        0x00200000
#define FLASH_BOUNDARY_EFC_SELF_TEST_ERROR_S                                21

// Field:    [20] EFC_INSTRUCTION_INFO
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_EFC_INSTRUCTION_INFO                         0x00100000
#define FLASH_BOUNDARY_EFC_INSTRUCTION_INFO_BITN                            20
#define FLASH_BOUNDARY_EFC_INSTRUCTION_INFO_M                       0x00100000
#define FLASH_BOUNDARY_EFC_INSTRUCTION_INFO_S                               20

// Field:    [19] EFC_INSTRUCTION_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_EFC_INSTRUCTION_ERROR                        0x00080000
#define FLASH_BOUNDARY_EFC_INSTRUCTION_ERROR_BITN                           19
#define FLASH_BOUNDARY_EFC_INSTRUCTION_ERROR_M                      0x00080000
#define FLASH_BOUNDARY_EFC_INSTRUCTION_ERROR_S                              19

// Field:    [18] EFC_AUTOLOAD_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_EFC_AUTOLOAD_ERROR                           0x00040000
#define FLASH_BOUNDARY_EFC_AUTOLOAD_ERROR_BITN                              18
#define FLASH_BOUNDARY_EFC_AUTOLOAD_ERROR_M                         0x00040000
#define FLASH_BOUNDARY_EFC_AUTOLOAD_ERROR_S                                 18

// Field: [17:14] OUTPUTENABLE
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_OUTPUTENABLE_W                                        4
#define FLASH_BOUNDARY_OUTPUTENABLE_M                               0x0003C000
#define FLASH_BOUNDARY_OUTPUTENABLE_S                                       14

// Field:    [13] SYS_ECC_SELF_TEST_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SYS_ECC_SELF_TEST_EN                         0x00002000
#define FLASH_BOUNDARY_SYS_ECC_SELF_TEST_EN_BITN                            13
#define FLASH_BOUNDARY_SYS_ECC_SELF_TEST_EN_M                       0x00002000
#define FLASH_BOUNDARY_SYS_ECC_SELF_TEST_EN_S                               13

// Field:    [12] SYS_ECC_OVERRIDE_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SYS_ECC_OVERRIDE_EN                          0x00001000
#define FLASH_BOUNDARY_SYS_ECC_OVERRIDE_EN_BITN                             12
#define FLASH_BOUNDARY_SYS_ECC_OVERRIDE_EN_M                        0x00001000
#define FLASH_BOUNDARY_SYS_ECC_OVERRIDE_EN_S                                12

// Field:    [11] EFC_FDI
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_EFC_FDI                                      0x00000800
#define FLASH_BOUNDARY_EFC_FDI_BITN                                         11
#define FLASH_BOUNDARY_EFC_FDI_M                                    0x00000800
#define FLASH_BOUNDARY_EFC_FDI_S                                            11

// Field:    [10] SYS_DIEID_AUTOLOAD_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SYS_DIEID_AUTOLOAD_EN                        0x00000400
#define FLASH_BOUNDARY_SYS_DIEID_AUTOLOAD_EN_BITN                           10
#define FLASH_BOUNDARY_SYS_DIEID_AUTOLOAD_EN_M                      0x00000400
#define FLASH_BOUNDARY_SYS_DIEID_AUTOLOAD_EN_S                              10

// Field:   [9:8] SYS_REPAIR_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SYS_REPAIR_EN_W                                       2
#define FLASH_BOUNDARY_SYS_REPAIR_EN_M                              0x00000300
#define FLASH_BOUNDARY_SYS_REPAIR_EN_S                                       8

// Field:   [7:4] SYS_WS_READ_STATES
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_SYS_WS_READ_STATES_W                                  4
#define FLASH_BOUNDARY_SYS_WS_READ_STATES_M                         0x000000F0
#define FLASH_BOUNDARY_SYS_WS_READ_STATES_S                                  4

// Field:   [3:0] INPUTENABLE
//
// Internal. Only to be used through TI provided API.
#define FLASH_BOUNDARY_INPUTENABLE_W                                         4
#define FLASH_BOUNDARY_INPUTENABLE_M                                0x0000000F
#define FLASH_BOUNDARY_INPUTENABLE_S                                         0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEFLAG
//
//*****************************************************************************
// Field:     [0] KEY
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEFLAG_KEY                                         0x00000001
#define FLASH_EFUSEFLAG_KEY_BITN                                             0
#define FLASH_EFUSEFLAG_KEY_M                                       0x00000001
#define FLASH_EFUSEFLAG_KEY_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEKEY
//
//*****************************************************************************
// Field:  [31:0] CODE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEKEY_CODE_W                                               32
#define FLASH_EFUSEKEY_CODE_M                                       0xFFFFFFFF
#define FLASH_EFUSEKEY_CODE_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_EFUSERELEASE
//
//*****************************************************************************
// Field: [31:25] ODPYEAR
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_ODPYEAR_W                                         7
#define FLASH_EFUSERELEASE_ODPYEAR_M                                0xFE000000
#define FLASH_EFUSERELEASE_ODPYEAR_S                                        25

// Field: [24:21] ODPMONTH
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_ODPMONTH_W                                        4
#define FLASH_EFUSERELEASE_ODPMONTH_M                               0x01E00000
#define FLASH_EFUSERELEASE_ODPMONTH_S                                       21

// Field: [20:16] ODPDAY
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_ODPDAY_W                                          5
#define FLASH_EFUSERELEASE_ODPDAY_M                                 0x001F0000
#define FLASH_EFUSERELEASE_ODPDAY_S                                         16

// Field:  [15:9] EFUSEYEAR
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_EFUSEYEAR_W                                       7
#define FLASH_EFUSERELEASE_EFUSEYEAR_M                              0x0000FE00
#define FLASH_EFUSERELEASE_EFUSEYEAR_S                                       9

// Field:   [8:5] EFUSEMONTH
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_EFUSEMONTH_W                                      4
#define FLASH_EFUSERELEASE_EFUSEMONTH_M                             0x000001E0
#define FLASH_EFUSERELEASE_EFUSEMONTH_S                                      5

// Field:   [4:0] EFUSEDAY
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSERELEASE_EFUSEDAY_W                                        5
#define FLASH_EFUSERELEASE_EFUSEDAY_M                               0x0000001F
#define FLASH_EFUSERELEASE_EFUSEDAY_S                                        0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEPINS
//
//*****************************************************************************
// Field:    [15] EFC_SELF_TEST_DONE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_SELF_TEST_DONE                          0x00008000
#define FLASH_EFUSEPINS_EFC_SELF_TEST_DONE_BITN                             15
#define FLASH_EFUSEPINS_EFC_SELF_TEST_DONE_M                        0x00008000
#define FLASH_EFUSEPINS_EFC_SELF_TEST_DONE_S                                15

// Field:    [14] EFC_SELF_TEST_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_SELF_TEST_ERROR                         0x00004000
#define FLASH_EFUSEPINS_EFC_SELF_TEST_ERROR_BITN                            14
#define FLASH_EFUSEPINS_EFC_SELF_TEST_ERROR_M                       0x00004000
#define FLASH_EFUSEPINS_EFC_SELF_TEST_ERROR_S                               14

// Field:    [13] SYS_ECC_SELF_TEST_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_SYS_ECC_SELF_TEST_EN                        0x00002000
#define FLASH_EFUSEPINS_SYS_ECC_SELF_TEST_EN_BITN                           13
#define FLASH_EFUSEPINS_SYS_ECC_SELF_TEST_EN_M                      0x00002000
#define FLASH_EFUSEPINS_SYS_ECC_SELF_TEST_EN_S                              13

// Field:    [12] EFC_INSTRUCTION_INFO
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_INFO                        0x00001000
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_INFO_BITN                           12
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_INFO_M                      0x00001000
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_INFO_S                              12

// Field:    [11] EFC_INSTRUCTION_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_ERROR                       0x00000800
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_ERROR_BITN                          11
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_ERROR_M                     0x00000800
#define FLASH_EFUSEPINS_EFC_INSTRUCTION_ERROR_S                             11

// Field:    [10] EFC_AUTOLOAD_ERROR
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_AUTOLOAD_ERROR                          0x00000400
#define FLASH_EFUSEPINS_EFC_AUTOLOAD_ERROR_BITN                             10
#define FLASH_EFUSEPINS_EFC_AUTOLOAD_ERROR_M                        0x00000400
#define FLASH_EFUSEPINS_EFC_AUTOLOAD_ERROR_S                                10

// Field:     [9] SYS_ECC_OVERRIDE_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_SYS_ECC_OVERRIDE_EN                         0x00000200
#define FLASH_EFUSEPINS_SYS_ECC_OVERRIDE_EN_BITN                             9
#define FLASH_EFUSEPINS_SYS_ECC_OVERRIDE_EN_M                       0x00000200
#define FLASH_EFUSEPINS_SYS_ECC_OVERRIDE_EN_S                                9

// Field:     [8] EFC_READY
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_READY                                   0x00000100
#define FLASH_EFUSEPINS_EFC_READY_BITN                                       8
#define FLASH_EFUSEPINS_EFC_READY_M                                 0x00000100
#define FLASH_EFUSEPINS_EFC_READY_S                                          8

// Field:     [7] EFC_FCLRZ
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_EFC_FCLRZ                                   0x00000080
#define FLASH_EFUSEPINS_EFC_FCLRZ_BITN                                       7
#define FLASH_EFUSEPINS_EFC_FCLRZ_M                                 0x00000080
#define FLASH_EFUSEPINS_EFC_FCLRZ_S                                          7

// Field:     [6] SYS_DIEID_AUTOLOAD_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_SYS_DIEID_AUTOLOAD_EN                       0x00000040
#define FLASH_EFUSEPINS_SYS_DIEID_AUTOLOAD_EN_BITN                           6
#define FLASH_EFUSEPINS_SYS_DIEID_AUTOLOAD_EN_M                     0x00000040
#define FLASH_EFUSEPINS_SYS_DIEID_AUTOLOAD_EN_S                              6

// Field:   [5:4] SYS_REPAIR_EN
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_SYS_REPAIR_EN_W                                      2
#define FLASH_EFUSEPINS_SYS_REPAIR_EN_M                             0x00000030
#define FLASH_EFUSEPINS_SYS_REPAIR_EN_S                                      4

// Field:   [3:0] SYS_WS_READ_STATES
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPINS_SYS_WS_READ_STATES_W                                 4
#define FLASH_EFUSEPINS_SYS_WS_READ_STATES_M                        0x0000000F
#define FLASH_EFUSEPINS_SYS_WS_READ_STATES_S                                 0

//*****************************************************************************
//
// Register: FLASH_O_EFUSECRA
//
//*****************************************************************************
// Field:   [5:0] DATA
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSECRA_DATA_W                                                6
#define FLASH_EFUSECRA_DATA_M                                       0x0000003F
#define FLASH_EFUSECRA_DATA_S                                                0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEREAD
//
//*****************************************************************************
// Field:   [9:8] DATABIT
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEREAD_DATABIT_W                                            2
#define FLASH_EFUSEREAD_DATABIT_M                                   0x00000300
#define FLASH_EFUSEREAD_DATABIT_S                                            8

// Field:   [7:4] READCLOCK
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEREAD_READCLOCK_W                                          4
#define FLASH_EFUSEREAD_READCLOCK_M                                 0x000000F0
#define FLASH_EFUSEREAD_READCLOCK_S                                          4

// Field:     [3] DEBUG
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEREAD_DEBUG                                       0x00000008
#define FLASH_EFUSEREAD_DEBUG_BITN                                           3
#define FLASH_EFUSEREAD_DEBUG_M                                     0x00000008
#define FLASH_EFUSEREAD_DEBUG_S                                              3

// Field:     [2] SPARE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEREAD_SPARE                                       0x00000004
#define FLASH_EFUSEREAD_SPARE_BITN                                           2
#define FLASH_EFUSEREAD_SPARE_M                                     0x00000004
#define FLASH_EFUSEREAD_SPARE_S                                              2

// Field:   [1:0] MARGIN
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEREAD_MARGIN_W                                             2
#define FLASH_EFUSEREAD_MARGIN_M                                    0x00000003
#define FLASH_EFUSEREAD_MARGIN_S                                             0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEPROGRAM
//
//*****************************************************************************
// Field:    [30] COMPAREDISABLE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPROGRAM_COMPAREDISABLE                           0x40000000
#define FLASH_EFUSEPROGRAM_COMPAREDISABLE_BITN                              30
#define FLASH_EFUSEPROGRAM_COMPAREDISABLE_M                         0x40000000
#define FLASH_EFUSEPROGRAM_COMPAREDISABLE_S                                 30

// Field: [29:14] CLOCKSTALL
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPROGRAM_CLOCKSTALL_W                                     16
#define FLASH_EFUSEPROGRAM_CLOCKSTALL_M                             0x3FFFC000
#define FLASH_EFUSEPROGRAM_CLOCKSTALL_S                                     14

// Field:    [13] VPPTOVDD
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPROGRAM_VPPTOVDD                                 0x00002000
#define FLASH_EFUSEPROGRAM_VPPTOVDD_BITN                                    13
#define FLASH_EFUSEPROGRAM_VPPTOVDD_M                               0x00002000
#define FLASH_EFUSEPROGRAM_VPPTOVDD_S                                       13

// Field:  [12:9] ITERATIONS
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPROGRAM_ITERATIONS_W                                      4
#define FLASH_EFUSEPROGRAM_ITERATIONS_M                             0x00001E00
#define FLASH_EFUSEPROGRAM_ITERATIONS_S                                      9

// Field:   [8:0] WRITECLOCK
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEPROGRAM_WRITECLOCK_W                                      9
#define FLASH_EFUSEPROGRAM_WRITECLOCK_M                             0x000001FF
#define FLASH_EFUSEPROGRAM_WRITECLOCK_S                                      0

//*****************************************************************************
//
// Register: FLASH_O_EFUSEERROR
//
//*****************************************************************************
// Field:     [5] DONE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEERROR_DONE                                       0x00000020
#define FLASH_EFUSEERROR_DONE_BITN                                           5
#define FLASH_EFUSEERROR_DONE_M                                     0x00000020
#define FLASH_EFUSEERROR_DONE_S                                              5

// Field:   [4:0] CODE
//
// Internal. Only to be used through TI provided API.
#define FLASH_EFUSEERROR_CODE_W                                              5
#define FLASH_EFUSEERROR_CODE_M                                     0x0000001F
#define FLASH_EFUSEERROR_CODE_S                                              0

//*****************************************************************************
//
// Register: FLASH_O_SINGLEBIT
//
//*****************************************************************************
// Field:  [31:1] FROMN
//
// Internal. Only to be used through TI provided API.
#define FLASH_SINGLEBIT_FROMN_W                                             31
#define FLASH_SINGLEBIT_FROMN_M                                     0xFFFFFFFE
#define FLASH_SINGLEBIT_FROMN_S                                              1

// Field:     [0] FROM0
//
// Internal. Only to be used through TI provided API.
#define FLASH_SINGLEBIT_FROM0                                       0x00000001
#define FLASH_SINGLEBIT_FROM0_BITN                                           0
#define FLASH_SINGLEBIT_FROM0_M                                     0x00000001
#define FLASH_SINGLEBIT_FROM0_S                                              0

//*****************************************************************************
//
// Register: FLASH_O_TWOBIT
//
//*****************************************************************************
// Field:  [31:1] FROMN
//
// Internal. Only to be used through TI provided API.
#define FLASH_TWOBIT_FROMN_W                                                31
#define FLASH_TWOBIT_FROMN_M                                        0xFFFFFFFE
#define FLASH_TWOBIT_FROMN_S                                                 1

// Field:     [0] FROM0
//
// Internal. Only to be used through TI provided API.
#define FLASH_TWOBIT_FROM0                                          0x00000001
#define FLASH_TWOBIT_FROM0_BITN                                              0
#define FLASH_TWOBIT_FROM0_M                                        0x00000001
#define FLASH_TWOBIT_FROM0_S                                                 0

//*****************************************************************************
//
// Register: FLASH_O_SELFTESTCYC
//
//*****************************************************************************
// Field:  [31:0] CYCLES
//
// Internal. Only to be used through TI provided API.
#define FLASH_SELFTESTCYC_CYCLES_W                                          32
#define FLASH_SELFTESTCYC_CYCLES_M                                  0xFFFFFFFF
#define FLASH_SELFTESTCYC_CYCLES_S                                           0

//*****************************************************************************
//
// Register: FLASH_O_SELFTESTSIGN
//
//*****************************************************************************
// Field:  [31:0] SIGNATURE
//
// Internal. Only to be used through TI provided API.
#define FLASH_SELFTESTSIGN_SIGNATURE_W                                      32
#define FLASH_SELFTESTSIGN_SIGNATURE_M                              0xFFFFFFFF
#define FLASH_SELFTESTSIGN_SIGNATURE_S                                       0


#endif // __FLASH__
