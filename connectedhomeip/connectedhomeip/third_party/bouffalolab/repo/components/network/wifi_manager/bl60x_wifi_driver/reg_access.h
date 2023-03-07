
/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef REG_ACCESS_H_
#define REG_ACCESS_H_

/*****************************************************************************
 * Addresses within RWNX_ADDR_CPU
 *****************************************************************************/
#define RAM_LMAC_FW_ADDR     0x00000000

/*****************************************************************************
 * Addresses within RWNX_ADDR_SYSTEM
 *****************************************************************************/
/* Shard RAM */
#define SHARED_RAM_START_ADDR         0x00000000

/* IPC registers */
#define IPC_REG_BASE_ADDR             0x00800000

/* System Controller Registers */
#define SYSCTRL_SIGNATURE_ADDR        0x00900000
#define SYSCTRL_MISC_CNTL_ADDR        0x009000E0
#define  BOOTROM_ENABLE               BIT(4)
#define  FPGA_B_RESET                 BIT(1)
#define  SOFT_RESET                   BIT(0)

/* MAC platform */
#define NXMAC_SW_SET_PROFILING_ADDR   0x00B08564
#define NXMAC_SW_CLEAR_PROFILING_ADDR 0x00B08568

/* Modem Status */
#define MDM_HDMCONFIG_ADDR            0x00C00000

/* Modem Config */
#define MDM_MEMCLKCTRL0_ADDR          0x00C00848
#define MDM_CLKGATEFCTRL0_ADDR        0x00C00874

/* AGC (trident) */
#define AGC_RWNXAGCCNTL_ADDR          0x00C02060

/* LDPC RAM*/
#define PHY_LDPC_RAM_ADDR             0x00C09000

/* FCU (elma )*/
#define FCU_RWNXFCAGCCNTL_ADDR        0x00C09034

/* AGC RAM */
#define PHY_AGC_UCODE_ADDR            0x00C0A000

/* RIU */
#define RIU_RWNXVERSION_ADDR          0x00C0B000
#define RIU_RWNXDYNAMICCONFIG_ADDR    0x00C0B008
#define RIU_AGCMEMBISTSTAT_ADDR       0x00C0B238
#define RIU_AGCMEMSIGNATURESTAT_ADDR  0x00C0B23C
#define RIU_RWNXAGCCNTL_ADDR          0x00C0B390

/* FCU RAM */
#define PHY_FCU_UCODE_ADDR            0x00C0E000

/* */
#define FPGAB_MPIF_SEL_ADDR           0x00C10030

/*****************************************************************************
 * Macros for generated register files
 *****************************************************************************/
/* Macros for IPC registers access (used in reg_ipc_app.h) */
#define REG_IPC_APP_RD(env, INDEX)                                      \
    (*(volatile u32*)((u8*)env + IPC_REG_BASE_ADDR + 4*(INDEX)))

#define REG_IPC_APP_WR(env, INDEX, value)                               \
    (*(volatile u32*)((u8*)env + IPC_REG_BASE_ADDR + 4*(INDEX)) = value)

/* Macro used in reg_mac_core.h */
#define REG_PL_RD(addr)         0
#define REG_PL_WR(addr, value)

#endif /* REG_ACCESS_H_ */
