/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

/*! \file  soc3_0.h
*    \brief This file contains the info of soc3_0
*/

#ifdef SOC3_0

#ifndef _SOC3_0_H
#define _SOC3_0_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define CONNAC2X_TOP_HCR 0x88000000  /*no use, set HCR = HVR*/
#define CONNAC2X_TOP_HVR 0x88000000
#define CONNAC2X_TOP_FVR 0x88000004
#define CONNAC2x_CONN_CFG_ON_BASE	0x7C060000
#define CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_ADDR \
	(CONNAC2x_CONN_CFG_ON_BASE + 0xF0)
#define CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_DRV_FM_STAT_SYNC_SHFT         0

#define SOC3_0_CHIP_ID                 (0x7915)
#define SOC3_0_SW_SYNC0                CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_ADDR
#define SOC3_0_SW_SYNC0_RDY_OFFSET \
	CONNAC2x_CONN_CFG_ON_CONN_ON_MISC_DRV_FM_STAT_SYNC_SHFT
#define SOC3_0_PATCH_START_ADDR        (0x00100000)
#define SOC3_0_TOP_CFG_BASE			CONN_CFG_BASE
#define SOC3_0_TX_DESC_APPEND_LENGTH   32
#define SOC3_0_RX_DESC_LENGTH   24
#define SOC3_0_ARB_AC_MODE_ADDR (0x820e3020)

#define CONN_HOST_CSR_TOP_BASE_ADDR 0x18060000
#define CONN_INFRA_CFG_BASE_ADDR 0x18001000
#define CONN_INFRA_RGU_BASE_ADDR 0x18000000
#define CONN_INFRA_BRCM_BASE_ADDR 0x1800E000

#define WF_TOP_MISC_OFF_BASE_ADDR 0x184B0000

#define CONN_INFRA_WAKEUP_WF_ADDR (CONN_HOST_CSR_TOP_BASE_ADDR + 0x01A4)
#define CONN_INFRA_ON2OFF_SLP_PROT_ACK_ADDR \
	(CONN_HOST_CSR_TOP_BASE_ADDR + 0x0184)
#define CONN_HW_VER_ADDR (CONN_INFRA_CFG_BASE_ADDR + 0x0000)
#define WFSYS_CPU_SW_RST_B_ADDR (CONN_INFRA_RGU_BASE_ADDR + 0x0010)
#define WFSYS_ON_TOP_PWR_CTL_ADDR (CONN_INFRA_RGU_BASE_ADDR + 0x0000)
#define TOP_DBG_DUMMY_3_CONNSYS_PWR_STATUS_ADDR \
	(CONN_HOST_CSR_TOP_BASE_ADDR + 0x02CC)
#define CONN_INFRA_WF_SLP_CTRL_R_ADDR (CONN_INFRA_CFG_BASE_ADDR + 0x0620)
#define CONN_INFRA_WFDMA_SLP_CTRL_R_ADDR (CONN_INFRA_CFG_BASE_ADDR + 0x0624)
#define WF_VDNR_EN_ADDR (CONN_INFRA_BRCM_BASE_ADDR + 0x6C)
#define WFSYS_VERSION_ID_ADDR (WF_TOP_MISC_OFF_BASE_ADDR + 0x10)
#define CONN_CFG_AP2WF_REMAP_1_ADDR (CONN_INFRA_CFG_BASE_ADDR + 0x0120)
#define CONN_MCU_CONFG_HS_BASE 0x89040000
#define CONNSYS_VERSION_ID  0x20010000
#define WF_DYNAMIC_BASE 0x18500000
#define MCU_EMI_ENTRY_OFFSET 0x01DC
#define WF_EMI_ENTRY_OFFSET 0x01E0

#define CONNSYS_ROM_DONE_CHECK  0x00001D1E

#define WF_ROM_CODE_INDEX_ADDR 0x184C1604

#define WF_TRIGGER_AP2CONN_EINT 0x10001F00
#define WF_CONN_INFA_BUS_CLOCK_RATE 0x1000123C


/*******************************************************************************
*                         D A T A   T Y P E S
********************************************************************************
*/
enum consys_drv_type {
	CONNDRV_TYPE_BT = 0,
	CONNDRV_TYPE_FM = 1,
	CONNDRV_TYPE_GPS = 2,
	CONNDRV_TYPE_WIFI = 3,
	CONNDRV_TYPE_MAX
};

struct whole_chip_rst_cb {
	int (*pre_whole_chip_rst)(void);
	int (*post_whole_chip_rst)(void);
};

struct pre_calibration_cb {
	int (*pwr_on_cb)(void);
	int (*do_cal_cb)(void);
};

struct sub_drv_ops_cb {
	/* chip reset */
	struct whole_chip_rst_cb rst_cb;

	/* calibration */
	struct pre_calibration_cb pre_cal_cb;

	/* thermal query */
	int (*thermal_qry)(void);

};
/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
#if (CFG_SUPPORT_CONNINFRA == 1)
extern int conninfra_pwr_on(enum consys_drv_type drv_type);
extern int conninfra_pwr_off(enum consys_drv_type drv_type);
extern int conninfra_sub_drv_ops_register(enum consys_drv_type drv_type,
				struct sub_drv_ops_cb *cb);
extern int conninfra_trigger_whole_chip_rst(enum consys_drv_type who,
				char *reason);
#endif
void soc3_0_show_ple_info(
	struct ADAPTER *prAdapter,
	u_int8_t fgDumpTxd);
void soc3_0_show_pse_info(
	struct ADAPTER *prAdapter);

void soc3_0_show_wfdma_info(
	IN struct ADAPTER *prAdapter);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#if (CFG_DOWNLOAD_DYN_MEMORY_MAP == 1)
uint32_t soc3_0_DownloadByDynMemMap(IN struct ADAPTER *prAdapter,
	IN uint32_t u4Addr, IN uint32_t u4Len,
	IN uint8_t *pucStartPtr, IN enum ENUM_IMG_DL_IDX_T eDlIdx);
#endif
int hifWmmcuPwrOn(void);
int hifWmmcuPwrOff(void);
int wf_ioremap_read(size_t addr, unsigned int *val);

int wf_ioremap_write(phys_addr_t addr, unsigned int val);
int soc3_0_Trigger_fw_assert(void);
#if (CFG_SUPPORT_CONNINFRA == 1)
int soc3_0_Trigger_whole_chip_rst(char *reason);
void soc3_0_Sw_interrupt_handler(struct ADAPTER *prAdapter);
#endif /*#if (CFG_SUPPORT_CONNINFRA == 1)*/
void soc3_0_icapRiseVcoreClockRate(void);
void soc3_0_icapDownVcoreClockRate(void);

#endif /* _SOC3_0_H */

#endif  /* soc3_0 */
