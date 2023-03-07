/******************************************************************************
*  Filename:       hw_nvmnw_h
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

#ifndef __HW_NVMNW_H__
#define __HW_NVMNW_H__

//*****************************************************************************
//
// This section defines the register offsets of
// NVMNW component
//
//*****************************************************************************
// Interrupt Index Register:
#define NVMNW_O_IIDX                                                0x00000020

// Interrupt Mask Register:
#define NVMNW_O_IMASK                                               0x00000028

// Raw Interrupt Status Register:
#define NVMNW_O_RIS                                                 0x00000030

// Masked Interrupt Status Register:
#define NVMNW_O_MIS                                                 0x00000038

// Interrupt Set Register:
#define NVMNW_O_ISET                                                0x00000040

// Interrupt Clear Register.
#define NVMNW_O_ICLR                                                0x00000048

// Event mode register. It is used to select whether each line is disabled, in
// software mode (software clears the RIS) or in hardware mode (hardware clears
// the RIS)
#define NVMNW_O_EVT_MODE                                            0x000000E0

// Hardware Version Description Register:
#define NVMNW_O_DESC                                                0x000000FC

// Command Execute Register:
#define NVMNW_O_CMDEXEC                                             0x00000100

// Command Type Register
#define NVMNW_O_CMDTYPE                                             0x00000104

// Command Control Register
#define NVMNW_O_CMDCTL                                              0x00000108

// Command Address Register:
#define NVMNW_O_CMDADDR                                             0x00000120

// Command Program Byte Enable Register:
#define NVMNW_O_CMDBYTEN                                            0x00000124

// Command Program Data Index Register:
#define NVMNW_O_CMDDATAINDEX                                        0x0000012C

// Command Data Register 0
#define NVMNW_O_CMDDATA0                                            0x00000130

// Command Data Register 1
#define NVMNW_O_CMDDATA1                                            0x00000134

// Command Data Register 2
#define NVMNW_O_CMDDATA2                                            0x00000138

// Command Data Register 3
#define NVMNW_O_CMDDATA3                                            0x0000013C

// Command Data Register 4
#define NVMNW_O_CMDDATA4                                            0x00000140

// Command Data Register 5
#define NVMNW_O_CMDDATA5                                            0x00000144

// Command Data Register 6
#define NVMNW_O_CMDDATA6                                            0x00000148

// Command Data Register 7
#define NVMNW_O_CMDDATA7                                            0x0000014C

// Command Data Register 8
#define NVMNW_O_CMDDATA8                                            0x00000150

// Command Data Register 9
#define NVMNW_O_CMDDATA9                                            0x00000154

// Command Data Register 10
#define NVMNW_O_CMDDATA10                                           0x00000158

// Command Data Register 11
#define NVMNW_O_CMDDATA11                                           0x0000015C

// Command Data Register 12
#define NVMNW_O_CMDDATA12                                           0x00000160

// Command Data Register 13
#define NVMNW_O_CMDDATA13                                           0x00000164

// Command Data Register 14
#define NVMNW_O_CMDDATA14                                           0x00000168

// Command Data Register 15
#define NVMNW_O_CMDDATA15                                           0x0000016C

// Command Data Register 16
#define NVMNW_O_CMDDATA16                                           0x00000170

// Command Data Register 17
#define NVMNW_O_CMDDATA17                                           0x00000174

// Command Data Register 18
#define NVMNW_O_CMDDATA18                                           0x00000178

// Command Data Register 19
#define NVMNW_O_CMDDATA19                                           0x0000017C

// Command Data Register 20
#define NVMNW_O_CMDDATA20                                           0x00000180

// Command Data Register 21
#define NVMNW_O_CMDDATA21                                           0x00000184

// Command Data Register 22
#define NVMNW_O_CMDDATA22                                           0x00000188

// Command Data Register 23
#define NVMNW_O_CMDDATA23                                           0x0000018C

// Command Data Register 24
#define NVMNW_O_CMDDATA24                                           0x00000190

// Command Data Register 25
#define NVMNW_O_CMDDATA25                                           0x00000194

// Command Data Register 26
#define NVMNW_O_CMDDATA26                                           0x00000198

// Command Data Register 27
#define NVMNW_O_CMDDATA27                                           0x0000019C

// Command Data Register 28
#define NVMNW_O_CMDDATA28                                           0x000001A0

// Command Data Register 29
#define NVMNW_O_CMDDATA29                                           0x000001A4

// Command Data Register 30
#define NVMNW_O_CMDDATA30                                           0x000001A8

// Command Data Register 31
#define NVMNW_O_CMDDATA31                                           0x000001AC

// Command Data Register 0
#define NVMNW_O_CMDDATAECC0                                         0x000001B0

// Command Data Register 1
#define NVMNW_O_CMDDATAECC1                                         0x000001B4

// Command Data Register 2
#define NVMNW_O_CMDDATAECC2                                         0x000001B8

// Command Data Register 3
#define NVMNW_O_CMDDATAECC3                                         0x000001BC

// Command Data Register 4
#define NVMNW_O_CMDDATAECC4                                         0x000001C0

// Command Data Register 5
#define NVMNW_O_CMDDATAECC5                                         0x000001C4

// Command Data Register 6
#define NVMNW_O_CMDDATAECC6                                         0x000001C8

// Command Data Register 7
#define NVMNW_O_CMDDATAECC7                                         0x000001CC

// Command WriteErase Protect A Register
#define NVMNW_O_CMDWEPROTA                                          0x000001D0

// Command WriteErase Protect B Register
#define NVMNW_O_CMDWEPROTB                                          0x000001D4

// Command WriteErase Protect C Register
#define NVMNW_O_CMDWEPROTC                                          0x000001D8

// Command WriteErase Protect Non-Main
#define NVMNW_O_CMDWEPROTNM                                         0x00000210

// Command WriteErase Protect Trim
#define NVMNW_O_CMDWEPROTTR                                         0x00000214

// Command WriteErase Protect Engr
#define NVMNW_O_CMDWEPROTEN                                         0x00000218

// Command Configuration Register
#define NVMNW_O_CFGCMD                                              0x000003B0

// Pulse Counter Configuration Register
#define NVMNW_O_CFGPCNT                                             0x000003B4

// Command Status Register
#define NVMNW_O_STATCMD                                             0x000003D0

// Current Address Counter Value
#define NVMNW_O_STATADDR                                            0x000003D4

// Current Pulse Count Register:
#define NVMNW_O_STATPCNT                                            0x000003D8

// Mode Status Register
#define NVMNW_O_STATMODE                                            0x000003DC

// Global Info 0 Register
#define NVMNW_O_GBLINFO0                                            0x000003F0

// Global Info 1 Register
#define NVMNW_O_GBLINFO1                                            0x000003F4

// Bank Info 0 Register for bank 0.
#define NVMNW_O_BANK0INFO0                                          0x00000400

// Bank Info1 Register for bank 0.
#define NVMNW_O_BANK0INFO1                                          0x00000404

// Bank Info 0 Register for bank 1.
#define NVMNW_O_BANK1INFO0                                          0x00000410

// Bank Info1 Register for bank 1.
#define NVMNW_O_BANK1INFO1                                          0x00000414

// Bank Info 0 Register for bank 2.
#define NVMNW_O_BANK2INFO0                                          0x00000420

// Bank Info1 Register for bank 2.
#define NVMNW_O_BANK2INFO1                                          0x00000424

// Bank Info 0 Register for bank 3.
#define NVMNW_O_BANK3INFO0                                          0x00000430

// Bank Info1 Register for bank 3.
#define NVMNW_O_BANK3INFO1                                          0x00000434

// Bank Info 0 Register for bank 4.
#define NVMNW_O_BANK4INFO0                                          0x00000440

// Bank Info1 Register for bank 4.
#define NVMNW_O_BANK4INFO1                                          0x00000444

// DFT Enable Register
#define NVMNW_O_DFTEN                                               0x00000500

// DFT Command Control Register
#define NVMNW_O_DFTCMDCTL                                           0x00000504

// DFT Timer Control Register
#define NVMNW_O_DFTTIMERCTL                                         0x00000508

// DFT EXECUTEZ control register.  This register allows direct control of the
#define NVMNW_O_DFTEXECZCTL                                         0x0000050C

// DFT Pump Clock Test Control Register.  This register controls hardware
// features
#define NVMNW_O_DFTPCLKTESTCTL                                      0x00000510

// DFT Pump Clock Test Status Register.  This register shows status reported by
// the
#define NVMNW_O_DFTPCLKTESTSTAT                                     0x00000514

// DFT Redundancy Data Register 0
#define NVMNW_O_DFTDATARED0                                         0x00000540

// DFT Redundancy Data Register 1
#define NVMNW_O_DFTDATARED1                                         0x00000544

// DFT Redundancy Data Register 2
#define NVMNW_O_DFTDATARED2                                         0x00000548

// DFT Redundancy Data Register 3
#define NVMNW_O_DFTDATARED3                                         0x0000054C

// DFT Redundancy Data Register 4
#define NVMNW_O_DFTDATARED4                                         0x00000550

// DFT Redundancy Data Register 5
#define NVMNW_O_DFTDATARED5                                         0x00000554

// DFT Redundancy Data Register 6
#define NVMNW_O_DFTDATARED6                                         0x00000558

// DFT Redundancy Data Register 7
#define NVMNW_O_DFTDATARED7                                         0x0000055C

// DFT Pump Control Register
#define NVMNW_O_DFTPUMPCTL                                          0x00000560

// DFT Bank Control Register
#define NVMNW_O_DFTBANKCTL                                          0x00000564

// Allows control of the application of bank/pump trim values.
#define NVMNW_O_TRIMCTL                                             0x00000600

// Pump Trim 0 Register.
#define NVMNW_O_PUMPTRIM0                                           0x00000610

// Pump Trim 1 Register.
#define NVMNW_O_PUMPTRIM1                                           0x00000614

// Pump Trim 2 Register.
#define NVMNW_O_PUMPTRIM2                                           0x00000618

// Bank 0 Trim 0 Register.
#define NVMNW_O_BANK0TRIM0                                          0x00000630

// Bank 0 Trim 1 Register.
#define NVMNW_O_BANK0TRIM1                                          0x00000634

// Bank 1 Trim 0 Register.
#define NVMNW_O_BANK1TRIM0                                          0x00000640

// Bank 1 Trim 1 Register.
#define NVMNW_O_BANK1TRIM1                                          0x00000644

// Bank 2 Trim 0 Register.
#define NVMNW_O_BANK2TRIM0                                          0x00000650

// Bank 2 Trim 1 Register.
#define NVMNW_O_BANK2TRIM1                                          0x00000654

// Bank 3 Trim 0 Register.
#define NVMNW_O_BANK3TRIM0                                          0x00000660

// Bank 3 Trim 1 Register.
#define NVMNW_O_BANK3TRIM1                                          0x00000664

// Bank 4 Trim 0 Register.
#define NVMNW_O_BANK4TRIM0                                          0x00000670

// Bank 4 Trim 1 Register.
#define NVMNW_O_BANK4TRIM1                                          0x00000674

//*****************************************************************************
//
// Register: NVMNW_O_IIDX
//
//*****************************************************************************
// Field:     [0] STAT
//
// Indicates which interrupt has fired. 0x0 means no event pending. The
// priority order is fixed. On each read, only one interrupt is indicated. On a
// read, the current interrupt (highest priority) is automatically cleared by
// the hardware and the corresponding interrupt flags in the RIS and MIS are
// cleared as well. After a read from the CPU (not from the debug interface),
// the register must be updated with the next highest priority interrupt.
// ENUMs:
// DONE                     DONE Interrupt Pending
// NO_INTR                  No Interrupt Pending
#define NVMNW_IIDX_STAT                                             0x00000001
#define NVMNW_IIDX_STAT_BITN                                                 0
#define NVMNW_IIDX_STAT_M                                           0x00000001
#define NVMNW_IIDX_STAT_S                                                    0
#define NVMNW_IIDX_STAT_DONE                                        0x00000001
#define NVMNW_IIDX_STAT_NO_INTR                                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_IMASK
//
//*****************************************************************************
// Field:     [0] DONE
//
// Interrupt mask for DONE:
// 0: Interrupt is disabled in MIS register
// 1: Interrupt is enabled in MIS register
// ENUMs:
// ENABLED                  Interrupt will request an interrupt service
//                          routine and corresponding bit in IPSTANDARD.MIS
//                          will be set
// DISABLED                 Interrupt is masked out
#define NVMNW_IMASK_DONE                                            0x00000001
#define NVMNW_IMASK_DONE_BITN                                                0
#define NVMNW_IMASK_DONE_M                                          0x00000001
#define NVMNW_IMASK_DONE_S                                                   0
#define NVMNW_IMASK_DONE_ENABLED                                    0x00000001
#define NVMNW_IMASK_DONE_DISABLED                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_RIS
//
//*****************************************************************************
// Field:     [0] DONE
//
// NoWrapper operation completed.
// This interrupt bit is set by firmware or the corresponding bit in the ISET
// register.
// It is cleared by the corresponding bit in in the ICLR register or reading
// the IIDX register when this interrupt is the highest priority.
// ENUMs:
// SET                      Interrupt occurred
// CLR                      Interrupt did not occur
#define NVMNW_RIS_DONE                                              0x00000001
#define NVMNW_RIS_DONE_BITN                                                  0
#define NVMNW_RIS_DONE_M                                            0x00000001
#define NVMNW_RIS_DONE_S                                                     0
#define NVMNW_RIS_DONE_SET                                          0x00000001
#define NVMNW_RIS_DONE_CLR                                          0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_MIS
//
//*****************************************************************************
// Field:     [0] DONE
//
// NoWrapper operation completed.
// This masked interrupt bit reflects the bitwise AND of the corresponding RIS
// and IMASK bits.
// ENUMs:
// SET                      Masked interrupt occurred
// CLR                      Masked interrupt did not occur
#define NVMNW_MIS_DONE                                              0x00000001
#define NVMNW_MIS_DONE_BITN                                                  0
#define NVMNW_MIS_DONE_M                                            0x00000001
#define NVMNW_MIS_DONE_S                                                     0
#define NVMNW_MIS_DONE_SET                                          0x00000001
#define NVMNW_MIS_DONE_CLR                                          0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_ISET
//
//*****************************************************************************
// Field:     [0] DONE
//
// 0: No effect
// 1: Set the DONE interrupt in the RIS register
// ENUMs:
// SET                      Set IPSTANDARD.RIS bit
// NO_EFFECT                Writing a 0 has no effect
#define NVMNW_ISET_DONE                                             0x00000001
#define NVMNW_ISET_DONE_BITN                                                 0
#define NVMNW_ISET_DONE_M                                           0x00000001
#define NVMNW_ISET_DONE_S                                                    0
#define NVMNW_ISET_DONE_SET                                         0x00000001
#define NVMNW_ISET_DONE_NO_EFFECT                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_ICLR
//
//*****************************************************************************
// Field:     [0] DONE
//
// 0: No effect
// 1: Clear the DONE interrupt in the RIS register
// ENUMs:
// CLR                      Clear IPSTANDARD.RIS bit
// NO_EFFECT                Writing a 0 has no effect
#define NVMNW_ICLR_DONE                                             0x00000001
#define NVMNW_ICLR_DONE_BITN                                                 0
#define NVMNW_ICLR_DONE_M                                           0x00000001
#define NVMNW_ICLR_DONE_S                                                    0
#define NVMNW_ICLR_DONE_CLR                                         0x00000001
#define NVMNW_ICLR_DONE_NO_EFFECT                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_EVT_MODE
//
//*****************************************************************************
// Field:   [1:0] INT0_CFG
//
// Event line mode select for peripheral event
// ENUMs:
// HARDWARE                 The interrupt or event line is in hardware mode.
//                          Hardware should clear the RIS.
// SOFTWARE                 The interrupt or event line is in software mode.
//                          Software must clear the RIS.
// DISABLE                  The interrupt or event line is disabled.
#define NVMNW_EVT_MODE_INT0_CFG_W                                            2
#define NVMNW_EVT_MODE_INT0_CFG_M                                   0x00000003
#define NVMNW_EVT_MODE_INT0_CFG_S                                            0
#define NVMNW_EVT_MODE_INT0_CFG_HARDWARE                            0x00000002
#define NVMNW_EVT_MODE_INT0_CFG_SOFTWARE                            0x00000001
#define NVMNW_EVT_MODE_INT0_CFG_DISABLE                             0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DESC
//
//*****************************************************************************
// Field: [31:16] MODULEID
//
// Module ID
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define NVMNW_DESC_MODULEID_W                                               16
#define NVMNW_DESC_MODULEID_M                                       0xFFFF0000
#define NVMNW_DESC_MODULEID_S                                               16
#define NVMNW_DESC_MODULEID_MAXIMUM                                 0xFFFF0000
#define NVMNW_DESC_MODULEID_MINIMUM                                 0x00000000

// Field: [15:12] FEATUREVER
//
// Feature set
// ENUMs:
// MAXIMUM                  Maximum Value
// MINIMUM                  Minimum Value
#define NVMNW_DESC_FEATUREVER_W                                              4
#define NVMNW_DESC_FEATUREVER_M                                     0x0000F000
#define NVMNW_DESC_FEATUREVER_S                                             12
#define NVMNW_DESC_FEATUREVER_MAXIMUM                               0x0000F000
#define NVMNW_DESC_FEATUREVER_MINIMUM                               0x00000000

// Field:  [11:8] INSTNUM
//
// Instance number
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define NVMNW_DESC_INSTNUM_W                                                 4
#define NVMNW_DESC_INSTNUM_M                                        0x00000F00
#define NVMNW_DESC_INSTNUM_S                                                 8
#define NVMNW_DESC_INSTNUM_MAXIMUM                                  0x00000F00
#define NVMNW_DESC_INSTNUM_MINIMUM                                  0x00000000

// Field:   [7:4] MAJREV
//
// Major Revision
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define NVMNW_DESC_MAJREV_W                                                  4
#define NVMNW_DESC_MAJREV_M                                         0x000000F0
#define NVMNW_DESC_MAJREV_S                                                  4
#define NVMNW_DESC_MAJREV_MAXIMUM                                   0x000000F0
#define NVMNW_DESC_MAJREV_MINIMUM                                   0x00000000

// Field:   [3:0] MINREV
//
// Minor Revision
// ENUMs:
// MAXIMUM                  Highest possible value
// MINIMUM                  Smallest value
#define NVMNW_DESC_MINREV_W                                                  4
#define NVMNW_DESC_MINREV_M                                         0x0000000F
#define NVMNW_DESC_MINREV_S                                                  0
#define NVMNW_DESC_MINREV_MAXIMUM                                   0x0000000F
#define NVMNW_DESC_MINREV_MINIMUM                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDEXEC
//
//*****************************************************************************
// Field:     [0] VAL
//
// Flash Wrapper State Machine Command Execute
// Initiates execution of the command specified in the FW_SMCMD_TYPE register.
// ENUMs:
// EXECUTE                  Command will execute or is executing in NoWrapper
// NOEXECUTE                Command will not execute or is not executing in
//                          NoWrapper
#define NVMNW_CMDEXEC_VAL                                           0x00000001
#define NVMNW_CMDEXEC_VAL_BITN                                               0
#define NVMNW_CMDEXEC_VAL_M                                         0x00000001
#define NVMNW_CMDEXEC_VAL_S                                                  0
#define NVMNW_CMDEXEC_VAL_EXECUTE                                   0x00000001
#define NVMNW_CMDEXEC_VAL_NOEXECUTE                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDTYPE
//
//*****************************************************************************
// Field:   [6:4] SIZE
//
// Command size
// ENUMs:
// BANK                     Operate on an entire flash bank
// SECTOR                   Operate on a flash sector
// EIGHTWORD                Operate on 8 flash words
// FOURWORD                 Operate on 4 flash words
// TWOWORD                  Operate on 2 flash words
// ONEWORD                  Operate on 1 flash word
#define NVMNW_CMDTYPE_SIZE_W                                                 3
#define NVMNW_CMDTYPE_SIZE_M                                        0x00000070
#define NVMNW_CMDTYPE_SIZE_S                                                 4
#define NVMNW_CMDTYPE_SIZE_BANK                                     0x00000050
#define NVMNW_CMDTYPE_SIZE_SECTOR                                   0x00000040
#define NVMNW_CMDTYPE_SIZE_EIGHTWORD                                0x00000030
#define NVMNW_CMDTYPE_SIZE_FOURWORD                                 0x00000020
#define NVMNW_CMDTYPE_SIZE_TWOWORD                                  0x00000010
#define NVMNW_CMDTYPE_SIZE_ONEWORD                                  0x00000000

// Field:   [2:0] COMMAND
//
// Command type
// ENUMs:
// CLEARSTATUS              Clear Status - Clear status bits in FW_SMSTAT
//                          only.
// MODECHANGE               Mode Change - Perform a mode change only, no other
//                          operation.
// READVERIFY               Read Verify - Perform a standalone read verify
//                          operation.
// ERASE                    Erase
// PROGRAM                  Program
// NOOP                     No Operation
#define NVMNW_CMDTYPE_COMMAND_W                                              3
#define NVMNW_CMDTYPE_COMMAND_M                                     0x00000007
#define NVMNW_CMDTYPE_COMMAND_S                                              0
#define NVMNW_CMDTYPE_COMMAND_CLEARSTATUS                           0x00000005
#define NVMNW_CMDTYPE_COMMAND_MODECHANGE                            0x00000004
#define NVMNW_CMDTYPE_COMMAND_READVERIFY                            0x00000003
#define NVMNW_CMDTYPE_COMMAND_ERASE                                 0x00000002
#define NVMNW_CMDTYPE_COMMAND_PROGRAM                               0x00000001
#define NVMNW_CMDTYPE_COMMAND_NOOP                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDCTL
//
//*****************************************************************************
// Field:    [21] DATAVEREN
//
// Enable invalid data verify.  This checks for 0->1 transitions in the memory
// when
// a program operation is initiated.  If such a transition is found, the
// program will
// fail with an error without doing any programming.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_CMDCTL_DATAVEREN                                      0x00200000
#define NVMNW_CMDCTL_DATAVEREN_BITN                                         21
#define NVMNW_CMDCTL_DATAVEREN_M                                    0x00200000
#define NVMNW_CMDCTL_DATAVEREN_S                                            21
#define NVMNW_CMDCTL_DATAVEREN_ENABLE                               0x00200000
#define NVMNW_CMDCTL_DATAVEREN_DISABLE                              0x00000000

// Field:    [20] SSERASEDIS
//
// Disable Stair-Step Erase.  If set, the default VHV trim voltage setting will
// be used
// for all erase pulses.
// By default, this bit is reset, meaning that the VHV voltage will be stepped
// during
// successive erase pulses.  The step count, step voltage, begin and end
// voltages
// are all hard-wired.
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_CMDCTL_SSERASEDIS                                     0x00100000
#define NVMNW_CMDCTL_SSERASEDIS_BITN                                        20
#define NVMNW_CMDCTL_SSERASEDIS_M                                   0x00100000
#define NVMNW_CMDCTL_SSERASEDIS_S                                           20
#define NVMNW_CMDCTL_SSERASEDIS_DISABLE                             0x00100000
#define NVMNW_CMDCTL_SSERASEDIS_ENABLE                              0x00000000

// Field:    [19] ERASEMASKDIS
//
// Disable use of erase mask for erase
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_CMDCTL_ERASEMASKDIS                                   0x00080000
#define NVMNW_CMDCTL_ERASEMASKDIS_BITN                                      19
#define NVMNW_CMDCTL_ERASEMASKDIS_M                                 0x00080000
#define NVMNW_CMDCTL_ERASEMASKDIS_S                                         19
#define NVMNW_CMDCTL_ERASEMASKDIS_DISABLE                           0x00080000
#define NVMNW_CMDCTL_ERASEMASKDIS_ENABLE                            0x00000000

// Field:    [18] PROGMASKDIS
//
// Disable use of program mask for programming
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_CMDCTL_PROGMASKDIS                                    0x00040000
#define NVMNW_CMDCTL_PROGMASKDIS_BITN                                       18
#define NVMNW_CMDCTL_PROGMASKDIS_M                                  0x00040000
#define NVMNW_CMDCTL_PROGMASKDIS_S                                          18
#define NVMNW_CMDCTL_PROGMASKDIS_DISABLE                            0x00040000
#define NVMNW_CMDCTL_PROGMASKDIS_ENABLE                             0x00000000

// Field:    [17] ECCGENOVR
//
// Override hardware generation of ECC data for program.  Use data written to
// FWSM_DATA_ECC.
// ENUMs:
// OVERRIDE                 Override
// NOOVERRIDE               Do not override
#define NVMNW_CMDCTL_ECCGENOVR                                      0x00020000
#define NVMNW_CMDCTL_ECCGENOVR_BITN                                         17
#define NVMNW_CMDCTL_ECCGENOVR_M                                    0x00020000
#define NVMNW_CMDCTL_ECCGENOVR_S                                            17
#define NVMNW_CMDCTL_ECCGENOVR_OVERRIDE                             0x00020000
#define NVMNW_CMDCTL_ECCGENOVR_NOOVERRIDE                           0x00000000

// Field:    [16] ADDRXLATEOVR
//
// Override hardware address translation of address in CMDADDR from a
// system address to a bank address and bank ID.  Use data written to
// CMDADDR directly as the bank address.  Use the value written to
// CMDCTL.BANKSEL directly as the bank ID.  Use the value written to
// CMDCTL.REGIONSEL directly as the region ID.
// ENUMs:
// OVERRIDE                 Override
// NOOVERRIDE               Do not override
#define NVMNW_CMDCTL_ADDRXLATEOVR                                   0x00010000
#define NVMNW_CMDCTL_ADDRXLATEOVR_BITN                                      16
#define NVMNW_CMDCTL_ADDRXLATEOVR_M                                 0x00010000
#define NVMNW_CMDCTL_ADDRXLATEOVR_S                                         16
#define NVMNW_CMDCTL_ADDRXLATEOVR_OVERRIDE                          0x00010000
#define NVMNW_CMDCTL_ADDRXLATEOVR_NOOVERRIDE                        0x00000000

// Field:    [15] POSTVEREN
//
// Enable verify after program or erase
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_CMDCTL_POSTVEREN                                      0x00008000
#define NVMNW_CMDCTL_POSTVEREN_BITN                                         15
#define NVMNW_CMDCTL_POSTVEREN_M                                    0x00008000
#define NVMNW_CMDCTL_POSTVEREN_S                                            15
#define NVMNW_CMDCTL_POSTVEREN_ENABLE                               0x00008000
#define NVMNW_CMDCTL_POSTVEREN_DISABLE                              0x00000000

// Field:    [14] PREVEREN
//
// Enable verify before program or erase.  For program, bits already
// programmed
// to the requested value will be masked.  For erase, sectors already erased
// will be
// masked.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_CMDCTL_PREVEREN                                       0x00004000
#define NVMNW_CMDCTL_PREVEREN_BITN                                          14
#define NVMNW_CMDCTL_PREVEREN_M                                     0x00004000
#define NVMNW_CMDCTL_PREVEREN_S                                             14
#define NVMNW_CMDCTL_PREVEREN_ENABLE                                0x00004000
#define NVMNW_CMDCTL_PREVEREN_DISABLE                               0x00000000

// Field:  [12:9] REGIONSEL
//
// Bank Region
// ENUMs:
// ENGR                     Engr Region
// TRIM                     Trim Region
// NONMAIN                  Non-Main Region
// MAIN                     Main Region
#define NVMNW_CMDCTL_REGIONSEL_W                                             4
#define NVMNW_CMDCTL_REGIONSEL_M                                    0x00001E00
#define NVMNW_CMDCTL_REGIONSEL_S                                             9
#define NVMNW_CMDCTL_REGIONSEL_ENGR                                 0x00001000
#define NVMNW_CMDCTL_REGIONSEL_TRIM                                 0x00000800
#define NVMNW_CMDCTL_REGIONSEL_NONMAIN                              0x00000400
#define NVMNW_CMDCTL_REGIONSEL_MAIN                                 0x00000200

// Field:     [4] BANKSEL
//
// Bank Select
// ENUMs:
// BANK4                    Bank 4
// BANK3                    Bank 3
// BANK2                    Bank 2
// BANK1                    Bank 1
// BANK0                    Bank 0
#define NVMNW_CMDCTL_BANKSEL                                        0x00000010
#define NVMNW_CMDCTL_BANKSEL_BITN                                            4
#define NVMNW_CMDCTL_BANKSEL_M                                      0x00000010
#define NVMNW_CMDCTL_BANKSEL_S                                               4
#define NVMNW_CMDCTL_BANKSEL_BANK4                                  0x00000100
#define NVMNW_CMDCTL_BANKSEL_BANK3                                  0x00000080
#define NVMNW_CMDCTL_BANKSEL_BANK2                                  0x00000040
#define NVMNW_CMDCTL_BANKSEL_BANK1                                  0x00000020
#define NVMNW_CMDCTL_BANKSEL_BANK0                                  0x00000010

// Field:   [3:0] MODESEL
//
// Mode
// This field is only used for the Mode Change command type.  Otherwise, bank
// and pump modes are set automaticlly via the NW hardware.
// ENUMs:
// ERASEBNK                 Erase Bank
// PGMMW                    Program Multiple Word
// LEAKVER                  Leakage Verify Mode
// ERASESECT                Erase Sector
// ERASEVER                 Erase Verify Mode
// PGMSW                    Program Single Word
// PGMVER                   Program Verify Mode
// NOOP                     No Operation
// RDMARG1B                 Read Margin 1B Mode
// RDMARG0B                 Read Margin 0B Mode
// RDMARG1                  Read Margin 1 Mode
// RDMARG0                  Read Margin 0 Mode
// READ                     Read Mode
#define NVMNW_CMDCTL_MODESEL_W                                               4
#define NVMNW_CMDCTL_MODESEL_M                                      0x0000000F
#define NVMNW_CMDCTL_MODESEL_S                                               0
#define NVMNW_CMDCTL_MODESEL_ERASEBNK                               0x0000000F
#define NVMNW_CMDCTL_MODESEL_PGMMW                                  0x0000000E
#define NVMNW_CMDCTL_MODESEL_LEAKVER                                0x0000000D
#define NVMNW_CMDCTL_MODESEL_ERASESECT                              0x0000000C
#define NVMNW_CMDCTL_MODESEL_ERASEVER                               0x0000000B
#define NVMNW_CMDCTL_MODESEL_PGMSW                                  0x0000000A
#define NVMNW_CMDCTL_MODESEL_PGMVER                                 0x00000009
#define NVMNW_CMDCTL_MODESEL_NOOP                                   0x00000008
#define NVMNW_CMDCTL_MODESEL_RDMARG1B                               0x00000007
#define NVMNW_CMDCTL_MODESEL_RDMARG0B                               0x00000006
#define NVMNW_CMDCTL_MODESEL_RDMARG1                                0x00000004
#define NVMNW_CMDCTL_MODESEL_RDMARG0                                0x00000002
#define NVMNW_CMDCTL_MODESEL_READ                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDADDR
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Flash Wrapper State Machine Command Address
// A 32-bit system address is placed in this register.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDADDR_VAL_W                                                 32
#define NVMNW_CMDADDR_VAL_M                                         0xFFFFFFFF
#define NVMNW_CMDADDR_VAL_S                                                  0
#define NVMNW_CMDADDR_VAL_MAXIMUM                                   0xFFFFFFFF
#define NVMNW_CMDADDR_VAL_MINIMUM                                   0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDBYTEN
//
//*****************************************************************************
// Field:   [7:0] VAL
//
// Flash Wrapper State Machine Command Program Mask
// A 1-bit per flash word byte value is placed in this register.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDBYTEN_VAL_W                                                 8
#define NVMNW_CMDBYTEN_VAL_M                                        0x000000FF
#define NVMNW_CMDBYTEN_VAL_S                                                 0
#define NVMNW_CMDBYTEN_VAL_MAXIMUM                                  0x0003FFFF
#define NVMNW_CMDBYTEN_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAINDEX
//
//*****************************************************************************
// Field:   [2:0] VAL
//
// Data register index
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATAINDEX_VAL_W                                             3
#define NVMNW_CMDDATAINDEX_VAL_M                                    0x00000007
#define NVMNW_CMDDATAINDEX_VAL_S                                             0
#define NVMNW_CMDDATAINDEX_VAL_MAXIMUM                              0x00000007
#define NVMNW_CMDDATAINDEX_VAL_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA0_VAL_W                                                32
#define NVMNW_CMDDATA0_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA0_VAL_S                                                 0
#define NVMNW_CMDDATA0_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA0_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA1_VAL_W                                                32
#define NVMNW_CMDDATA1_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA1_VAL_S                                                 0
#define NVMNW_CMDDATA1_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA1_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA2
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA2_VAL_W                                                32
#define NVMNW_CMDDATA2_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA2_VAL_S                                                 0
#define NVMNW_CMDDATA2_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA2_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA3
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA3_VAL_W                                                32
#define NVMNW_CMDDATA3_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA3_VAL_S                                                 0
#define NVMNW_CMDDATA3_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA3_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA4
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA4_VAL_W                                                32
#define NVMNW_CMDDATA4_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA4_VAL_S                                                 0
#define NVMNW_CMDDATA4_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA4_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA5
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA5_VAL_W                                                32
#define NVMNW_CMDDATA5_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA5_VAL_S                                                 0
#define NVMNW_CMDDATA5_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA5_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA6
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA6_VAL_W                                                32
#define NVMNW_CMDDATA6_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA6_VAL_S                                                 0
#define NVMNW_CMDDATA6_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA6_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA7
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA7_VAL_W                                                32
#define NVMNW_CMDDATA7_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA7_VAL_S                                                 0
#define NVMNW_CMDDATA7_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA7_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA8
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA8_VAL_W                                                32
#define NVMNW_CMDDATA8_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA8_VAL_S                                                 0
#define NVMNW_CMDDATA8_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA8_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA9
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA9_VAL_W                                                32
#define NVMNW_CMDDATA9_VAL_M                                        0xFFFFFFFF
#define NVMNW_CMDDATA9_VAL_S                                                 0
#define NVMNW_CMDDATA9_VAL_MAXIMUM                                  0xFFFFFFFF
#define NVMNW_CMDDATA9_VAL_MINIMUM                                  0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA10
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA10_VAL_W                                               32
#define NVMNW_CMDDATA10_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA10_VAL_S                                                0
#define NVMNW_CMDDATA10_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA10_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA11
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA11_VAL_W                                               32
#define NVMNW_CMDDATA11_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA11_VAL_S                                                0
#define NVMNW_CMDDATA11_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA11_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA12
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA12_VAL_W                                               32
#define NVMNW_CMDDATA12_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA12_VAL_S                                                0
#define NVMNW_CMDDATA12_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA12_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA13
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA13_VAL_W                                               32
#define NVMNW_CMDDATA13_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA13_VAL_S                                                0
#define NVMNW_CMDDATA13_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA13_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA14
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA14_VAL_W                                               32
#define NVMNW_CMDDATA14_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA14_VAL_S                                                0
#define NVMNW_CMDDATA14_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA14_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA15
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA15_VAL_W                                               32
#define NVMNW_CMDDATA15_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA15_VAL_S                                                0
#define NVMNW_CMDDATA15_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA15_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA16
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA16_VAL_W                                               32
#define NVMNW_CMDDATA16_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA16_VAL_S                                                0
#define NVMNW_CMDDATA16_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA16_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA17
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA17_VAL_W                                               32
#define NVMNW_CMDDATA17_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA17_VAL_S                                                0
#define NVMNW_CMDDATA17_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA17_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA18
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA18_VAL_W                                               32
#define NVMNW_CMDDATA18_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA18_VAL_S                                                0
#define NVMNW_CMDDATA18_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA18_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA19
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA19_VAL_W                                               32
#define NVMNW_CMDDATA19_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA19_VAL_S                                                0
#define NVMNW_CMDDATA19_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA19_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA20
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA20_VAL_W                                               32
#define NVMNW_CMDDATA20_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA20_VAL_S                                                0
#define NVMNW_CMDDATA20_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA20_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA21
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA21_VAL_W                                               32
#define NVMNW_CMDDATA21_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA21_VAL_S                                                0
#define NVMNW_CMDDATA21_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA21_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA22
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA22_VAL_W                                               32
#define NVMNW_CMDDATA22_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA22_VAL_S                                                0
#define NVMNW_CMDDATA22_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA22_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA23
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA23_VAL_W                                               32
#define NVMNW_CMDDATA23_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA23_VAL_S                                                0
#define NVMNW_CMDDATA23_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA23_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA24
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA24_VAL_W                                               32
#define NVMNW_CMDDATA24_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA24_VAL_S                                                0
#define NVMNW_CMDDATA24_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA24_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA25
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA25_VAL_W                                               32
#define NVMNW_CMDDATA25_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA25_VAL_S                                                0
#define NVMNW_CMDDATA25_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA25_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA26
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA26_VAL_W                                               32
#define NVMNW_CMDDATA26_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA26_VAL_S                                                0
#define NVMNW_CMDDATA26_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA26_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA27
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA27_VAL_W                                               32
#define NVMNW_CMDDATA27_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA27_VAL_S                                                0
#define NVMNW_CMDDATA27_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA27_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA28
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA28_VAL_W                                               32
#define NVMNW_CMDDATA28_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA28_VAL_S                                                0
#define NVMNW_CMDDATA28_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA28_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA29
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA29_VAL_W                                               32
#define NVMNW_CMDDATA29_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA29_VAL_S                                                0
#define NVMNW_CMDDATA29_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA29_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA30
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA30_VAL_W                                               32
#define NVMNW_CMDDATA30_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA30_VAL_S                                                0
#define NVMNW_CMDDATA30_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA30_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATA31
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// A 32-bit data value is placed in this field.
// This field is blocked for writes after being written to 1 and prior to
// FW_SMCMT_STAT.DONE being set by the NoWrapper hardware.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDDATA31_VAL_W                                               32
#define NVMNW_CMDDATA31_VAL_M                                       0xFFFFFFFF
#define NVMNW_CMDDATA31_VAL_S                                                0
#define NVMNW_CMDDATA31_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_CMDDATA31_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC0
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC0_VAL1_W                                             8
#define NVMNW_CMDDATAECC0_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC0_VAL1_S                                             8
#define NVMNW_CMDDATAECC0_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC0_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC0_VAL0_W                                             8
#define NVMNW_CMDDATAECC0_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC0_VAL0_S                                             0
#define NVMNW_CMDDATAECC0_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC0_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC1
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC1_VAL1_W                                             8
#define NVMNW_CMDDATAECC1_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC1_VAL1_S                                             8
#define NVMNW_CMDDATAECC1_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC1_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC1_VAL0_W                                             8
#define NVMNW_CMDDATAECC1_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC1_VAL0_S                                             0
#define NVMNW_CMDDATAECC1_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC1_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC2
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC2_VAL1_W                                             8
#define NVMNW_CMDDATAECC2_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC2_VAL1_S                                             8
#define NVMNW_CMDDATAECC2_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC2_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC2_VAL0_W                                             8
#define NVMNW_CMDDATAECC2_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC2_VAL0_S                                             0
#define NVMNW_CMDDATAECC2_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC2_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC3
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC3_VAL1_W                                             8
#define NVMNW_CMDDATAECC3_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC3_VAL1_S                                             8
#define NVMNW_CMDDATAECC3_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC3_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC3_VAL0_W                                             8
#define NVMNW_CMDDATAECC3_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC3_VAL0_S                                             0
#define NVMNW_CMDDATAECC3_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC3_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC4
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC4_VAL1_W                                             8
#define NVMNW_CMDDATAECC4_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC4_VAL1_S                                             8
#define NVMNW_CMDDATAECC4_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC4_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC4_VAL0_W                                             8
#define NVMNW_CMDDATAECC4_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC4_VAL0_S                                             0
#define NVMNW_CMDDATAECC4_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC4_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC5
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC5_VAL1_W                                             8
#define NVMNW_CMDDATAECC5_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC5_VAL1_S                                             8
#define NVMNW_CMDDATAECC5_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC5_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC5_VAL0_W                                             8
#define NVMNW_CMDDATAECC5_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC5_VAL0_S                                             0
#define NVMNW_CMDDATAECC5_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC5_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC6
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC6_VAL1_W                                             8
#define NVMNW_CMDDATAECC6_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC6_VAL1_S                                             8
#define NVMNW_CMDDATAECC6_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC6_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC6_VAL0_W                                             8
#define NVMNW_CMDDATAECC6_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC6_VAL0_S                                             0
#define NVMNW_CMDDATAECC6_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC6_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDDATAECC7
//
//*****************************************************************************
// Field:  [15:8] VAL1
//
// ECC data for bits 127:64 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC7_VAL1_W                                             8
#define NVMNW_CMDDATAECC7_VAL1_M                                    0x0000FF00
#define NVMNW_CMDDATAECC7_VAL1_S                                             8
#define NVMNW_CMDDATAECC7_VAL1_MAXIMUM                              0x0000FF00
#define NVMNW_CMDDATAECC7_VAL1_MINIMUM                              0x00000000

// Field:   [7:0] VAL0
//
// ECC data for bits 63:0 of the data is placed here.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CMDDATAECC7_VAL0_W                                             8
#define NVMNW_CMDDATAECC7_VAL0_M                                    0x000000FF
#define NVMNW_CMDDATAECC7_VAL0_S                                             0
#define NVMNW_CMDDATAECC7_VAL0_MAXIMUM                              0x000000FF
#define NVMNW_CMDDATAECC7_VAL0_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTA
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects 1 sector.
//
// bit [0]:	When 1, sector 0 of the flash memory will be protected from
// program
// 		and erase.
// bit [1]:	When 1, sector 1 of the flash memory will be protected from
// program
// 		and erase.
// 	:
// 	:
// bit [31]:	When 1, sector 31 of the flash memory will be protected from
// program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTA_VAL_W                                              32
#define NVMNW_CMDWEPROTA_VAL_M                                      0xFFFFFFFF
#define NVMNW_CMDWEPROTA_VAL_S                                               0
#define NVMNW_CMDWEPROTA_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_CMDWEPROTA_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTB
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects a group of 8 sectors.
//
// bit [0]:	When 1, sectors 0-7 of the flash memory will be protected from
// program
// 		and erase.
// bit [1]:	When 1, sectors 8-15 of the flash memory will be protected from
// program
// 		and erase.
// 	:
// 	:
// bit [31]:	When 1, sectors 248-255 of the flash memory will be protected from
// program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTB_VAL_W                                              32
#define NVMNW_CMDWEPROTB_VAL_M                                      0xFFFFFFFF
#define NVMNW_CMDWEPROTB_VAL_S                                               0
#define NVMNW_CMDWEPROTB_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_CMDWEPROTB_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTC
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects a group of 8 sectors.
//
// bit [0]:	When 1, sectors 256-263 of the main region will be protected from
// program
// 		and erase.
// bit [1]:	When 1, sectors 264-271 of the main region will be protected from
// program
// 		and erase.
// 	:
// 	:
// bit [31]:	When 1, sectors 504-511 of the main region will be protected from
// program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTC_VAL_W                                              32
#define NVMNW_CMDWEPROTC_VAL_M                                      0xFFFFFFFF
#define NVMNW_CMDWEPROTC_VAL_S                                               0
#define NVMNW_CMDWEPROTC_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_CMDWEPROTC_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTNM
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects 1 sector.
//
// bit [0]:	When 1, sector 0 of the non-main region will be protected from
// program
// 		and erase.
// bit [1]:	When 1, sector 1 of the non-main region will be protected from
// program
// 		and erase.
// 	:
// 	:
// bit [16]:	When 1, sector 15 of the non-main will be protected from program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTNM_VAL_W                                             32
#define NVMNW_CMDWEPROTNM_VAL_M                                     0xFFFFFFFF
#define NVMNW_CMDWEPROTNM_VAL_S                                              0
#define NVMNW_CMDWEPROTNM_VAL_MAXIMUM                               0xFFFFFFFF
#define NVMNW_CMDWEPROTNM_VAL_MINIMUM                               0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTTR
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects 1 sector.
//
// bit [0]:	When 1, sector 0 of the engr region will be protected from program
// 		and erase.
// bit [1]:	When 1, sector 1 of the engr region will be protected from program
// 		and erase.
// 	:
// 	:
// bit [16]:	When 1, sector 15 of the engr region will be protected from
// program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTTR_VAL_W                                             32
#define NVMNW_CMDWEPROTTR_VAL_M                                     0xFFFFFFFF
#define NVMNW_CMDWEPROTTR_VAL_S                                              0
#define NVMNW_CMDWEPROTTR_VAL_MAXIMUM                               0xFFFFFFFF
#define NVMNW_CMDWEPROTTR_VAL_MINIMUM                               0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CMDWEPROTEN
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Each bit protects 1 sector.
//
// bit [0]:	When 1, sector 0 of the engr region will be protected from program
// 		and erase.
// bit [1]:	When 1, sector 1 of the engr region will be protected from program
// 		and erase.
// 	:
// 	:
// bit [16]:	When 1, sector 15 of the engr region will be protected from
// program
// 		and erase.
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_CMDWEPROTEN_VAL_W                                             32
#define NVMNW_CMDWEPROTEN_VAL_M                                     0xFFFFFFFF
#define NVMNW_CMDWEPROTEN_VAL_S                                              0
#define NVMNW_CMDWEPROTEN_VAL_MAXIMUM                               0xFFFFFFFF
#define NVMNW_CMDWEPROTEN_VAL_MINIMUM                               0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CFGCMD
//
//*****************************************************************************
// Field:   [3:0] WAITSTATE
//
// Wait State setting for read verify
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CFGCMD_WAITSTATE_W                                             4
#define NVMNW_CFGCMD_WAITSTATE_M                                    0x0000000F
#define NVMNW_CFGCMD_WAITSTATE_S                                             0
#define NVMNW_CFGCMD_WAITSTATE_MAXIMUM                              0x0000000F
#define NVMNW_CFGCMD_WAITSTATE_MINIMUM                              0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_CFGPCNT
//
//*****************************************************************************
// Field:  [11:4] MAXPCNTVAL
//
// Override maximum pulse counter with this value.  Full max value will be
// {4'h0, MAXPCNTVAL}
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_CFGPCNT_MAXPCNTVAL_W                                           8
#define NVMNW_CFGPCNT_MAXPCNTVAL_M                                  0x00000FF0
#define NVMNW_CFGPCNT_MAXPCNTVAL_S                                           4
#define NVMNW_CFGPCNT_MAXPCNTVAL_MAXIMUM                            0x00000FF0
#define NVMNW_CFGPCNT_MAXPCNTVAL_MINIMUM                            0x00000000

// Field:     [0] MAXPCNTOVR
//
// Override hard-wired maximum pulse count for program or erase
// Enumeration:
// 0: Use hard-wired (default) maximum pulse count
// 1: Use value from MAX_PCNT field as maximum pulse count
// ENUMs:
// OVERRIDE                 Use value from MAX_PCNT_OVR field as maximum puse
//                          count
// DEFAULT                  Use hard-wired (default) value for maximum pulse
//                          count
#define NVMNW_CFGPCNT_MAXPCNTOVR                                    0x00000001
#define NVMNW_CFGPCNT_MAXPCNTOVR_BITN                                        0
#define NVMNW_CFGPCNT_MAXPCNTOVR_M                                  0x00000001
#define NVMNW_CFGPCNT_MAXPCNTOVR_S                                           0
#define NVMNW_CFGPCNT_MAXPCNTOVR_OVERRIDE                           0x00000001
#define NVMNW_CFGPCNT_MAXPCNTOVR_DEFAULT                            0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_STATCMD
//
//*****************************************************************************
// Field:    [12] FAILMISC
//
// Command failed due to error other than write/erase protect violation or
// verify
// error.  This is an extra bit in case a new failure mechanism is added which
// requires a status bit.
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILMISC                                      0x00001000
#define NVMNW_STATCMD_FAILMISC_BITN                                         12
#define NVMNW_STATCMD_FAILMISC_M                                    0x00001000
#define NVMNW_STATCMD_FAILMISC_S                                            12
#define NVMNW_STATCMD_FAILMISC_STATFAIL                             0x00001000
#define NVMNW_STATCMD_FAILMISC_STATNOFAIL                           0x00000000

// Field:     [8] FAILINVDATA
//
// Program command failed because an attempt was made to program a stored
// 0 value to a 1.
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILINVDATA                                   0x00000100
#define NVMNW_STATCMD_FAILINVDATA_BITN                                       8
#define NVMNW_STATCMD_FAILINVDATA_M                                 0x00000100
#define NVMNW_STATCMD_FAILINVDATA_S                                          8
#define NVMNW_STATCMD_FAILINVDATA_STATFAIL                          0x00000100
#define NVMNW_STATCMD_FAILINVDATA_STATNOFAIL                        0x00000000

// Field:     [7] FAILMODE
//
// Command failed because a bank has been set to a mode other than READ.
// Program and Erase commands cannot be initiated unless all banks are in READ
// mode.
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILMODE                                      0x00000080
#define NVMNW_STATCMD_FAILMODE_BITN                                          7
#define NVMNW_STATCMD_FAILMODE_M                                    0x00000080
#define NVMNW_STATCMD_FAILMODE_S                                             7
#define NVMNW_STATCMD_FAILMODE_STATFAIL                             0x00000080
#define NVMNW_STATCMD_FAILMODE_STATNOFAIL                           0x00000000

// Field:     [6] FAILILLADDR
//
// Command failed due to the use of an illegal address
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILILLADDR                                   0x00000040
#define NVMNW_STATCMD_FAILILLADDR_BITN                                       6
#define NVMNW_STATCMD_FAILILLADDR_M                                 0x00000040
#define NVMNW_STATCMD_FAILILLADDR_S                                          6
#define NVMNW_STATCMD_FAILILLADDR_STATFAIL                          0x00000040
#define NVMNW_STATCMD_FAILILLADDR_STATNOFAIL                        0x00000000

// Field:     [5] FAILVERIFY
//
// Command failed due to verify error
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILVERIFY                                    0x00000020
#define NVMNW_STATCMD_FAILVERIFY_BITN                                        5
#define NVMNW_STATCMD_FAILVERIFY_M                                  0x00000020
#define NVMNW_STATCMD_FAILVERIFY_S                                           5
#define NVMNW_STATCMD_FAILVERIFY_STATFAIL                           0x00000020
#define NVMNW_STATCMD_FAILVERIFY_STATNOFAIL                         0x00000000

// Field:     [4] FAILWEPROT
//
// Command failed due to Write/Erase Protect Sector Violation
// ENUMs:
// STATFAIL                 Fail
// STATNOFAIL               No Fail
#define NVMNW_STATCMD_FAILWEPROT                                    0x00000010
#define NVMNW_STATCMD_FAILWEPROT_BITN                                        4
#define NVMNW_STATCMD_FAILWEPROT_M                                  0x00000010
#define NVMNW_STATCMD_FAILWEPROT_S                                           4
#define NVMNW_STATCMD_FAILWEPROT_STATFAIL                           0x00000010
#define NVMNW_STATCMD_FAILWEPROT_STATNOFAIL                         0x00000000

// Field:     [2] CMDINPROGRESS
//
// Command In Progress
// ENUMs:
// STATINPROGRESS           In Progress
// STATCOMPLETE             Complete
#define NVMNW_STATCMD_CMDINPROGRESS                                 0x00000004
#define NVMNW_STATCMD_CMDINPROGRESS_BITN                                     2
#define NVMNW_STATCMD_CMDINPROGRESS_M                               0x00000004
#define NVMNW_STATCMD_CMDINPROGRESS_S                                        2
#define NVMNW_STATCMD_CMDINPROGRESS_STATINPROGRESS                  0x00000004
#define NVMNW_STATCMD_CMDINPROGRESS_STATCOMPLETE                    0x00000000

// Field:     [1] CMDPASS
//
// Command Pass - valid when CMD_DONE field is 1
// ENUMs:
// STATPASS                 Pass
// STATFAIL                 Fail
#define NVMNW_STATCMD_CMDPASS                                       0x00000002
#define NVMNW_STATCMD_CMDPASS_BITN                                           1
#define NVMNW_STATCMD_CMDPASS_M                                     0x00000002
#define NVMNW_STATCMD_CMDPASS_S                                              1
#define NVMNW_STATCMD_CMDPASS_STATPASS                              0x00000002
#define NVMNW_STATCMD_CMDPASS_STATFAIL                              0x00000000

// Field:     [0] CMDDONE
//
// Command Done
// ENUMs:
// STATDONE                 Done
// STATNOTDONE              Not Done
#define NVMNW_STATCMD_CMDDONE                                       0x00000001
#define NVMNW_STATCMD_CMDDONE_BITN                                           0
#define NVMNW_STATCMD_CMDDONE_M                                     0x00000001
#define NVMNW_STATCMD_CMDDONE_S                                              0
#define NVMNW_STATCMD_CMDDONE_STATDONE                              0x00000001
#define NVMNW_STATCMD_CMDDONE_STATNOTDONE                           0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_STATADDR
//
//*****************************************************************************
// Field: [25:21] BANKID
//
// Current Bank ID
// A bank indicator is stored in this register which represents the current
// bank on
// which the state  machine is operating.  There is 1 bit per bank.
// ENUMs:
// BANK4                    Bank 4
// BANK3                    Bank 3
// BANK2                    Bank 2
// BANK1                    Bank 1
// BANK0                    Bank 0
#define NVMNW_STATADDR_BANKID_W                                              5
#define NVMNW_STATADDR_BANKID_M                                     0x03E00000
#define NVMNW_STATADDR_BANKID_S                                             21
#define NVMNW_STATADDR_BANKID_BANK4                                 0x02000000
#define NVMNW_STATADDR_BANKID_BANK3                                 0x01000000
#define NVMNW_STATADDR_BANKID_BANK2                                 0x00800000
#define NVMNW_STATADDR_BANKID_BANK1                                 0x00400000
#define NVMNW_STATADDR_BANKID_BANK0                                 0x00200000

// Field: [20:16] REGIONID
//
// Current Region ID
// A region indicator is stored in this register which represents the current
// flash
// region on which the state  machine is operating.
// ENUMs:
// ENGR                     Engr Region
// TRIM                     Trim Region
// NONMAIN                  Non-Main Region
// MAIN                     Main Region
#define NVMNW_STATADDR_REGIONID_W                                            5
#define NVMNW_STATADDR_REGIONID_M                                   0x001F0000
#define NVMNW_STATADDR_REGIONID_S                                           16
#define NVMNW_STATADDR_REGIONID_ENGR                                0x00080000
#define NVMNW_STATADDR_REGIONID_TRIM                                0x00040000
#define NVMNW_STATADDR_REGIONID_NONMAIN                             0x00020000
#define NVMNW_STATADDR_REGIONID_MAIN                                0x00010000

// Field:  [15:0] BANKADDR
//
// Current Bank Address
// A bank offset address is stored in this register.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_STATADDR_BANKADDR_W                                           16
#define NVMNW_STATADDR_BANKADDR_M                                   0x0000FFFF
#define NVMNW_STATADDR_BANKADDR_S                                            0
#define NVMNW_STATADDR_BANKADDR_MAXIMUM                             0x0000FFFF
#define NVMNW_STATADDR_BANKADDR_MINIMUM                             0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_STATPCNT
//
//*****************************************************************************
// Field:  [11:0] PULSECNT
//
// Current Pulse Counter Value
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_STATPCNT_PULSECNT_W                                           12
#define NVMNW_STATPCNT_PULSECNT_M                                   0x00000FFF
#define NVMNW_STATPCNT_PULSECNT_S                                            0
#define NVMNW_STATPCNT_PULSECNT_MAXIMUM                             0x00000FFF
#define NVMNW_STATPCNT_PULSECNT_MINIMUM                             0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_STATMODE
//
//*****************************************************************************
// Field:  [11:8] BANKMODE
//
// Indicates mode of bank(s) that are not in READ mode
// ENUMs:
// ERASEBNK                 Erase Bank
// PGMMW                    Program Multiple Word
// LEAKVER                  Leakage Verify Mode
// ERASESECT                Erase Sector
// ERASEVER                 Erase Verify Mode
// PGMSW                    Program Single Word
// PGMVER                   Program Verify Mode
// NOOP                     No Operation
// RDMARG1B                 Read Margin 1B Mode
// RDMARG0B                 Read Margin 0B Mode
// RDMARG1                  Read Margin 1 Mode
// RDMARG0                  Read Margin 0 Mode
// READ                     Read Mode
#define NVMNW_STATMODE_BANKMODE_W                                            4
#define NVMNW_STATMODE_BANKMODE_M                                   0x00000F00
#define NVMNW_STATMODE_BANKMODE_S                                            8
#define NVMNW_STATMODE_BANKMODE_ERASEBNK                            0x00000F00
#define NVMNW_STATMODE_BANKMODE_PGMMW                               0x00000E00
#define NVMNW_STATMODE_BANKMODE_LEAKVER                             0x00000D00
#define NVMNW_STATMODE_BANKMODE_ERASESECT                           0x00000C00
#define NVMNW_STATMODE_BANKMODE_ERASEVER                            0x00000B00
#define NVMNW_STATMODE_BANKMODE_PGMSW                               0x00000A00
#define NVMNW_STATMODE_BANKMODE_PGMVER                              0x00000900
#define NVMNW_STATMODE_BANKMODE_NOOP                                0x00000800
#define NVMNW_STATMODE_BANKMODE_RDMARG1B                            0x00000700
#define NVMNW_STATMODE_BANKMODE_RDMARG0B                            0x00000600
#define NVMNW_STATMODE_BANKMODE_RDMARG1                             0x00000400
#define NVMNW_STATMODE_BANKMODE_RDMARG0                             0x00000200
#define NVMNW_STATMODE_BANKMODE_READ                                0x00000000

// Field:     [0] BANKNOTINRD
//
// Bank not in read mode.
// Indicates which banks are not in READ mode.  There is 1 bit per bank.
// ENUMs:
// BANK4                    Bank 4
// BANK3                    Bank 3
// BANK2                    Bank 2
// BANK1                    Bank 1
// BANK0                    Bank 0
#define NVMNW_STATMODE_BANKNOTINRD                                  0x00000001
#define NVMNW_STATMODE_BANKNOTINRD_BITN                                      0
#define NVMNW_STATMODE_BANKNOTINRD_M                                0x00000001
#define NVMNW_STATMODE_BANKNOTINRD_S                                         0
#define NVMNW_STATMODE_BANKNOTINRD_BANK4                            0x00000010
#define NVMNW_STATMODE_BANKNOTINRD_BANK3                            0x00000008
#define NVMNW_STATMODE_BANKNOTINRD_BANK2                            0x00000004
#define NVMNW_STATMODE_BANKNOTINRD_BANK1                            0x00000002
#define NVMNW_STATMODE_BANKNOTINRD_BANK0                            0x00000001

//*****************************************************************************
//
// Register: NVMNW_O_GBLINFO0
//
//*****************************************************************************
// Field: [18:16] NUMBANKS
//
// Number of banks instantiated
// Minimum:	1
// Maximum:	5
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_GBLINFO0_NUMBANKS_W                                            3
#define NVMNW_GBLINFO0_NUMBANKS_M                                   0x00070000
#define NVMNW_GBLINFO0_NUMBANKS_S                                           16
#define NVMNW_GBLINFO0_NUMBANKS_MAXIMUM                             0x00050000
#define NVMNW_GBLINFO0_NUMBANKS_MINIMUM                             0x00010000

// Field:  [15:0] SECTORSIZE
//
// Sector size in bytes
// ENUMs:
// TWOKB                    Sector size is TWOKB
// ONEKB                    Sector size is ONEKB
#define NVMNW_GBLINFO0_SECTORSIZE_W                                         16
#define NVMNW_GBLINFO0_SECTORSIZE_M                                 0x0000FFFF
#define NVMNW_GBLINFO0_SECTORSIZE_S                                          0
#define NVMNW_GBLINFO0_SECTORSIZE_TWOKB                             0x00000800
#define NVMNW_GBLINFO0_SECTORSIZE_ONEKB                             0x00000400

//*****************************************************************************
//
// Register: NVMNW_O_GBLINFO1
//
//*****************************************************************************
// Field: [18:16] REDWIDTH
//
// Redundant data width in bits
// ENUMs:
// W4BIT                    Redundant data width is 4 bits
// W2BIT                    Redundant data width is 2 bits
// W0BIT                    Redundant data width is 0.  Redundancy/Repair not
//                          present.
#define NVMNW_GBLINFO1_REDWIDTH_W                                            3
#define NVMNW_GBLINFO1_REDWIDTH_M                                   0x00070000
#define NVMNW_GBLINFO1_REDWIDTH_S                                           16
#define NVMNW_GBLINFO1_REDWIDTH_W4BIT                               0x00040000
#define NVMNW_GBLINFO1_REDWIDTH_W2BIT                               0x00020000
#define NVMNW_GBLINFO1_REDWIDTH_W0BIT                               0x00000000

// Field:  [12:8] ECCWIDTH
//
// ECC data width in bits
// ENUMs:
// W16BIT                   ECC data width is 16 bits
// W8BIT                    ECC data width is 8 bits
// W0BIT                    ECC data width is 0.  ECC not used.
#define NVMNW_GBLINFO1_ECCWIDTH_W                                            5
#define NVMNW_GBLINFO1_ECCWIDTH_M                                   0x00001F00
#define NVMNW_GBLINFO1_ECCWIDTH_S                                            8
#define NVMNW_GBLINFO1_ECCWIDTH_W16BIT                              0x00001000
#define NVMNW_GBLINFO1_ECCWIDTH_W8BIT                               0x00000800
#define NVMNW_GBLINFO1_ECCWIDTH_W0BIT                               0x00000000

// Field:   [7:0] DATAWIDTH
//
// Data width in bits
// ENUMs:
// W128BIT                  Data width is 128 bits
// W64BIT                   Data width is 64 bits
#define NVMNW_GBLINFO1_DATAWIDTH_W                                           8
#define NVMNW_GBLINFO1_DATAWIDTH_M                                  0x000000FF
#define NVMNW_GBLINFO1_DATAWIDTH_S                                           0
#define NVMNW_GBLINFO1_DATAWIDTH_W128BIT                            0x00000080
#define NVMNW_GBLINFO1_DATAWIDTH_W64BIT                             0x00000040

//*****************************************************************************
//
// Register: NVMNW_O_BANK0INFO0
//
//*****************************************************************************
// Field:  [11:0] MAINSIZE
//
// Main region size in sectors
// Minimum:	0x8 (8)
// Maximum:	0x200 (512)
// ENUMs:
// MAXSECTORS               Maximum value of MAINSIZE
// MINSECTORS               Minimum value of MAINSIZE
#define NVMNW_BANK0INFO0_MAINSIZE_W                                         12
#define NVMNW_BANK0INFO0_MAINSIZE_M                                 0x00000FFF
#define NVMNW_BANK0INFO0_MAINSIZE_S                                          0
#define NVMNW_BANK0INFO0_MAINSIZE_MAXSECTORS                        0x00000200
#define NVMNW_BANK0INFO0_MAINSIZE_MINSECTORS                        0x00000008

//*****************************************************************************
//
// Register: NVMNW_O_BANK0INFO1
//
//*****************************************************************************
// Field: [23:16] ENGRSIZE
//
// Engr region size in sectors
// Minimum:	0x0 (0)
// Maximum:	0x10 (16)
// ENUMs:
// MAXSECTORS               Maximum value of ENGRSIZE
// MINSECTORS               Minimum value of ENGRSIZE
#define NVMNW_BANK0INFO1_ENGRSIZE_W                                          8
#define NVMNW_BANK0INFO1_ENGRSIZE_M                                 0x00FF0000
#define NVMNW_BANK0INFO1_ENGRSIZE_S                                         16
#define NVMNW_BANK0INFO1_ENGRSIZE_MAXSECTORS                        0x00200000
#define NVMNW_BANK0INFO1_ENGRSIZE_MINSECTORS                        0x00000000

// Field:  [15:8] TRIMSIZE
//
// Trim region size in sectors
// Minimum:	0x0 (0)
// Maximum:	0x10 (16)
// ENUMs:
// MAXSECTORS               Maximum value of TRIMSIZE
// MINSECTORS               Minimum value of TRIMSIZE
#define NVMNW_BANK0INFO1_TRIMSIZE_W                                          8
#define NVMNW_BANK0INFO1_TRIMSIZE_M                                 0x0000FF00
#define NVMNW_BANK0INFO1_TRIMSIZE_S                                          8
#define NVMNW_BANK0INFO1_TRIMSIZE_MAXSECTORS                        0x00002000
#define NVMNW_BANK0INFO1_TRIMSIZE_MINSECTORS                        0x00000000

// Field:   [7:0] NONMAINSIZE
//
// Non-main region size in sectors
// Minimum:	0x0 (0)
// Maximum:	0x10 (16)
// ENUMs:
// MAXSECTORS               Maximum value of NONMAINSIZE
// MINSECTORS               Minimum value of NONMAINSIZE
#define NVMNW_BANK0INFO1_NONMAINSIZE_W                                       8
#define NVMNW_BANK0INFO1_NONMAINSIZE_M                              0x000000FF
#define NVMNW_BANK0INFO1_NONMAINSIZE_S                                       0
#define NVMNW_BANK0INFO1_NONMAINSIZE_MAXSECTORS                     0x00000020
#define NVMNW_BANK0INFO1_NONMAINSIZE_MINSECTORS                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK1INFO0
//
//*****************************************************************************
// Field:  [11:0] MAINSIZE
//
// Main region size in sectors
// Minimum:	0x8 (8)
// Maximum:	0x200 (512)
// ENUMs:
// MAXSECTORS               Maximum value of MAINSIZE
// MINSECTORS               Minimum value of MAINSIZE
#define NVMNW_BANK1INFO0_MAINSIZE_W                                         12
#define NVMNW_BANK1INFO0_MAINSIZE_M                                 0x00000FFF
#define NVMNW_BANK1INFO0_MAINSIZE_S                                          0
#define NVMNW_BANK1INFO0_MAINSIZE_MAXSECTORS                        0x00000200
#define NVMNW_BANK1INFO0_MAINSIZE_MINSECTORS                        0x00000008

//*****************************************************************************
//
// Register: NVMNW_O_BANK1INFO1
//
//*****************************************************************************
// Field: [23:16] ENGRSIZE
//
// Engr region size in sectors
// Minimum:	0x0 (0)
// Maximum:	0x10 (16)
// ENUMs:
// MAXSECTORS               Maximum value of ENGRSIZE
// MINSECTORS               Minimum value of ENGRSIZE
#define NVMNW_BANK1INFO1_ENGRSIZE_W                                          8
#define NVMNW_BANK1INFO1_ENGRSIZE_M                                 0x00FF0000
#define NVMNW_BANK1INFO1_ENGRSIZE_S                                         16
#define NVMNW_BANK1INFO1_ENGRSIZE_MAXSECTORS                        0x00200000
#define NVMNW_BANK1INFO1_ENGRSIZE_MINSECTORS                        0x00000000

// Field:  [15:8] TRIMSIZE
//
// Trim region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of TRIMSIZE
// MINSECTORS               Minimum value of TRIMSIZE
#define NVMNW_BANK1INFO1_TRIMSIZE_W                                          8
#define NVMNW_BANK1INFO1_TRIMSIZE_M                                 0x0000FF00
#define NVMNW_BANK1INFO1_TRIMSIZE_S                                          8
#define NVMNW_BANK1INFO1_TRIMSIZE_MAXSECTORS                        0x00002000
#define NVMNW_BANK1INFO1_TRIMSIZE_MINSECTORS                        0x00000000

// Field:   [7:0] NONMAINSIZE
//
// Non-main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of NONMAINSIZE
// MINSECTORS               Minimum value of NONMAINSIZE
#define NVMNW_BANK1INFO1_NONMAINSIZE_W                                       8
#define NVMNW_BANK1INFO1_NONMAINSIZE_M                              0x000000FF
#define NVMNW_BANK1INFO1_NONMAINSIZE_S                                       0
#define NVMNW_BANK1INFO1_NONMAINSIZE_MAXSECTORS                     0x00000020
#define NVMNW_BANK1INFO1_NONMAINSIZE_MINSECTORS                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK2INFO0
//
//*****************************************************************************
// Field:  [11:0] MAINSIZE
//
// Main region size in sectors
// Minimum:	0x8 (8)
// Maximum:	0x200 (512)
// ENUMs:
// MAXSECTORS               Maximum value of MAINSIZE
// MINSECTORS               Minimum value of MAINSIZE
#define NVMNW_BANK2INFO0_MAINSIZE_W                                         12
#define NVMNW_BANK2INFO0_MAINSIZE_M                                 0x00000FFF
#define NVMNW_BANK2INFO0_MAINSIZE_S                                          0
#define NVMNW_BANK2INFO0_MAINSIZE_MAXSECTORS                        0x00000200
#define NVMNW_BANK2INFO0_MAINSIZE_MINSECTORS                        0x00000008

//*****************************************************************************
//
// Register: NVMNW_O_BANK2INFO1
//
//*****************************************************************************
// Field: [23:16] ENGRSIZE
//
// Engr region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of ENGRSIZE
// MINSECTORS               Minimum value of ENGRSIZE
#define NVMNW_BANK2INFO1_ENGRSIZE_W                                          8
#define NVMNW_BANK2INFO1_ENGRSIZE_M                                 0x00FF0000
#define NVMNW_BANK2INFO1_ENGRSIZE_S                                         16
#define NVMNW_BANK2INFO1_ENGRSIZE_MAXSECTORS                        0x00200000
#define NVMNW_BANK2INFO1_ENGRSIZE_MINSECTORS                        0x00000000

// Field:  [15:8] TRIMSIZE
//
// Trim region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of TRIMSIZE
// MINSECTORS               Minimum value of TRIMSIZE
#define NVMNW_BANK2INFO1_TRIMSIZE_W                                          8
#define NVMNW_BANK2INFO1_TRIMSIZE_M                                 0x0000FF00
#define NVMNW_BANK2INFO1_TRIMSIZE_S                                          8
#define NVMNW_BANK2INFO1_TRIMSIZE_MAXSECTORS                        0x00002000
#define NVMNW_BANK2INFO1_TRIMSIZE_MINSECTORS                        0x00000000

// Field:   [7:0] NONMAINSIZE
//
// Non-main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of NONMAINSIZE
// MINSECTORS               Minimum value of NONMAINSIZE
#define NVMNW_BANK2INFO1_NONMAINSIZE_W                                       8
#define NVMNW_BANK2INFO1_NONMAINSIZE_M                              0x000000FF
#define NVMNW_BANK2INFO1_NONMAINSIZE_S                                       0
#define NVMNW_BANK2INFO1_NONMAINSIZE_MAXSECTORS                     0x00000020
#define NVMNW_BANK2INFO1_NONMAINSIZE_MINSECTORS                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK3INFO0
//
//*****************************************************************************
// Field:  [11:0] MAINSIZE
//
// Main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of MAINSIZE
// MINSECTORS               Minimum value of MAINSIZE
#define NVMNW_BANK3INFO0_MAINSIZE_W                                         12
#define NVMNW_BANK3INFO0_MAINSIZE_M                                 0x00000FFF
#define NVMNW_BANK3INFO0_MAINSIZE_S                                          0
#define NVMNW_BANK3INFO0_MAINSIZE_MAXSECTORS                        0x00000200
#define NVMNW_BANK3INFO0_MAINSIZE_MINSECTORS                        0x00000008

//*****************************************************************************
//
// Register: NVMNW_O_BANK3INFO1
//
//*****************************************************************************
// Field: [23:16] ENGRSIZE
//
// Engr region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of ENGRSIZE
// MINSECTORS               Minimum value of ENGRSIZE
#define NVMNW_BANK3INFO1_ENGRSIZE_W                                          8
#define NVMNW_BANK3INFO1_ENGRSIZE_M                                 0x00FF0000
#define NVMNW_BANK3INFO1_ENGRSIZE_S                                         16
#define NVMNW_BANK3INFO1_ENGRSIZE_MAXSECTORS                        0x00200000
#define NVMNW_BANK3INFO1_ENGRSIZE_MINSECTORS                        0x00000000

// Field:  [15:8] TRIMSIZE
//
// Trim region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of TRIMSIZE
// MINSECTORS               Minimum value of TRIMSIZE
#define NVMNW_BANK3INFO1_TRIMSIZE_W                                          8
#define NVMNW_BANK3INFO1_TRIMSIZE_M                                 0x0000FF00
#define NVMNW_BANK3INFO1_TRIMSIZE_S                                          8
#define NVMNW_BANK3INFO1_TRIMSIZE_MAXSECTORS                        0x00002000
#define NVMNW_BANK3INFO1_TRIMSIZE_MINSECTORS                        0x00000000

// Field:   [7:0] NONMAINSIZE
//
// Non-main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of NONMAINSIZE
// MINSECTORS               Minimum value of NONMAINSIZE
#define NVMNW_BANK3INFO1_NONMAINSIZE_W                                       8
#define NVMNW_BANK3INFO1_NONMAINSIZE_M                              0x000000FF
#define NVMNW_BANK3INFO1_NONMAINSIZE_S                                       0
#define NVMNW_BANK3INFO1_NONMAINSIZE_MAXSECTORS                     0x00000020
#define NVMNW_BANK3INFO1_NONMAINSIZE_MINSECTORS                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK4INFO0
//
//*****************************************************************************
// Field:  [11:0] MAINSIZE
//
// Main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of MAINSIZE
// MINSECTORS               Minimum value of MAINSIZE
#define NVMNW_BANK4INFO0_MAINSIZE_W                                         12
#define NVMNW_BANK4INFO0_MAINSIZE_M                                 0x00000FFF
#define NVMNW_BANK4INFO0_MAINSIZE_S                                          0
#define NVMNW_BANK4INFO0_MAINSIZE_MAXSECTORS                        0x00000200
#define NVMNW_BANK4INFO0_MAINSIZE_MINSECTORS                        0x00000008

//*****************************************************************************
//
// Register: NVMNW_O_BANK4INFO1
//
//*****************************************************************************
// Field: [23:16] ENGRSIZE
//
// Engr region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of ENGRSIZE
// MINSECTORS               Minimum value of ENGRSIZE
#define NVMNW_BANK4INFO1_ENGRSIZE_W                                          8
#define NVMNW_BANK4INFO1_ENGRSIZE_M                                 0x00FF0000
#define NVMNW_BANK4INFO1_ENGRSIZE_S                                         16
#define NVMNW_BANK4INFO1_ENGRSIZE_MAXSECTORS                        0x00200000
#define NVMNW_BANK4INFO1_ENGRSIZE_MINSECTORS                        0x00000000

// Field:  [15:8] TRIMSIZE
//
// Trim region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of TRIMSIZE
// MINSECTORS               Minimum value of TRIMSIZE
#define NVMNW_BANK4INFO1_TRIMSIZE_W                                          8
#define NVMNW_BANK4INFO1_TRIMSIZE_M                                 0x0000FF00
#define NVMNW_BANK4INFO1_TRIMSIZE_S                                          8
#define NVMNW_BANK4INFO1_TRIMSIZE_MAXSECTORS                        0x00002000
#define NVMNW_BANK4INFO1_TRIMSIZE_MINSECTORS                        0x00000000

// Field:   [7:0] NONMAINSIZE
//
// Non-main region size in sectors
// ENUMs:
// MAXSECTORS               Maximum value of NONMAINSIZE
// MINSECTORS               Minimum value of NONMAINSIZE
#define NVMNW_BANK4INFO1_NONMAINSIZE_W                                       8
#define NVMNW_BANK4INFO1_NONMAINSIZE_M                              0x000000FF
#define NVMNW_BANK4INFO1_NONMAINSIZE_S                                       0
#define NVMNW_BANK4INFO1_NONMAINSIZE_MAXSECTORS                     0x00000020
#define NVMNW_BANK4INFO1_NONMAINSIZE_MINSECTORS                     0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTEN
//
//*****************************************************************************
// Field:     [0] ENABLE
//
// Enable Test Features
// Enumeration:
// 0x0:	Test features disabled
// 0x1: Test features enabled
// ENUMs:
// ENABLED                  Command
// DISABLED                 Command
#define NVMNW_DFTEN_ENABLE                                          0x00000001
#define NVMNW_DFTEN_ENABLE_BITN                                              0
#define NVMNW_DFTEN_ENABLE_M                                        0x00000001
#define NVMNW_DFTEN_ENABLE_S                                                 0
#define NVMNW_DFTEN_ENABLE_ENABLED                                  0x00000001
#define NVMNW_DFTEN_ENABLE_DISABLED                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTCMDCTL
//
//*****************************************************************************
// Field: [31:28] DTBMUXSEL
//
// DTB Mux Select
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_DFTCMDCTL_DTBMUXSEL_W                                          4
#define NVMNW_DFTCMDCTL_DTBMUXSEL_M                                 0xF0000000
#define NVMNW_DFTCMDCTL_DTBMUXSEL_S                                         28
#define NVMNW_DFTCMDCTL_DTBMUXSEL_MAXIMUM                           0xF0000000
#define NVMNW_DFTCMDCTL_DTBMUXSEL_MINIMUM                           0x00000000

// Field:    [20] STOPVERONFAIL
//
// Stop read verify on fail.  If this bit is set, read verify will halt when
// the first verify
// fail is detected.  If command is program or erase, another program or erase
// pulse will be executed.  If command is read verify, comand will terminate.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTCMDCTL_STOPVERONFAIL                               0x00100000
#define NVMNW_DFTCMDCTL_STOPVERONFAIL_BITN                                  20
#define NVMNW_DFTCMDCTL_STOPVERONFAIL_M                             0x00100000
#define NVMNW_DFTCMDCTL_STOPVERONFAIL_S                                     20
#define NVMNW_DFTCMDCTL_STOPVERONFAIL_ENABLE                        0x00100000
#define NVMNW_DFTCMDCTL_STOPVERONFAIL_DISABLE                       0x00000000

// Field:    [18] ODDROWINVDATA
//
// Invert data at odd row addresses for program or verify.  The LSB of the row
// address is bit [4] of the bank address.  This bit only applies when pattern
// data
// is used; i.e. the DATAPATEN bit is set.  It will have no effect if CMDDATA
// is used.
// ENUMs:
// INVERT                   Use inverted data
// TRUE                     Use true data
#define NVMNW_DFTCMDCTL_ODDROWINVDATA                               0x00040000
#define NVMNW_DFTCMDCTL_ODDROWINVDATA_BITN                                  18
#define NVMNW_DFTCMDCTL_ODDROWINVDATA_M                             0x00040000
#define NVMNW_DFTCMDCTL_ODDROWINVDATA_S                                     18
#define NVMNW_DFTCMDCTL_ODDROWINVDATA_INVERT                        0x00040000
#define NVMNW_DFTCMDCTL_ODDROWINVDATA_TRUE                          0x00000000

// Field:    [17] ODDWORDINVDATA
//
// Invert data at odd bank addresses for program or verify. This bit only
// applies
// when pattern data is used; i.e. the DATAPATEN bit is set.  It will have no
// effect
// if CMDDATA is used.
// ENUMs:
// INVERT                   Use inverted data
// TRUE                     Use true data
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA                              0x00020000
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA_BITN                                 17
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA_M                            0x00020000
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA_S                                    17
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA_INVERT                       0x00020000
#define NVMNW_DFTCMDCTL_ODDWORDINVDATA_TRUE                         0x00000000

// Field:    [16] ALWAYSINVDATA
//
// Invert data always for program or verify.  This bit only applies when
// pattern data
// is used; i.e. the DATAPATEN bit is set.  It will have no effect if CMDDATA
// is used.
// ENUMs:
// INVERT                   Use inverted data
// TRUE                     Use true data
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA                               0x00010000
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA_BITN                                  16
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA_M                             0x00010000
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA_S                                     16
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA_INVERT                        0x00010000
#define NVMNW_DFTCMDCTL_ALWAYSINVDATA_TRUE                          0x00000000

// Field: [15:13] DATAPATSEL
//
// Select data pattern.  Valid when DATAPATEN bit is set to 1.  Overrides
// CMDDATA
// registers for program or verify.
// ENUMs:
// LOGCHKBRD                Set to logical checkerboard (0x01010101...)
// ALL1                     Set to all 1
// ALL0                     Set to all 0
#define NVMNW_DFTCMDCTL_DATAPATSEL_W                                         3
#define NVMNW_DFTCMDCTL_DATAPATSEL_M                                0x0000E000
#define NVMNW_DFTCMDCTL_DATAPATSEL_S                                        13
#define NVMNW_DFTCMDCTL_DATAPATSEL_LOGCHKBRD                        0x00004000
#define NVMNW_DFTCMDCTL_DATAPATSEL_ALL1                             0x00002000
#define NVMNW_DFTCMDCTL_DATAPATSEL_ALL0                             0x00000000

// Field:    [12] DATAPATEN
//
// Enable data pattern.  Data pattern select in DATAPAT_SEL field will override
// data
// from CMDDATA registers for use as program or verify data.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTCMDCTL_DATAPATEN                                   0x00001000
#define NVMNW_DFTCMDCTL_DATAPATEN_BITN                                      12
#define NVMNW_DFTCMDCTL_DATAPATEN_M                                 0x00001000
#define NVMNW_DFTCMDCTL_DATAPATEN_S                                         12
#define NVMNW_DFTCMDCTL_DATAPATEN_ENABLE                            0x00001000
#define NVMNW_DFTCMDCTL_DATAPATEN_DISABLE                           0x00000000

// Field:     [9] PULSECNTLDDIS
//
// Override pulse counter enable.
// When set, the state machine pulse counter will not be loaded when a
// command is initiated.
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS                               0x00000200
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS_BITN                                   9
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS_M                             0x00000200
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS_S                                      9
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS_DISABLE                       0x00000200
#define NVMNW_DFTCMDCTL_PULSECNTLDDIS_ENABLE                        0x00000000

// Field:     [8] ADDRCNTLDDIS
//
// Override address counter enable.
// When set, the state machine address counter will not be loaded when a
// command is initiated.
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS                                0x00000100
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS_BITN                                    8
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS_M                              0x00000100
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS_S                                       8
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS_DISABLE                        0x00000100
#define NVMNW_DFTCMDCTL_ADDRCNTLDDIS_ENABLE                         0x00000000

// Field:     [5] REDMATCHFORCE
//
// Force redundancy match.  If set to 1, repair configuration encoded in the
// flash
// bank trim will be forced for every access.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTCMDCTL_REDMATCHFORCE                               0x00000020
#define NVMNW_DFTCMDCTL_REDMATCHFORCE_BITN                                   5
#define NVMNW_DFTCMDCTL_REDMATCHFORCE_M                             0x00000020
#define NVMNW_DFTCMDCTL_REDMATCHFORCE_S                                      5
#define NVMNW_DFTCMDCTL_REDMATCHFORCE_ENABLE                        0x00000020
#define NVMNW_DFTCMDCTL_REDMATCHFORCE_DISABLE                       0x00000000

// Field:     [4] REDMATCHDIS
//
// Disable redundancy matching.  Any repair configuration encoded into the
// bank trim bits is disabled.
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_DFTCMDCTL_REDMATCHDIS                                 0x00000010
#define NVMNW_DFTCMDCTL_REDMATCHDIS_BITN                                     4
#define NVMNW_DFTCMDCTL_REDMATCHDIS_M                               0x00000010
#define NVMNW_DFTCMDCTL_REDMATCHDIS_S                                        4
#define NVMNW_DFTCMDCTL_REDMATCHDIS_DISABLE                         0x00000010
#define NVMNW_DFTCMDCTL_REDMATCHDIS_ENABLE                          0x00000000

// Field:     [2] AMX2TDIS
//
// 2T address mux disable control. When set to 1 2T address shifting is
// disabled.
// ENUMs:
// DISABLE                  Disable
// ENABLE                   Enable
#define NVMNW_DFTCMDCTL_AMX2TDIS                                    0x00000004
#define NVMNW_DFTCMDCTL_AMX2TDIS_BITN                                        2
#define NVMNW_DFTCMDCTL_AMX2TDIS_M                                  0x00000004
#define NVMNW_DFTCMDCTL_AMX2TDIS_S                                           2
#define NVMNW_DFTCMDCTL_AMX2TDIS_DISABLE                            0x00000004
#define NVMNW_DFTCMDCTL_AMX2TDIS_ENABLE                             0x00000000

// Field:     [1] FORCE2TEN
//
// Force 2T Enable - Force 2T access to regions that are designated as 1T.
// Regions
// designated as 2T will still be accessed as 2T.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTCMDCTL_FORCE2TEN                                   0x00000002
#define NVMNW_DFTCMDCTL_FORCE2TEN_BITN                                       1
#define NVMNW_DFTCMDCTL_FORCE2TEN_M                                 0x00000002
#define NVMNW_DFTCMDCTL_FORCE2TEN_S                                          1
#define NVMNW_DFTCMDCTL_FORCE2TEN_ENABLE                            0x00000002
#define NVMNW_DFTCMDCTL_FORCE2TEN_DISABLE                           0x00000000

// Field:     [0] FORCE1TEN
//
// Force 1T Enable - Force 1T access to regions that are designated as 2T.
// Regions
// designated as 1T will still be accessed as 1T.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTCMDCTL_FORCE1TEN                                   0x00000001
#define NVMNW_DFTCMDCTL_FORCE1TEN_BITN                                       0
#define NVMNW_DFTCMDCTL_FORCE1TEN_M                                 0x00000001
#define NVMNW_DFTCMDCTL_FORCE1TEN_S                                          0
#define NVMNW_DFTCMDCTL_FORCE1TEN_ENABLE                            0x00000001
#define NVMNW_DFTCMDCTL_FORCE1TEN_DISABLE                           0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTTIMERCTL
//
//*****************************************************************************
// Field: [30:28] TIMERCLOCKOVR
//
// Override Timer clock frequency using an ICG-based clock divide mechanism.
// To divide the timer clock, pulses can be skipped based on settings in this
// field.
// Enumeration:
// 0: No division on timer clock
// 1-7: Divide timer clock by 2-8
// ENUMs:
// DIVIDEBY8                Divide timer clock by 8
// DIVIDEBY7                Divide timer clock by 7
// DIVIDEBY6                Divide timer clock by 6
// DIVIDEBY5                Divide timer clock by 5
// DIVIDEBY4                Divide timer clock by 4
// DIVIDEBY3                Divide timer clock by 3
// DIVIDEBY2                Divide timer clock by 2
// NODIVIDE                 No divide on timer clock.
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_W                                    3
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_M                           0x70000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_S                                   28
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY8                   0x70000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY7                   0x60000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY6                   0x50000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY5                   0x40000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY4                   0x30000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY3                   0x20000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_DIVIDEBY2                   0x10000000
#define NVMNW_DFTTIMERCTL_TIMERCLOCKOVR_NODIVIDE                    0x00000000

// Field: [27:12] PEPULSETIMEVAL
//
// Program/Erase Pulse Time Value
// If operation is a program, this value gets loaded into bits [15:0] of the
// timer
// when the PEPULSETIMEVAL_OVR field is set to 1.
// If operation is an erase, this value gets loaded into bits [19:4] of the
// timer
// when the PEPULSETIMEVAL_OVR field is set to 1.
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_DFTTIMERCTL_PEPULSETIMEVAL_W                                  16
#define NVMNW_DFTTIMERCTL_PEPULSETIMEVAL_M                          0x0FFFF000
#define NVMNW_DFTTIMERCTL_PEPULSETIMEVAL_S                                  12
#define NVMNW_DFTTIMERCTL_PEPULSETIMEVAL_MAXIMUM                    0x0FFFF000
#define NVMNW_DFTTIMERCTL_PEPULSETIMEVAL_MINIMUM                    0x00001000

// Field:     [8] PEPULSETIMEOVR
//
// Override Program/Erase Pulse Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use bits [31:16] of this register as time value
// ENUMs:
// OVERRIDE                 Use value from the PE_PULSE_TIME field for time
//                          value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR                            0x00000100
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR_BITN                                8
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR_M                          0x00000100
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR_S                                   8
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR_OVERRIDE                   0x00000100
#define NVMNW_DFTTIMERCTL_PEPULSETIMEOVR_FUNCTIONAL                 0x00000000

// Field:     [7] READMODETIME
//
// Read Mode Change Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_READMODETIME                              0x00000080
#define NVMNW_DFTTIMERCTL_READMODETIME_BITN                                  7
#define NVMNW_DFTTIMERCTL_READMODETIME_M                            0x00000080
#define NVMNW_DFTTIMERCTL_READMODETIME_S                                     7
#define NVMNW_DFTTIMERCTL_READMODETIME_TWOXFUNCTIONAL               0x00000080
#define NVMNW_DFTTIMERCTL_READMODETIME_FUNCTIONAL                   0x00000000

// Field:     [6] PEVHOLDTIME
//
// Program/Erase Verify Hold Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME                               0x00000040
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME_BITN                                   6
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME_M                             0x00000040
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME_S                                      6
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME_TWOXFUNCTIONAL                0x00000040
#define NVMNW_DFTTIMERCTL_PEVHOLDTIME_FUNCTIONAL                    0x00000000

// Field:     [5] PEVSETUPTIME
//
// Program/Erase Verify Setup Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME                              0x00000020
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME_BITN                                  5
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME_M                            0x00000020
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME_S                                     5
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME_TWOXFUNCTIONAL               0x00000020
#define NVMNW_DFTTIMERCTL_PEVSETUPTIME_FUNCTIONAL                   0x00000000

// Field:     [4] PEVMODETIME
//
// Program/Erase Verify Mode Change Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PEVMODETIME                               0x00000010
#define NVMNW_DFTTIMERCTL_PEVMODETIME_BITN                                   4
#define NVMNW_DFTTIMERCTL_PEVMODETIME_M                             0x00000010
#define NVMNW_DFTTIMERCTL_PEVMODETIME_S                                      4
#define NVMNW_DFTTIMERCTL_PEVMODETIME_TWOXFUNCTIONAL                0x00000010
#define NVMNW_DFTTIMERCTL_PEVMODETIME_FUNCTIONAL                    0x00000000

// Field:     [3] PEHOLDTIME
//
// Program/Erase Hold Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PEHOLDTIME                                0x00000008
#define NVMNW_DFTTIMERCTL_PEHOLDTIME_BITN                                    3
#define NVMNW_DFTTIMERCTL_PEHOLDTIME_M                              0x00000008
#define NVMNW_DFTTIMERCTL_PEHOLDTIME_S                                       3
#define NVMNW_DFTTIMERCTL_PEHOLDTIME_TWOXFUNCTIONAL                 0x00000008
#define NVMNW_DFTTIMERCTL_PEHOLDTIME_FUNCTIONAL                     0x00000000

// Field:     [2] PPVWORDLINETIME
//
// Program and Program Verify Wordline Switching Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME                           0x00000004
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME_BITN                               2
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME_M                         0x00000004
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME_S                                  2
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME_TWOXFUNCTIONAL            0x00000004
#define NVMNW_DFTTIMERCTL_PPVWORDLINETIME_FUNCTIONAL                0x00000000

// Field:     [1] PVHVSETUPTIME
//
// Program VHV Setup Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME                             0x00000002
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME_BITN                                 1
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME_M                           0x00000002
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME_S                                    1
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME_TWOXFUNCTIONAL              0x00000002
#define NVMNW_DFTTIMERCTL_PVHVSETUPTIME_FUNCTIONAL                  0x00000000

// Field:     [0] PESETUPTIME
//
// Program/Erase Setup Time
// Enumeration:
// 0: Use hard-wired time value
// 1: Use 2x the hard-wired time value
// ENUMs:
// TWOXFUNCTIONAL           Use 2x the hard-wired (functional) time value
// FUNCTIONAL               Use hard-wired (Functional) timer value
#define NVMNW_DFTTIMERCTL_PESETUPTIME                               0x00000001
#define NVMNW_DFTTIMERCTL_PESETUPTIME_BITN                                   0
#define NVMNW_DFTTIMERCTL_PESETUPTIME_M                             0x00000001
#define NVMNW_DFTTIMERCTL_PESETUPTIME_S                                      0
#define NVMNW_DFTTIMERCTL_PESETUPTIME_TWOXFUNCTIONAL                0x00000001
#define NVMNW_DFTTIMERCTL_PESETUPTIME_FUNCTIONAL                    0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTEXECZCTL
//
//*****************************************************************************
// Field:     [1] EXEZ_OVR
//
// Override value to be applied to EXECUTEZ
// ENUMs:
// ONE                      Set EXECUTEZ to 1
// ZERO                     Set EXECUTEZ to 0
#define NVMNW_DFTEXECZCTL_EXEZ_OVR                                  0x00000002
#define NVMNW_DFTEXECZCTL_EXEZ_OVR_BITN                                      1
#define NVMNW_DFTEXECZCTL_EXEZ_OVR_M                                0x00000002
#define NVMNW_DFTEXECZCTL_EXEZ_OVR_S                                         1
#define NVMNW_DFTEXECZCTL_EXEZ_OVR_ONE                              0x00000002
#define NVMNW_DFTEXECZCTL_EXEZ_OVR_ZERO                             0x00000000

// Field:     [0] EXEZOVREN
//
// Enable override of EXECUTEZ
// Enumeration:
// 0: Override of EXECUTEZ disabled
// 1: Override of EXECUTEZ enabled
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTEXECZCTL_EXEZOVREN                                 0x00000001
#define NVMNW_DFTEXECZCTL_EXEZOVREN_BITN                                     0
#define NVMNW_DFTEXECZCTL_EXEZOVREN_M                               0x00000001
#define NVMNW_DFTEXECZCTL_EXEZOVREN_S                                        0
#define NVMNW_DFTEXECZCTL_EXEZOVREN_ENABLE                          0x00000001
#define NVMNW_DFTEXECZCTL_EXEZOVREN_DISABLE                         0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTPCLKTESTCTL
//
//*****************************************************************************
// Field:     [0] ENABLE
//
// Enable the state machine which sequences measurement of pump clock
// frequency.
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTPCLKTESTCTL_ENABLE                                 0x00000001
#define NVMNW_DFTPCLKTESTCTL_ENABLE_BITN                                     0
#define NVMNW_DFTPCLKTESTCTL_ENABLE_M                               0x00000001
#define NVMNW_DFTPCLKTESTCTL_ENABLE_S                                        0
#define NVMNW_DFTPCLKTESTCTL_ENABLE_ENABLE                          0x00000001
#define NVMNW_DFTPCLKTESTCTL_ENABLE_DISABLE                         0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTPCLKTESTSTAT
//
//*****************************************************************************
// Field:  [15:4] CLOCKCNT
//
// Indicates the core clock count captured during the pump clock measurement.
// ENUMs:
// MAXIMUM                  Maximum count value
// MINIMUM                  Minimum count value
#define NVMNW_DFTPCLKTESTSTAT_CLOCKCNT_W                                    12
#define NVMNW_DFTPCLKTESTSTAT_CLOCKCNT_M                            0x0000FFF0
#define NVMNW_DFTPCLKTESTSTAT_CLOCKCNT_S                                     4
#define NVMNW_DFTPCLKTESTSTAT_CLOCKCNT_MAXIMUM                      0x0000FFF0
#define NVMNW_DFTPCLKTESTSTAT_CLOCKCNT_MINIMUM                      0x00000000

// Field:     [0] BUSY
//
// Indicates that a pump clock measurement is in progress.
// ENUMs:
// INPROGRESS               Indicates test in progress
// COMPLETE                 Indicates test complete
#define NVMNW_DFTPCLKTESTSTAT_BUSY                                  0x00000001
#define NVMNW_DFTPCLKTESTSTAT_BUSY_BITN                                      0
#define NVMNW_DFTPCLKTESTSTAT_BUSY_M                                0x00000001
#define NVMNW_DFTPCLKTESTSTAT_BUSY_S                                         0
#define NVMNW_DFTPCLKTESTSTAT_BUSY_INPROGRESS                       0x00000001
#define NVMNW_DFTPCLKTESTSTAT_BUSY_COMPLETE                         0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED0
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED0_VAL_W                                              4
#define NVMNW_DFTDATARED0_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED0_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED1
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED1_VAL_W                                              4
#define NVMNW_DFTDATARED1_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED1_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED2
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED2_VAL_W                                              4
#define NVMNW_DFTDATARED2_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED2_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED3
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED3_VAL_W                                              4
#define NVMNW_DFTDATARED3_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED3_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED4
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED4_VAL_W                                              4
#define NVMNW_DFTDATARED4_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED4_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED5
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED5_VAL_W                                              4
#define NVMNW_DFTDATARED5_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED5_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED6
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED6_VAL_W                                              4
#define NVMNW_DFTDATARED6_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED6_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTDATARED7
//
//*****************************************************************************
// Field:   [3:0] VAL
//
// Data for redundant bits
#define NVMNW_DFTDATARED7_VAL_W                                              4
#define NVMNW_DFTDATARED7_VAL_M                                     0x0000000F
#define NVMNW_DFTDATARED7_VAL_S                                              0

//*****************************************************************************
//
// Register: NVMNW_O_DFTPUMPCTL
//
//*****************************************************************************
// Field: [18:16] IREFEVCTL
//
// IREFEV control IREFVRD, REFTC, IREFCONST, IREFCCOR blocks in IREFEV
#define NVMNW_DFTPUMPCTL_IREFEVCTL_W                                         3
#define NVMNW_DFTPUMPCTL_IREFEVCTL_M                                0x00070000
#define NVMNW_DFTPUMPCTL_IREFEVCTL_S                                        16

// Field: [15:12] CONFIGPMP
//
// Pump configuration control. LP, HP operation
#define NVMNW_DFTPUMPCTL_CONFIGPMP_W                                         4
#define NVMNW_DFTPUMPCTL_CONFIGPMP_M                                0x0000F000
#define NVMNW_DFTPUMPCTL_CONFIGPMP_S                                        12

// Field:     [9] SSEN
//
// Dither control for oscillator
// Enumeration:
// 0: Disable Dither
// 1: Enable Dither
// ENUMs:
// ENABLE                   Enable
// DISABLE                  Disable
#define NVMNW_DFTPUMPCTL_SSEN                                       0x00000200
#define NVMNW_DFTPUMPCTL_SSEN_BITN                                           9
#define NVMNW_DFTPUMPCTL_SSEN_M                                     0x00000200
#define NVMNW_DFTPUMPCTL_SSEN_S                                              9
#define NVMNW_DFTPUMPCTL_SSEN_ENABLE                                0x00000200
#define NVMNW_DFTPUMPCTL_SSEN_DISABLE                               0x00000000

// Field:     [8] PUMPCLKEN
//
// Allows direct control of the pump oscillator which is used to generate
// pumpclk.
// Normally, enable/disable of pumpclk is under NoWrapper state machine
// control.  This bit allows system to enable the clock independently.
// ENUMs:
// ENABLE                   Force pump clock oscillator to be enabled.
// HWCTL                    Allow pump clock oscillator to be controlled by
//                          hardware.
#define NVMNW_DFTPUMPCTL_PUMPCLKEN                                  0x00000100
#define NVMNW_DFTPUMPCTL_PUMPCLKEN_BITN                                      8
#define NVMNW_DFTPUMPCTL_PUMPCLKEN_M                                0x00000100
#define NVMNW_DFTPUMPCTL_PUMPCLKEN_S                                         8
#define NVMNW_DFTPUMPCTL_PUMPCLKEN_ENABLE                           0x00000100
#define NVMNW_DFTPUMPCTL_PUMPCLKEN_HWCTL                            0x00000000

// Field:   [6:0] TCR
//
// TCR test mode to be applied to the pump
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_DFTPUMPCTL_TCR_W                                               7
#define NVMNW_DFTPUMPCTL_TCR_M                                      0x0000007F
#define NVMNW_DFTPUMPCTL_TCR_S                                               0
#define NVMNW_DFTPUMPCTL_TCR_MAXIMUM                                0x0000007F
#define NVMNW_DFTPUMPCTL_TCR_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_DFTBANKCTL
//
//*****************************************************************************
// Field:     [8] TEZ
//
// When set, TEZ is asserted to the flash banks.  Which banks get the asserted
//
// signal is determined by the BANKSELECT field in CMDCTL.
// 0x0 Do no assert TEZ
// 0x1 Assert TEZ
// ENUMs:
// NEGATE                   Do not assert TEZ
// ASSERT                   Assert TEZ
#define NVMNW_DFTBANKCTL_TEZ                                        0x00000100
#define NVMNW_DFTBANKCTL_TEZ_BITN                                            8
#define NVMNW_DFTBANKCTL_TEZ_M                                      0x00000100
#define NVMNW_DFTBANKCTL_TEZ_S                                               8
#define NVMNW_DFTBANKCTL_TEZ_NEGATE                                 0x00000100
#define NVMNW_DFTBANKCTL_TEZ_ASSERT                                 0x00000000

// Field:   [6:0] TCR
//
// TCR test mode to be applied to the bank
// ENUMs:
// MAXIMUM                  Maximum value
// MINIMUM                  Minimum value
#define NVMNW_DFTBANKCTL_TCR_W                                               7
#define NVMNW_DFTBANKCTL_TCR_M                                      0x0000007F
#define NVMNW_DFTBANKCTL_TCR_S                                               0
#define NVMNW_DFTBANKCTL_TCR_MAXIMUM                                0x0000007F
#define NVMNW_DFTBANKCTL_TCR_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_TRIMCTL
//
//*****************************************************************************
// Field:     [0] ENABLE
//
// Indicate that Bank and Pump trim values are valid.
// ENUMs:
// VALID                    Trim data is valid
// INVALID                  Trim data is not valid
#define NVMNW_TRIMCTL_ENABLE                                        0x00000001
#define NVMNW_TRIMCTL_ENABLE_BITN                                            0
#define NVMNW_TRIMCTL_ENABLE_M                                      0x00000001
#define NVMNW_TRIMCTL_ENABLE_S                                               0
#define NVMNW_TRIMCTL_ENABLE_VALID                                  0x00000001
#define NVMNW_TRIMCTL_ENABLE_INVALID                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_PUMPTRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_PUMPTRIM0_VAL_W                                               32
#define NVMNW_PUMPTRIM0_VAL_M                                       0xFFFFFFFF
#define NVMNW_PUMPTRIM0_VAL_S                                                0
#define NVMNW_PUMPTRIM0_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_PUMPTRIM0_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_PUMPTRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_PUMPTRIM1_VAL_W                                               32
#define NVMNW_PUMPTRIM1_VAL_M                                       0xFFFFFFFF
#define NVMNW_PUMPTRIM1_VAL_S                                                0
#define NVMNW_PUMPTRIM1_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_PUMPTRIM1_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_PUMPTRIM2
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_PUMPTRIM2_VAL_W                                               32
#define NVMNW_PUMPTRIM2_VAL_M                                       0xFFFFFFFF
#define NVMNW_PUMPTRIM2_VAL_S                                                0
#define NVMNW_PUMPTRIM2_VAL_MAXIMUM                                 0xFFFFFFFF
#define NVMNW_PUMPTRIM2_VAL_MINIMUM                                 0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK0TRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK0TRIM0_VAL_W                                              32
#define NVMNW_BANK0TRIM0_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK0TRIM0_VAL_S                                               0
#define NVMNW_BANK0TRIM0_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_BANK0TRIM0_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK0TRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK0TRIM1_VAL_W                                              32
#define NVMNW_BANK0TRIM1_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK0TRIM1_VAL_S                                               0
#define NVMNW_BANK0TRIM1_VAL_MAXIMUM                                0x0FFFFFFF
#define NVMNW_BANK0TRIM1_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK1TRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK1TRIM0_VAL_W                                              32
#define NVMNW_BANK1TRIM0_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK1TRIM0_VAL_S                                               0
#define NVMNW_BANK1TRIM0_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_BANK1TRIM0_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK1TRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK1TRIM1_VAL_W                                              32
#define NVMNW_BANK1TRIM1_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK1TRIM1_VAL_S                                               0
#define NVMNW_BANK1TRIM1_VAL_MAXIMUM                                0x0FFFFFFF
#define NVMNW_BANK1TRIM1_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK2TRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK2TRIM0_VAL_W                                              32
#define NVMNW_BANK2TRIM0_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK2TRIM0_VAL_S                                               0
#define NVMNW_BANK2TRIM0_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_BANK2TRIM0_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK2TRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK2TRIM1_VAL_W                                              32
#define NVMNW_BANK2TRIM1_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK2TRIM1_VAL_S                                               0
#define NVMNW_BANK2TRIM1_VAL_MAXIMUM                                0x0FFFFFFF
#define NVMNW_BANK2TRIM1_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK3TRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK3TRIM0_VAL_W                                              32
#define NVMNW_BANK3TRIM0_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK3TRIM0_VAL_S                                               0
#define NVMNW_BANK3TRIM0_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_BANK3TRIM0_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK3TRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK3TRIM1_VAL_W                                              32
#define NVMNW_BANK3TRIM1_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK3TRIM1_VAL_S                                               0
#define NVMNW_BANK3TRIM1_VAL_MAXIMUM                                0x0FFFFFFF
#define NVMNW_BANK3TRIM1_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK4TRIM0
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK4TRIM0_VAL_W                                              32
#define NVMNW_BANK4TRIM0_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK4TRIM0_VAL_S                                               0
#define NVMNW_BANK4TRIM0_VAL_MAXIMUM                                0xFFFFFFFF
#define NVMNW_BANK4TRIM0_VAL_MINIMUM                                0x00000000

//*****************************************************************************
//
// Register: NVMNW_O_BANK4TRIM1
//
//*****************************************************************************
// Field:  [31:0] VAL
//
// Trim Data Value
// ENUMs:
// MAXIMUM                  Maximum value of VAL
// MINIMUM                  Minimum value of VAL
#define NVMNW_BANK4TRIM1_VAL_W                                              32
#define NVMNW_BANK4TRIM1_VAL_M                                      0xFFFFFFFF
#define NVMNW_BANK4TRIM1_VAL_S                                               0
#define NVMNW_BANK4TRIM1_VAL_MAXIMUM                                0x0FFFFFFF
#define NVMNW_BANK4TRIM1_VAL_MINIMUM                                0x00000000


#endif // __NVMNW__
