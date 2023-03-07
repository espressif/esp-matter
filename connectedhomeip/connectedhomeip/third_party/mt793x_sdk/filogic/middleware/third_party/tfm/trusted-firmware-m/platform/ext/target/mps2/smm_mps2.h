/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SMM_MPS2_H__
#define __SMM_MPS2_H__

#include "cmsis.h"   /* device specific header file */

/* FPGAIO register map structure */
struct arm_mps2_fpgaio_t {
    volatile uint32_t LED;           /* Offset: 0x000 (R/W) LED connections
                                      *         [31:2] : Reserved
                                      *         [1:0]  : LEDs */
    volatile uint32_t RESERVED1[1];
    volatile uint32_t BUTTON;        /* Offset: 0x008 (R/W) Buttons
                                      *         [31:2] : Reserved
                                      *         [1:0]  : Buttons */
    volatile uint32_t RESERVED2[1];
    volatile uint32_t CLK1HZ;        /* Offset: 0x010 (R/W) 1Hz up counter */
    volatile uint32_t CLK100HZ;      /* Offset: 0x014 (R/W) 100Hz up counter */
    volatile uint32_t COUNTER;       /* Offset: 0x018 (R/W) Cycle Up Counter
                                      *                     Increments when
                                      *                     32bit prescale
                                      *                     counter reach
                                      *                     zero */
    volatile uint32_t RESERVED3[1];
    volatile uint32_t PRESCALE;      /* Offset: 0x020 (R/W) Prescaler
                                      *                     Bit[31:0] : reload
                                      *                     value for prescale
                                      *                     counter */
    volatile uint32_t PSCNTR;        /* Offset: 0x024 (R/W) 32bit Prescale
                                      *                     counter. Current
                                      *                     value of the
                                      *                     prescaler counter.
                                      *
                                      * The Cycle Up Counter increment when the
                                      * prescale down counter reach 0.
                                      * The prescaler counter is reloaded with
                                      * PRESCALE after reaching 0. */
    volatile uint32_t RESERVED4[9];
    volatile uint32_t MISC;          /* Offset: 0x04C (R/W) Misc control
                                      *         [31:10] : Reserved
                                      *         [9] : SHIELD_1_SPI_nCS
                                      *         [8] : SHIELD_0_SPI_nCS
                                      *         [7] : ADC_SPI_nCS
                                      *         [6] : CLCD_BL_CTRL
                                      *         [5] : CLCD_RD
                                      *         [4] : CLCD_RS
                                      *         [3] : CLCD_RESET
                                      *         [2] : RESERVED
                                      *         [1] : SPI_nSS
                                      *         [0] : CLCD_CS */
};

/* SCC register map structure */
struct arm_mps2_scc_t {
    volatile uint32_t CFG_REG0;    /* Offset: 0x000 (R/W) Remaps block RAM to
                                    *                     ZBT
                                    *         [31:1] : Reserved
                                    *         [0] 1  : REMAP BlockRam to ZBT */
    volatile uint32_t LEDS;        /* Offset: 0x004 (R/W) Controls the MCC user
                                    *                      LEDs
                                    *         [31:8] : Reserved
                                    *         [7:0]  : MCC LEDs */
    volatile uint32_t RESERVED0[1];
    volatile uint32_t SWITCHES;    /* Offset: 0x00C (R/ ) Denotes the state
                                    *                     of the MCC user
                                    *                     switches
                                    *         [31:8] : Reserved
                                    *         [7:0]  : These bits indicate state
                                    *                  of the MCC switches */
    volatile uint32_t CFG_REG4;    /* Offset: 0x010 (R/ ) Denotes the board
                                    *                     revision
                                    *         [31:4] : Reserved
                                    *         [3:0]  : Used by the MCC to pass
                                    *                  PCB revision.
                                    *                  0 = A 1 = B */
    volatile uint32_t RESERVED1[35];
    volatile uint32_t SYS_CFGDATA_RTN; /* Offset: 0x0A0 (R/W) User data register
                                        *         [31:0] : Data */
    volatile uint32_t SYS_CFGDATA_OUT; /* Offset: 0x0A4 (R/W)  User data
                                        *                      register
                                        *         [31:0] : Data */
    volatile uint32_t SYS_CFGCTRL;     /* Offset: 0x0A8 (R/W) Control register
                                        *         [31]    : Start (generates
                                        *                   interrupt on write
                                        *                   to this bit)
                                        *         [30]    : R/W access
                                        *         [29:26] : Reserved
                                        *         [25:20] : Function value
                                        *         [19:12] : Reserved
                                        *         [11:0]  : Device (value of
                                        *                   0/1/2 for supported
                                        *                   clocks) */
    volatile uint32_t SYS_CFGSTAT;     /* Offset: 0x0AC (R/W) Contains status
                                        *                     information
                                        *         [31:2] : Reserved
                                        *         [1]    : Error
                                        *         [0]    : Complete */
    volatile uint32_t RESERVED2[20];
    volatile uint32_t SCC_DLL;         /* Offset: 0x100 (R/W) DLL Lock Register
                                        *         [31:24] : DLL LOCK MASK[7:0]
                                        *                   Indicate if the DLL
                                        *                   locked is masked
                                        *         [23:16] : DLL LOCK MASK[7:0]
                                        *                   Indicate if the DLLs
                                        *                   are locked or
                                        *                   unlocked
                                        *         [15:1]  : Reserved
                                        *         [0]     : This bit indicates
                                        *                   if all enabled DLLs
                                        *                   are locked */
    volatile uint32_t RESERVED3[957];
    volatile uint32_t SCC_AID;         /* Offset: 0xFF8 (R/ ) SCC AID Register
                                        *         [31:24] : FPGA build number
                                        *         [23:20] : V2MMPS2 target
                                        *                   board revision
                                        *                   (A = 0, B = 1)
                                        *         [19:11] : Reserved
                                        *         [10]    : if “1” SCC_SW
                                        *                   register has been
                                        *                   implemented
                                        *         [9]     : if “1” SCC_LED
                                        *                   register has been
                                        *                   implemented
                                        *         [8]     : if “1” DLL lock
                                        *                   register has been
                                        *                   implemented
                                        *         [7:0]   : number of SCC
                                        *                   configuration
                                        *                   register */
    volatile uint32_t SCC_ID;          /* Offset: 0xFFC (R/ ) Contains
                                        *                     information about
                                        *                     the FPGA image
                                        *         [31:24] : Implementer ID:
                                        *                   0x41 = ARM
                                        *         [23:20] : Application note
                                        *                   IP variant number
                                        *         [19:16] : IP Architecture:
                                        *                   0x4 =AHB
                                        *         [15:4]  : Primary part number:
                                        *                   386 = AN386
                                        *         [3:0]   : Application note IP
                                        *                   revision number */
};

/* Peripheral declaration */
#define MPS2_FPGAIO      ((struct arm_mps2_fpgaio_t*) MPS2_IO_FPGAIO_BASE_NS)
#define MPS2_SCC         ((struct arm_mps2_scc_t*) MPS2_IO_SCC_BASE_NS)

/* Secure Peripheral declaration */
#define SEC_MPS2_FPGAIO  ((struct arm_mps2_fpgaio_t*) MPS2_IO_FPGAIO_BASE_S)
#define SEC_MPS2_SCC     ((struct arm_mps2_scc_t*) MPS2_IO_SCC_BASE_S)

#endif /* __SMM_MPS2_H__ */
