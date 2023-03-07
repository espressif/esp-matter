/*******************************************************************************
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
 ******************************************************************************/
/*
 ** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/include/config.h#3
 */

/*! \file   "config.h"
 *   \brief  This file includes the various configurable parameters for
 *           customers
 *
 *    This file ncludes the configurable parameters except the parameters
 *    indicate the turning-on/off of some features
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef LINUX
#include <linux/version.h>
#endif

#if (CFG_WIFI_SWLA_ENABLE == 1)
#include "swla.h"
#endif
/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */
/* 2 Flags for OS capability */

#if defined(_HIF_AXI)
#ifdef LINUX
#ifdef CONFIG_X86
#define MTK_WCN_HIF_AXI			0
#else
#define MTK_WCN_HIF_AXI			1
#endif
#else
#define MTK_WCN_HIF_AXI			0
#endif
#else
#define MTK_WCN_HIF_AXI			0
#endif

#ifndef CFG_PSRAM_ENABLE
#define CFG_PSRAM_ENABLE		0
#endif

#ifndef CFG_ENABLE_WFDMA_DVT
#define CFG_ENABLE_WFDMA_DVT 0
#endif

#ifndef CFG_STATIC_MEM_ALLOC
#define CFG_STATIC_MEM_ALLOC 1
#endif

#ifndef CONFIG_WIFI_MEM_DBG
#define CONFIG_WIFI_MEM_DBG 0
#endif

#ifdef MTK_WLAN_MEM_PREALLOC_ENABLE
#define CFG_WLAN_MEM_CACHE_ALIGN        0
#endif

#ifndef CFG_WLAN_MEM_CACHE_ALIGN
#define CFG_WLAN_MEM_CACHE_ALIGN        1
#endif

#ifndef CFG_WLAN_CACHE_HIT_DBG
#define CFG_WLAN_CACHE_HIT_DBG 0
#endif

#ifndef CFG_WLAN_TX_PATH_DBG
#define CFG_WLAN_TX_PATH_DBG 0
#endif

#ifndef CFG_WLAN_LWIP_ZERO_COPY
#define CFG_WLAN_LWIP_ZERO_COPY LWIP_NETIF_TX_ZEROCP_ENABLE
#endif

#ifdef CFG_WLAN_LWIP_RXZERO_COPY
#undef CFG_WLAN_LWIP_RXZERO_COPY
#define CFG_WLAN_LWIP_RXZERO_COPY 0
#endif

#ifndef CFG_WLAN_LWIP_ZERO_COPY_DBG
#define CFG_WLAN_LWIP_ZERO_COPY_DBG 0
#endif

/* low perf CPU avoid context switch overhead */
#ifndef CONFIG_WF_AVOID_CONTEXT_SWITCH
#define CONFIG_WF_AVOID_CONTEXT_SWITCH LWIP_TCPIP_CORE_LOCKING
#endif

/* Android build-in driver switch, Mike 2016/11/11*/
#ifndef CFG_BUILT_IN_DRIVER
#define CFG_BUILT_IN_DRIVER         0
#endif

#ifndef CFG_ACPI_STATE
#define CFG_ACPI_STATE             0
#endif

#ifndef CFG_SUPPORT_PERMON
#define CFG_SUPPORT_PERMON         0
#endif

#ifndef CFG_SUPPORT_DMASCH_QUOTA_CONFIG
#define CFG_SUPPORT_DMASCH_QUOTA_CONFIG 0
#endif

#ifndef CFG_SUPPORT_VOE_TEST
#define CFG_SUPPORT_VOE_TEST       0
#endif

#ifndef CFG_SUPPORT_DBGOPS
#if defined(MTK_RELEASE_MODE) && (MTK_RELEASE_MODE == MTK_M_RELEASE)
#define CFG_SUPPORT_DBGOPS         0
#else
#define CFG_SUPPORT_DBGOPS         1
#endif
#endif

#if defined(MTK_RELEASE_MODE)
#define CFG_SUPPORT_NON_PREF_CHAN  0
#define CFG_SUPPORT_11KV_SWITCH    0
#else
#define CFG_SUPPORT_NON_PREF_CHAN  1
#define CFG_SUPPORT_11KV_SWITCH    1
#endif

/*
 * enum ENUM_MAX_BANDWIDTH_SETTING {
 *	MAX_BW_20MHZ = 0,
 *	MAX_BW_40MHZ,
 *	MAX_BW_80MHZ,
 *	MAX_BW_160MHZ,
 *	MAX_BW_80_80_MHZ,
 *	MAX_BW_UNKNOWN
 * };
*/
#define MAX_BW_20 (0)
#define MAX_BW_40 (1)
#define MAX_BW_80 (2)
#define MAX_BW_160 (3)
#define MAX_BW_80_80 (4)
#ifndef CFG_SUPPORT_MAX_BW
#define CFG_SUPPORT_MAX_BW         MAX_BW_20
#endif

/* Mike 2016/09/01 ALPS update K3.18 80211_disconnect to K4.4 version*/
/* work around for any alps K3.18 platform*/
#ifndef CFG_WPS_DISCONNECT
#define CFG_WPS_DISCONNECT         0
#endif


#if (CFG_SUPPORT_AEE == 1)
/* TODO: temp remove for 7663 on mobile */
/* #define CFG_ENABLE_AEE_MSG          1 */
#define CFG_ENABLE_AEE_MSG          0
#else
#define CFG_ENABLE_AEE_MSG          0
#endif

#define CFG_ENABLE_EARLY_SUSPEND        0
#define CFG_ENABLE_NET_DEV_NOTIFY		1

/* 2 Flags for Driver Features */
#define CFG_TX_FRAGMENT                 1 /*!< 1: Enable TX fragmentation */
/* 0: Disable */
#define CFG_SUPPORT_PERFORMANCE_TEST    0 /*Only for performance Test */

#define CFG_COUNTRY_CODE                NULL /* "US" */
#define CFG_SUPPORT_DEBUG_FS		0
#define CFG_SUPPORT_SET_CAM_BY_PROC	0 /*Enable/disable powersave mode*/

#ifndef LINUX
#define CFG_FW_FILENAME                 L"WIFI_RAM_CODE"
#define CFG_CR4_FW_FILENAME             L"WIFI_RAM_CODE2"
#else
#define CFG_FW_FILENAME                 "WIFI_RAM_CODE"
#define CFG_CR4_FW_FILENAME             "WIFI_RAM_CODE2"
#endif

#ifndef CFG_MET_PACKET_TRACE_SUPPORT
#define CFG_MET_PACKET_TRACE_SUPPORT    0 /*move to wlan/MAKEFILE */
#endif

#ifndef CFG_MET_TAG_SUPPORT
#define CFG_MET_TAG_SUPPORT             0
#endif

#ifndef CFG_SUPPORT_PERF_IND
#define CFG_SUPPORT_PERF_IND            0
#endif

/* Support AP Selection */
#define CFG_SUPPORT_RSN_SCORE		0
#define CFG_SELECT_BSS_BASE_ON_MULTI_PARAM	1
#define CFG_MAX_NUM_OF_CHNL_INFO		50
#ifndef CFG_SUPPORT_CHNL_CONFLICT_REVISE
#define CFG_SUPPORT_CHNL_CONFLICT_REVISE	0
#endif


/*------------------------------------------------------------------------------
 * Driver config
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_CFG
#define CFG_SUPPORT_CFG	        1
#endif

#ifndef CFG_SUPPORT_NVDM
#define CFG_SUPPORT_NVDM        0
#endif

#ifndef CFG_SUPPORT_CFG_FILE
#ifndef LINUX
#define CFG_SUPPORT_CFG_FILE	0
#else
#define CFG_SUPPORT_CFG_FILE	0
#endif
#endif

/*!< 1(default): Enable 802.11d */
#define CFG_SUPPORT_802_11D	0
/* 0: Disable */

/* Radio Reasource Measurement (802.11k) */
#define CFG_SUPPORT_RRM		0

/* If CSA new channel is DFS channel, link down directly*/
#define CFG_DFS_NEWCH_DFS_FORCE_DISCONNECT	1

/* 11n feature. RX RDG capability */
#define CFG_SUPPORT_RX_RDG	0

/* 802.11n MCS Feedback responder */
#define CFG_SUPPORT_MFB		0

/* 802.11n RX STBC (1SS) */
#define CFG_SUPPORT_RX_STBC	1

/* 802.11n RX short GI for both 20M and 40M BW */
#define CFG_SUPPORT_RX_SGI	1

/* 802.11n RX HT green-field capability */
#define CFG_SUPPORT_RX_HT_GF	1

#define CFG_SUPPORT_BFER	0
#define CFG_SUPPORT_BFEE	1

#ifndef CFG_SUPPORT_WAPI
#define CFG_SUPPORT_WAPI	0
#endif

/* Enable QA Tool Support */
#ifndef CFG_SUPPORT_QA_TOOL
#define CFG_SUPPORT_QA_TOOL	0
#endif

/* Enable QA Tool Support for ICAP */
#ifndef CFG_SUPPORT_ICAP
#define CFG_SUPPORT_ICAP    0
#endif

/* Enable TX BF Support */
#define CFG_SUPPORT_TX_BF	0

#define CFG_SUPPORT_TX_BF_FPGA	0

/* Enable MU MIMO Support */
#define CFG_SUPPORT_MU_MIMO	0

#ifndef CFG_WOW_SUPPORT
/* Enable WOW Support */
#define CFG_WOW_SUPPORT		1
#endif

/* Enable A-MSDU RX Reordering Support */
#define CFG_SUPPORT_RX_AMSDU	1

/* Enable Fragment Support */
#define CFG_SUPPORT_FRAG_SUPPORT 1

/* Enable frag and de-AMSDU security patch */
#define CFG_FRAG_DEAMSDU_ATTACK_DETECTION 1

#if (CFG_FRAG_DEAMSDU_ATTACK_DETECTION)

/* Enable A-MSDU Attack Detection */
#define CFG_SUPPORT_AMSDU_ATTACK_DETECTION 1

/* Enable Fragment Attack Detection */
#define CFG_SUPPORT_FRAG_ATTACK_DETECTION 1

/* Enable Fake EAPOL Detection */
#define CFG_SUPPORT_FAKE_EAPOL_DETECTION 1

/* Enable TKIP MIC ERROR Detection */
#define CFG_SUPPORT_TKIP_MICERROR_DETECTION 1

#else

/* Enable A-MSDU Attack Detection */
#define CFG_SUPPORT_AMSDU_ATTACK_DETECTION 0

/* Enable Fragment Attack Detection */
#define CFG_SUPPORT_FRAG_ATTACK_DETECTION 0

/* Enable Fake EAPOL Detection */
#define CFG_SUPPORT_FAKE_EAPOL_DETECTION 0

/* Enable TKIP MIC ERROR Detection */
#define CFG_SUPPORT_TKIP_MICERROR_DETECTION 0

#endif

/* Enable Android wake_lock operations */
#ifndef CFG_ENABLE_WAKE_LOCK
#define CFG_ENABLE_WAKE_LOCK	1
#endif

/*------------------------------------------------------------------------------
 * Flags of CSI
 *------------------------------------------------------------------------------
 */
#if defined(MTK_RELEASE_MODE)
#define CFG_SUPPORT_CSI         0
#else
#define CFG_SUPPORT_CSI         1
#endif

#if CFG_SUPPORT_CSI
#define CFG_SUPPORT_CSI_NF 1
#define CFG_CSI_DEBUG 0
#endif

#if CFG_SUPPORT_CSI_NF
#define CFG_SUPPORT_CSI_BFADJ 1
#define CFG_SUPPORT_CSI_BWDF 1
#endif

#define CFG_SUPPORT_OSHARE	1

#define CFG_SUPPORT_LOWLATENCY_MODE	1

#define CFG_SUPPORT_ANT_SWAP		0

#define CFG_SUPPORT_DATA_STALL			0

/*------------------------------------------------------------------------------
 * Support Extend Range feature
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_HE_ER
#define CFG_SUPPORT_HE_ER			0
#endif

/*------------------------------------------------------------------------------
 * Support Fast Connect
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_FAST_CONNECT
#define CFG_SUPPORT_FAST_CONNECT	0
#endif

/*------------------------------------------------------------------------------
 * Flags of Buffer mode SUPPORT
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_BUFFER_MODE                 1
#define CFG_SUPPORT_BUFFER_MODE_HDR             1
/*------------------------------------------------------------------------------
 * SLT Option
 *------------------------------------------------------------------------------
 */
#define CFG_SLT_SUPPORT				0

#ifdef NDIS60_MINIPORT
#define CFG_NATIVE_802_11                       1

#define CFG_TX_MAX_PKT_SIZE                     2304

/* !< 1: Enable TCP/IP header checksum offload */
#define CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60       0

/* 0: Disable */
#define CFG_TCP_IP_CHKSUM_OFFLOAD               1
#define CFG_WHQL_DOT11_STATISTICS               1
#define CFG_WHQL_ADD_REMOVE_KEY                 1
#define CFG_WHQL_CUSTOM_IE                      1
#define CFG_WHQL_SAFE_MODE_ENABLED              1

#else
#define CFG_TCP_IP_CHKSUM_OFFLOAD               1
#define CFG_TCP_IP_CHKSUM_OFFLOAD_NDIS_60       0
#define CFG_TX_MAX_PKT_SIZE                     1600
#define CFG_NATIVE_802_11                       0
#endif

/* By using GRO at NAPI level, the driver is doing the aggregation to a large
 * SKB very early, right at the receive completion handler. This means that all
 * the next functions in the receive stack do much less processing.
 * The GRO feature could enhance "Rx" tput.
 */
#define CFG_SUPPORT_RX_GRO                      0

/* 2 Flags for Driver Parameters */
/*------------------------------------------------------------------------------
 * Flags for EHPI Interface in Colibri Platform
 *------------------------------------------------------------------------------
 */
/*!< 1: Do workaround for faster bus timing */
/* 0(default): Disable */
#define CFG_EHPI_FASTER_BUS_TIMING                  0

/*------------------------------------------------------------------------------
 * Flags for UMAC
 *------------------------------------------------------------------------------
 */
#define CFG_UMAC_GENERATION                         0x20

/*------------------------------------------------------------------------------
 * Flags for HIFSYS Interface
 *------------------------------------------------------------------------------
 */
#ifdef _lint
/* #define _HIF_SDIO   1 */
#endif

/* 1(default): Enable SDIO ISR & TX/RX status enhance mode
 * 0: Disable
 */
#define CFG_SDIO_INTR_ENHANCE                        1
/* 1(default): Enable SDIO ISR & TX/RX status enhance mode
 * 0: Disable
 */
#define CFG_SDIO_RX_ENHANCE                          1
/* 1: Enable SDIO TX enhance mode(Multiple frames in single BLOCK CMD)
 * 0(default): Disable
 */
#define CFG_SDIO_TX_AGG                              1

/* 1: Enable SDIO RX enhance mode(Multiple frames in single BLOCK CMD)
 * 0(default): Disable
 */
#define CFG_SDIO_RX_AGG                              1

/* 1: Enable SDIO RX Workqueue De-Aggregation
 * 0(default): Disable
 */
#ifndef CFG_SDIO_RX_AGG_WORKQUE
#define CFG_SDIO_RX_AGG_WORKQUE                      0
#endif

#if (CFG_SDIO_RX_AGG == 1) && (CFG_SDIO_INTR_ENHANCE == 0)
#error \
	"CFG_SDIO_INTR_ENHANCE should be 1 once CFG_SDIO_RX_AGG equals to 1"
#elif (CFG_SDIO_INTR_ENHANCE == 1 || CFG_SDIO_RX_ENHANCE == 1) && \
	(CFG_SDIO_RX_AGG == 0)
#error \
	"CFG_SDIO_RX_AGG should be 1 once CFG_SDIO_INTR_ENHANCE and/or CFG_SDIO_RX_ENHANCE equals to 1"
#endif

#ifdef WINDOWS_CE
/*!< 1: Support pass through (PATHRU) mode */
#define CFG_SDIO_PATHRU_MODE                    1
/* 0: Disable */
#else
/*!< 0: Always disable if WINDOWS_CE is not defined */
#define CFG_SDIO_PATHRU_MODE                    0
#endif

#define CFG_SDIO_ACCESS_N9_REGISTER_BY_MAILBOX      0
#define CFG_MAX_RX_ENHANCE_LOOP_COUNT               3

#define CFG_USB_TX_AGG                              1
#define CFG_USB_CONSISTENT_DMA                      0
#define CFG_USB_TX_HANDLE_IN_HIF_THREAD             0
#define CFG_USB_RX_HANDLE_IN_HIF_THREAD             0

#ifndef CFG_TX_DIRECT_USB
#define CFG_TX_DIRECT_USB                           1
#endif
#ifndef CFG_RX_DIRECT_USB
#define CFG_RX_DIRECT_USB                           1
#endif

#define CFG_HW_WMM_BY_BSS                           1

/*------------------------------------------------------------------------------
 * Flags and Parameters for Integration
 *------------------------------------------------------------------------------
 */

#define CFG_MULTI_ECOVER_SUPPORT	1

#define CFG_ENABLE_CAL_LOG		1
#define CFG_REPORT_RFBB_VERSION		1

#define MAX_BSSID_NUM			4	/* MAX BSSID number */

#ifndef CFG_CHIP_RESET_SUPPORT
#define CFG_CHIP_RESET_SUPPORT          0
#endif

#if CFG_CHIP_RESET_SUPPORT
#define CFG_CHIP_RESET_HANG		0
#else
#define CFG_CHIP_RESET_HANG		0
#endif

#define HW_BSSID_NUM			4	/* HW BSSID number by chip */

/*------------------------------------------------------------------------------
 * Flags for workaround
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Flags for driver version
 *------------------------------------------------------------------------------
 */
#define CFG_DRV_OWN_VERSION	((uint16_t)((NIC_DRIVER_MAJOR_VERSION << 8) | \
				(NIC_DRIVER_MINOR_VERSION)))
#define CFG_DRV_PEER_VERSION	((uint16_t)0x0000)

/*------------------------------------------------------------------------------
 * Flags and Parameters for TX path
 *------------------------------------------------------------------------------
 */

/*! Maximum number of SW TX packet queue */
#define CFG_TX_MAX_PKT_NUM                      32

/*! Maximum number of SW TX CMD packet buffer */
#define CFG_TX_MAX_CMD_PKT_NUM                  32

/*------------------------------------------------------------------------------
 * Flags and Parameters for RX path
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * CONFIG_TITLE : Move BA from FW to Driver
 * OWNER            : Puff Wen
 * Description  : Move BA from FW to Driver
 *------------------------------------------------------------------------------
 */
#define CFG_M0VE_BA_TO_DRIVER                   0

/*! Max. descriptor number - sync. with firmware */
#if CFG_SLT_SUPPORT
#define CFG_NUM_OF_RX0_HIF_DESC                 42
#else
#define CFG_NUM_OF_RX0_HIF_DESC                 4
#endif
#define CFG_NUM_OF_RX1_HIF_DESC                 2

/*! Max. buffer hold by QM */
#define CFG_NUM_OF_QM_RX_PKT_NUM                HIF_NUM_OF_QM_RX_PKT_NUM

/*! Maximum number of SW RX packet buffer */
#define CFG_RX_MAX_PKT_NUM                      ((CFG_NUM_OF_RX0_HIF_DESC + \
						CFG_NUM_OF_RX1_HIF_DESC) * 2 \
						+ CFG_NUM_OF_QM_RX_PKT_NUM)

#define CFG_RX_REORDER_Q_THRESHOLD              8

#ifndef LINUX
#define CFG_RX_RETAINED_PKT_THRESHOLD           (CFG_NUM_OF_RX0_HIF_DESC + \
						CFG_NUM_OF_RX1_HIF_DESC \
						+ CFG_NUM_OF_QM_RX_PKT_NUM)
#else
#define CFG_RX_RETAINED_PKT_THRESHOLD           0
#endif

/*! Maximum RX packet size, if exceed this value, drop incoming packet */
/* 7.2.3 Maganement frames */
/* TODO: it should be 4096 under emulation mode */
#define CFG_RX_MAX_PKT_SIZE	(28 + 2312 + 12 /*HIF_RX_HEADER_T*/)

/*! Minimum RX packet size, if lower than this value, drop incoming packet */
#define CFG_RX_MIN_PKT_SIZE	10 /*!< 802.11 Control Frame is 10 bytes */

/*! RX BA capability */
#define CFG_NUM_OF_RX_BA_AGREEMENTS             80
#if CFG_M0VE_BA_TO_DRIVER
#define CFG_RX_BA_MAX_WINSIZE                   64
#endif
#define CFG_RX_BA_INC_SIZE                      64
#define CFG_RX_MAX_BA_TID_NUM                   8
#define CFG_RX_REORDERING_ENABLED               1

#define CFG_PF_ARP_NS_MAX_NUM                   3

#define CFG_COMPRESSION_DEBUG			0
#define CFG_DECOMPRESSION_TMP_ADDRESS		0
#define CFG_SUPPORT_COMPRESSION_FW_OPTION	0

/*------------------------------------------------------------------------------
 * Flags and Parameters for CMD/RESPONSE
 *------------------------------------------------------------------------------
 */
#define CFG_RESPONSE_POLLING_TIMEOUT            1000
#define CFG_RESPONSE_POLLING_DELAY              5

/*------------------------------------------------------------------------------
 * Flags and Parameters for Protocol Stack
 *------------------------------------------------------------------------------
 */
/*! Maximum number of BSS in the SCAN list */
#define CFG_MAX_NUM_BSS_LIST                    8

#define CFG_MAX_COMMON_IE_BUF_LEN         ((1500 * CFG_MAX_NUM_BSS_LIST) / 3)

/*! Maximum size of Header buffer of each SCAN record */
#define CFG_RAW_BUFFER_SIZE                      1024

/*! Maximum size of IE buffer of each SCAN record */
#define CFG_IE_BUFFER_SIZE                      512

/*------------------------------------------------------------------------------
 * Flags and Parameters for Power management
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_FULL_PM                      1
#define CFG_ENABLE_WAKEUP_ON_LAN                0

/* debug which packet wake up host */
#define CFG_SUPPORT_WAKEUP_REASON_DEBUG         1

#define CFG_INIT_POWER_SAVE_PROF		ENUM_PSP_FAST_SWITCH

#define CFG_INIT_ENABLE_PATTERN_FILTER_ARP	0

/* (BIT(3) | BIT(2) | BIT(1) | BIT(0)) */
#define CFG_INIT_UAPSD_AC_BMP			0

/* #define CFG_SUPPORT_WAPI			0 */
#define CFG_SUPPORT_WPS				1
#define CFG_SUPPORT_WPS2			1

/*------------------------------------------------------------------------------
 * Flags 1: drop all multicast packets when device suspend
 * Flags 0: drop multicast packets except white list when device suspend
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_DROP_ALL_MC_PACKET		0

/*------------------------------------------------------------------------------
 * Auto Channel Selection maximun channel number
 *------------------------------------------------------------------------------
 */
/* ARRAY_SIZE(mtk_5ghz_channels) + ARRAY_SIZE(mtk_2ghz_channels) */
#define MAX_CHN_NUM			39

#define MAX_2G_BAND_CHN_NUM		14
#define MAX_5G_BAND_CHN_NUM		(MAX_CHN_NUM - MAX_2G_BAND_CHN_NUM)
#define ACS_PRINT_BUFFER_LEN		200

/*------------------------------------------------------------------------------
 * Flags and Parameters for Ad-Hoc
 *------------------------------------------------------------------------------
 */
#define CFG_INIT_ADHOC_FREQ                     (2462000)
#define CFG_INIT_ADHOC_MODE                     AD_HOC_MODE_MIXED_11BG
#define CFG_INIT_ADHOC_BEACON_INTERVAL          (100)
#define CFG_INIT_ADHOC_ATIM_WINDOW              (0)

/*------------------------------------------------------------------------------
 * Maximum scan SSID number and channel number
 * Should be aligned with FW scan command
 *------------------------------------------------------------------------------
 */
#define SCAN_CMD_SSID_NUM                       (4)
#define SCAN_CMD_CHNL_NUM                       (32)

#if 1
/* to be compatible with old FW, we set ssid num to 0 here,
 * we should set correct num when query of scan capability from FW is done
 */
#define SCAN_CMD_EXT_SSID_NUM                   (0)
#define SCAN_CMD_EXT_CHNL_NUM                   (0)
#else
#define SCAN_CMD_EXT_SSID_NUM                   (6)
#define SCAN_CMD_EXT_CHNL_NUM                   (32)
#endif
#define CFG_SCAN_SSID_MAX_NUM (SCAN_CMD_SSID_NUM+SCAN_CMD_EXT_SSID_NUM)
#define MAXIMUM_OPERATION_CHANNEL_LIST (SCAN_CMD_CHNL_NUM+SCAN_CMD_EXT_CHNL_NUM)


/*------------------------------------------------------------------------------
 * Flags and Parameters for Load Setup Default
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Flags for enable 802.11A Band setting
 *------------------------------------------------------------------------------
 */

/*------------------------------------------------------------------------------
 * Flags and Parameters for Interrupt Process
 *------------------------------------------------------------------------------
 */
#define CFG_IST_LOOP_COUNT                      HIF_IST_LOOP_COUNT

#define CFG_INT_WRITE_CLEAR                     0

/* 2 Flags for Driver Debug Options */
/*------------------------------------------------------------------------------
 * Flags of TX Debug Option. NOTE(Kevin): Confirm with SA before modifying
 * following flags.
 *------------------------------------------------------------------------------
 */
/*!< 1: Debug statistics usage of MGMT Buffer */
/* 0: Disable */
#define CFG_DBG_MGT_BUF                         0

#define CFG_HIF_STATISTICS                      0

#define CFG_HIF_RX_STARVATION_WARNING           0

#define CFG_RX_PKTS_DUMP                        0

#define CFG_SUPPORT_STATISTICS			0

#define CFG_ASSERT_DUMP                         0

#define CFG_SUPPORT_TRACE_TC4			0
/*------------------------------------------------------------------------------
 * Flags of Firmware Download Option.
 *------------------------------------------------------------------------------
 */
#if 0
#define CFG_ENABLE_FW_PRELOAD                   1
#define CFG_ENABLE_FW_DOWNLOAD                  0
#else
#define CFG_ENABLE_FW_PRELOAD                   0
#define CFG_ENABLE_FW_DOWNLOAD                  1
#endif

#ifndef CFG_SUPPORT_CR4FW
#define CFG_SUPPORT_CR4FW                       0
#endif

#define CFG_ENABLE_FW_DOWNLOAD_ACK              1

#ifndef CFG_WIFI_IP_SET
#define CFG_WIFI_IP_SET                         1
#endif

/*------------------------------------------------------------------------------
 * Flags of Bluetooth-over-WiFi (BT 3.0 + HS) support
 *------------------------------------------------------------------------------
 */

#define CFG_ENABLE_BT_OVER_WIFI             0

#define CFG_BOW_SEPARATE_DATA_PATH              1

#define CFG_BOW_PHYSICAL_LINK_NUM               4

#define CFG_BOW_LIMIT_AIS_CHNL                  1

#define CFG_BOW_SUPPORT_11N                     1

#define CFG_BOW_RATE_LIMITATION                 1

/*------------------------------------------------------------------------------
 * Flags of Wi-Fi Direct support
 *------------------------------------------------------------------------------
 */
/*------------------------------------------------------------------------------
 * Support reporting all BSS networks to cfg80211 kernel when scan
 * request is from P2P interface
 * Originally only P2P networks will be reported when scan request is from p2p0
 *------------------------------------------------------------------------------
 */
#ifndef CFG_P2P_SCAN_REPORT_ALL_BSS
#define CFG_P2P_SCAN_REPORT_ALL_BSS            0
#endif

/* Allow connection with no P2P IE device */
#ifndef CFG_P2P_CONNECT_ALL_BSS
#define CFG_P2P_CONNECT_ALL_BSS            0
#endif

/* Allow setting max P2P GO client count */
#ifndef CFG_P2P_DEFAULT_CLIENT_COUNT
#define CFG_P2P_DEFAULT_CLIENT_COUNT 0
#endif
/*------------------------------------------------------------------------------
 * Flags for GTK rekey offload
 *------------------------------------------------------------------------------
 */

#ifndef CFG_ENABLE_WIFI_DIRECT
#define CFG_ENABLE_WIFI_DIRECT          0
#endif

#ifndef CONFIG_SUPPORT_GTK_REKEY
#define CONFIG_SUPPORT_GTK_REKEY        0
#endif

#ifndef CFG_SUPPORT_802_11W
/*!< 0(default): Disable 802.11W */
#define CFG_SUPPORT_802_11W             0
#endif

#define CFG_SUPPORT_PERSISTENT_GROUP            0

#define CFG_TEST_WIFI_DIRECT_GO                 0

#define CFG_TEST_ANDROID_DIRECT_GO              0

#define CFG_UNITEST_P2P                         0

/*
 * Enable cfg80211 option after Android 2.2(Froyo) is suggested,
 * cfg80211 on linux 2.6.29 is not mature yet
 */
#ifndef CFG_ENABLE_WIFI_DIRECT_CFG_80211
#define CFG_ENABLE_WIFI_DIRECT_CFG_80211        1
#endif

#define CFG_SUPPORT_HOTSPOT_OPTIMIZATION        0
#define CFG_HOTSPOT_OPTIMIZATION_BEACON_INTERVAL 300
#define CFG_HOTSPOT_OPTIMIZATION_DTIM           1
#define CFG_AUTO_CHANNEL_SEL_SUPPORT            0

#ifndef CFG_ENABLE_UNIFY_WIPHY
#define CFG_ENABLE_UNIFY_WIPHY 1
#endif

#define CFG_ENABLE_OFFCHANNEL_TX 1

/*------------------------------------------------------------------------------
 * Configuration Flags (Linux Only)
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_EXT_CONFIG                  0

/*------------------------------------------------------------------------------
 * Statistics Buffering Mechanism
 *------------------------------------------------------------------------------
 */
#if CFG_SUPPORT_PERFORMANCE_TEST
#define CFG_ENABLE_STATISTICS_BUFFERING         1
#else
#define CFG_ENABLE_STATISTICS_BUFFERING         0
#endif
#define CFG_STATISTICS_VALID_CYCLE              2000
#define CFG_LINK_QUALITY_VALID_PERIOD           500

/*------------------------------------------------------------------------------
 * Migration Option
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_ADHOC                       0
#if CFG_ENABLE_WIFI_DIRECT
#define CFG_SUPPORT_AAA                         1
#else
#define CFG_SUPPORT_AAA                         0
#endif

#if CFG_SUPPORT_ADHOC
#define CFG_SUPPORT_NOA                         1
#else
#define CFG_SUPPORT_NOA                         0
#endif

#define CFG_SUPPORT_BCM                         0
#define CFG_SUPPORT_BCM_BWCS                    0
#define CFG_SUPPORT_BCM_BWCS_DEBUG              0

#define CFG_SUPPORT_RDD_TEST_MODE               0

#define CFG_SUPPORT_PWR_MGT                     1

#define CFG_ENABLE_HOTSPOT_PRIVACY_CHECK        1

#define CFG_MGMT_FRAME_HANDLING                 1

#define CFG_MGMT_HW_ACCESS_REPLACEMENT          0

/* DFS (802.11h) */
#ifndef CFG_SUPPORT_DFS
#define CFG_SUPPORT_DFS		1
#endif
#if CFG_SUPPORT_AAA
#ifndef CFG_SUPPORT_DFS_MASTER
#define CFG_SUPPORT_DFS_MASTER		1
/* SoftAp Cross Band Channel Switch */
#define CFG_SUPPORT_IDC_CH_SWITCH	1
#endif
#else
#define CFG_SUPPORT_DFS_MASTER		0
#define CFG_SUPPORT_IDC_CH_SWITCH	0
#endif

#if (CFG_SUPPORT_DFS == 1)	/* Add by Enlai */
/* Quiet (802.11h) */
#define CFG_SUPPORT_QUIET	0
/* Spectrum Management (802.11h): TPC and DFS */
#define CFG_SUPPORT_SPEC_MGMT	1
#else
/* Quiet (802.11h) */
#define CFG_SUPPORT_QUIET	0
/* Spectrum Management (802.11h): TPC and DFS */
#define CFG_SUPPORT_SPEC_MGMT	0
#endif

#if CFG_SUPPORT_PERFORMANCE_TEST

#else

#endif

#define CFG_SUPPORT_AIS_5GHZ                    1
#define CFG_SUPPORT_BEACON_CHANGE_DETECTION     0

/*------------------------------------------------------------------------------
 * Option for NVRAM and Version Checking
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_NVRAM                       0
#define CFG_NVRAM_EXISTENCE_CHECK               1
#define CFG_SW_NVRAM_VERSION_CHECK              1
#define CFG_SUPPORT_NIC_CAPABILITY              1
#define CFG_SUPPORT_NVRAM_FRAGMENT              0

/*------------------------------------------------------------------------------
 * CONFIG_TITLE : Stress Test Option
 * OWNER        : Puff Wen
 * Description  : For stress test only. DO NOT enable it while normal operation
 *------------------------------------------------------------------------------
 */
#define CFG_STRESS_TEST_SUPPORT                 0

/*------------------------------------------------------------------------------
 * Flags for LINT
 *------------------------------------------------------------------------------
 */
#define LINT_SAVE_AND_DISABLE	/*lint -save -e* */

#define LINT_RESTORE		/*lint -restore */

#define LINT_EXT_HEADER_BEGIN		LINT_SAVE_AND_DISABLE

#define LINT_EXT_HEADER_END		LINT_RESTORE

/*------------------------------------------------------------------------------
 * Flags of Features
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_TDLS
#define CFG_SUPPORT_TDLS		0
#endif

/* Enable/disable QoS TX, AMPDU */
#define CFG_SUPPORT_QOS			1

#define CFG_SUPPORT_AMPDU_TX		1
#define CFG_SUPPORT_AMPDU_RX		1

/* Enable/disable TS-related Action frames handling */
#define CFG_SUPPORT_TSPEC		0
#define CFG_SUPPORT_UAPSD		1
#define CFG_SUPPORT_UL_PSMP		0

/* Roaming System */
#ifndef CFG_SUPPORT_ROAMING
#define CFG_SUPPORT_ROAMING		0
#endif

#if (CFG_SUPPORT_ROAMING == 1)

/* Roaming feature: skip roaming when only one ESSID AP
 * Need Android background scan
 * if no roaming event occurred
 * to trigger roaming scan
 * after skip roaming in one ESSID AP case
 */
#define CFG_SUPPORT_ROAMING_SKIP_ONE_AP		0
#define CFG_SUPPORT_DRIVER_ROAMING		1
#define CFG_SUPPORT_ROAMING_CUSTOMIZED		1
#else
#define CFG_SUPPORT_ROAMING_SKIP_ONE_AP		0
#define CFG_SUPPORT_DRIVER_ROAMING		0
#define CFG_SUPPORT_ROAMING_CUSTOMIZED		0

#endif /* CFG_SUPPORT_ROAMING */

#define CFG_SUPPORT_SWCR			0

#define CFG_SUPPORT_ANTI_PIRACY			1

#define CFG_SUPPORT_OSC_SETTING			1

#define CFG_SUPPORT_P2P_RSSI_QUERY		0

#define CFG_SUPPORT_P2P_GO_OFFLOAD_PROBE_RSP	0

#define CFG_SHOW_MACADDR_SOURCE			1

#define CFG_SHOW_FULL_MACADDR     1

#ifndef CFG_SUPPORT_VO_ENTERPRISE
#define CFG_SUPPORT_VO_ENTERPRISE               1
#endif

#define CFG_SUPPORT_WMM_AC                      1

#ifndef CFG_SUPPORT_MBO
#define CFG_SUPPORT_MBO                         1
#endif
#define CFG_SUPPORT_RIC                         0

#if (CFG_SUPPORT_VO_ENTERPRISE || CFG_SUPPORT_MBO)
#define CFG_SUPPORT_802_11R                     1
#define CFG_SUPPORT_802_11K                     1
#else
#define CFG_SUPPORT_802_11R                     0
#define CFG_SUPPORT_802_11K                     0
#endif

#if CFG_SUPPORT_802_11K
#define CFG_SUPPORT_BCN_REQ_PASSIVE_MODE        1
#define CFG_SUPPORT_BCN_REQ_ACTIVE_MODE         1
#define CFG_SUPPORT_BCN_REQ_TABLE_MODE          1
#else
#define CFG_SUPPORT_BCN_REQ_PASSIVE_MODE        0
#define CFG_SUPPORT_BCN_REQ_ACTIVE_MODE         0
#define CFG_SUPPORT_BCN_REQ_TABLE_MODE          0
#endif


/*------------------------------------------------------------------------------
 * Support supplicant SME/MLME.
 * Let upper layer to control auth/assoc,
 * driver process T/RX Auth/Assoc w/o SAA FSM
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SUPPLICANT_SME              0

#if (CFG_SUPPORT_802_11K == 1) && (CFG_SUPPORT_SUPPLICANT_SME == 1)
/* Enable to do beacon reports by supplicant.
 * Beacon report is a sub-feature of 802_11K(RRM)
 * Supplicant only support RRM when SME supported.
 */
#define CFG_SUPPORT_RM_BEACON_REPORT_BY_SUPPLICANT 0
#else
#define CFG_SUPPORT_RM_BEACON_REPORT_BY_SUPPLICANT 0
#endif

/* Support 802.11v Wireless Network Management */
#ifndef CFG_SUPPORT_802_11V
#define CFG_SUPPORT_802_11V                     1
#endif

#if CFG_SUPPORT_802_11V
#define CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT  1
#else
#define CFG_SUPPORT_802_11V_BSS_TRANSITION_MGT  0
#endif

#define CFG_SUPPORT_802_11V_TIMING_MEASUREMENT	0

#if (CFG_SUPPORT_802_11V_TIMING_MEASUREMENT == 1) && \
	(CFG_SUPPORT_802_11V == 0)
#error \
"CFG_SUPPORT_802_11V should be 1 once CFG_SUPPORT_802_11V_TIMING_MEASUREMENT equals to 1"
#endif

#define WNM_UNIT_TEST CFG_SUPPORT_802_11V

#define CFG_SUPPORT_802_11V_MBSSID		0
#if (CFG_SUPPORT_802_11AX == 1)
/*11v MBSSID is mandatory for 11ax*/
#undef CFG_SUPPORT_802_11V_MBSSID
#define CFG_SUPPORT_802_11V_MBSSID		1
#endif

#define CFG_SUPPORT_SUPPLICANT_MBO		1

#define CFG_SUPPORT_PPR2			1
#define CFG_DRIVER_COMPOSE_ASSOC_REQ		1
#define CFG_SUPPORT_802_11AC			1
#define CFG_STRICT_CHECK_CAPINFO_PRIVACY	0

#if CFG_ENABLE_WIFI_DIRECT
#ifndef CFG_SUPPORT_WFD
#define CFG_SUPPORT_WFD				1
#define CFG_SUPPORT_WFD_COMPOSE_IE		1
#endif
#else
#define CFG_SUPPORT_WFD				0
#define CFG_SUPPORT_WFD_COMPOSE_IE		0
#endif

/* Support Customer vendor IE */
#define CFG_SUPPORT_CUSTOM_VENDOR_IE		1

#ifndef CFG_SUPPORT_HOTSPOT_WPS_MANAGER
#define CFG_SUPPORT_HOTSPOT_WPS_MANAGER		1
#endif
#define CFG_SUPPORT_NFC_BEAM_PLUS		1

/* Refer to CONFIG_MTK_STAGE_SCAN */
#define CFG_MTK_STAGE_SCAN			1

/* Enable driver support multicore */
#ifndef CFG_SUPPORT_MULTITHREAD
#define CFG_SUPPORT_MULTITHREAD		0
#endif


#define CFG_SUPPORT_MTK_SYNERGY			1

#define CFG_SUPPORT_VHT_IE_IN_2G		1

#ifndef CFG_SUPPORT_PWR_LIMIT_COUNTRY
#if (CFG_SUPPORT_SINGLE_SKU_LOCAL_DB == 0)
#error "If CFG_SUPPORT_PWR_LIMIT_COUNTRY == 0"
#error "Need define CFG_SUPPORT_SINGLE_SKU_LOCAL_DB 1"
#error "please make sure regd_is_single_sku_en is true too"
#endif
#define CFG_SUPPORT_PWR_LIMIT_COUNTRY	0
#endif

#define CFG_SUPPORT_DYNAMIC_PWR_LIMIT		0

#define CFG_FIX_2_TX_PORT			0

#define CFG_CHANGE_CRITICAL_PACKET_PRIORITY	0

#if (CFG_ENABLE_WIFI_DIRECT == 1) && defined(MTK_RELEASE_MODE)
#define CFG_SUPPORT_APPEND_VENDOR_IE		0
#else
#define CFG_SUPPORT_APPEND_VENDOR_IE		1
#endif
/*------------------------------------------------------------------------------
 * Flags of bus error tolerance
 *------------------------------------------------------------------------------
 */
#define CFG_FORCE_RESET_UNDER_BUS_ERROR     0

/*------------------------------------------------------------------------------
 * Build Date Code Integration
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_BUILD_DATE_CODE         0

/*------------------------------------------------------------------------------
 * Flags of SDIO test pattern support
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SDIO_READ_WRITE_PATTERN 0

/*------------------------------------------------------------------------------
 * Flags of Workaround
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_READ_EXTRA_4_BYTES       1

/* Handle IOT issue for 11ac certification */
#define CFG_OPMODE_CONFLICT_OPINFO	1

/*------------------------------------------------------------------------------
 * Flags of Packet Lifetime Profiling Mechanism
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_PKT_LIFETIME_PROFILE     0
#define CFG_PRINT_PKT_LIFETIME_PROFILE      0

#define CFG_ENABLE_PER_STA_STATISTICS       0

#define CFG_ENABLE_PER_STA_STATISTICS_LOG   0

/*------------------------------------------------------------------------------
 * Flags for prepare the FW compile flag
 *------------------------------------------------------------------------------
 */
#define COMPILE_FLAG0_GET_STA_LINK_STATUS     (1<<0)
#define COMPILE_FLAG0_WFD_ENHANCEMENT_PROTECT (1<<1)

/*------------------------------------------------------------------------------
 * Flags of Batch Scan SUPPORT
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_BATCH_SCAN             (0)
#define CFG_BATCH_MAX_MSCAN                (2)

/*------------------------------------------------------------------------------
 * Flags of SCHEDULE SCAN SUPPORT
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_SCHED_SCAN
#define CFG_SUPPORT_SCHED_SCAN             (0)
#endif
#define SCHED_SCAN_CMD_VERSION             (1)

/* this value should be aligned to auSsid in struct CMD_SCHED_SCAN_REQ */
#define CFG_SCAN_HIDDEN_SSID_MAX_NUM       (10)
/* this value should be aligned to auMatchSsid in struct CMD_SCHED_SCAN_REQ */
#define CFG_SCAN_SSID_MATCH_MAX_NUM        (16)

/*------------------------------------------------------------------------------
 * Full2Partial Scan SUPPORT
 *------------------------------------------------------------------------------
 */
/* During a full2partial scan period, all online full scan requests would be
 * changed to partial scan. The unit of this value is second
 */
#define CFG_SUPPORT_FULL2PARTIAL_SCAN      (0)
#define CFG_SCAN_FULL2PARTIAL_PERIOD       (60)

/*------------------------------------------------------------------------------
 * Value of scan cache result
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SCAN_CACHE_RESULT      (0)
#define CFG_SCAN_CACHE_RESULT_PERIOD       (7000)	/* Unit: ms */
#define CFG_SCAN_CACHE_MIN_CHANNEL_NUM     (10)

/*------------------------------------------------------------------------------
 * Default value: the duration in ms to check TRX
 *                      while the beacon timeout event comes.
 * This is the default value for
 *                      prAdapter->rWifiVar.u4BeaconTimoutFilterDurationMs
 * can customize
 *        1. by project's requirement in this default value
 *        2. or by define in wifi.cfg directly (BeaconTimoutFilterDurationMs)
 * if the value set to 0, it means disable the filter.
 * if the value set to 2000, it means the duration of fitler is 2000 ms
 *------------------------------------------------------------------------------
 */
#define CFG_BEACON_TIMEOUT_FILTER_DURATION_DEFAULT_VALUE	2000

/*------------------------------------------------------------------------------
 * Flags of Random MAC support
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SCAN_RANDOM_MAC        (1)

/*------------------------------------------------------------------------------
 * Flags of Sniffer SUPPORT
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SNIFFER                 0

/*------------------------------------------------------------------------------
 * Flags of Sniffer SUPPORT
 *------------------------------------------------------------------------------
 */
#ifndef CFG_DUAL_P2PLIKE_INTERFACE
#define CFG_DUAL_P2PLIKE_INTERFACE         1
#endif

#define RUNNING_P2P_MODE 0
#define RUNNING_AP_MODE 1
#define RUNNING_DUAL_AP_MODE 2
#define RUNNING_P2P_AP_MODE 3
#define RUNNING_P2P_MODE_NUM 4

/*------------------------------------------------------------------------------
 * Flags of MSP SUPPORT
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_MSP				1



/*------------------------------------------------------------------------------
 * Flags of driver fw customization
 *------------------------------------------------------------------------------
 */

#define CFG_SUPPORT_EASY_DEBUG               1


/*------------------------------------------------------------------------------
 * Flags of driver delay calibration atfer efuse buffer mode CMD
 *------------------------------------------------------------------------------
 */

#define CFG_EFUSE_BUFFER_MODE_DELAY_CAL         1

/*------------------------------------------------------------------------------
 * Flags of Drop Packet Replay SUPPORT
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_REPLAY_DETECTION		1

/*------------------------------------------------------------------------------
 * Flags of driver EEPROM pages for QA tool
 *------------------------------------------------------------------------------
 */

#define CFG_EEPROM_PAGE_ACCESS         1

/*------------------------------------------------------------------------------
 * Flags for HOST_OFFLOAD
 *------------------------------------------------------------------------------
 */

#define CFG_SUPPORT_WIFI_HOST_OFFLOAD	0

/*------------------------------------------------------------------------------
 * Flags for DBDC Feature
 *------------------------------------------------------------------------------
 */

#define CFG_SUPPORT_DBDC	0
#define CFG_SUPPORT_DBDC_NO_BLOCKING_OPMODE 1
#if CFG_SUPPORT_AAA
#ifndef CFG_SUPPORT_SAP_DFS_CHANNEL
#define CFG_SUPPORT_SAP_DFS_CHANNEL 1
#endif
#endif

/*------------------------------------------------------------------------------
 * Flags for Using TC4 Resource in ROM code stage
 *------------------------------------------------------------------------------
 */

#define CFG_USE_TC4_RESOURCE_FOR_INIT_CMD	0

/*------------------------------------------------------------------------------
 * Flags for Efuse Start and End address report from FW
 *------------------------------------------------------------------------------
 */

#define CFG_FW_Report_Efuse_Address	1

/*------------------------------------------------------------------------------
 * FW name max length
 *------------------------------------------------------------------------------
 */
#define CFG_FW_NAME_MAX_LEN	(64)

/*------------------------------------------------------------------------------
 * Support WMT WIFI Path Config
 *------------------------------------------------------------------------------
 */
#define CFG_WMT_WIFI_PATH_SUPPORT	0

/*------------------------------------------------------------------------------
 * Support CFG_SISO_SW_DEVELOP
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SISO_SW_DEVELOP
#define CFG_SISO_SW_DEVELOP			0
#endif
/*------------------------------------------------------------------------------
 * Support spatial extension control
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_SPE_IDX_CONTROL
#define CFG_SUPPORT_SPE_IDX_CONTROL		0
#endif

/*------------------------------------------------------------------------------
 * Flags for a Goal for MT6632 : Cal Result Backup in Host or NVRam when Android
 *                               Boot
 *------------------------------------------------------------------------------
 */
#if 0 /*(MTK_WCN_HIF_SDIO) : 20161003 Default Off, later will enable
       *                     by MTK_WCN_HIF_SDIO
       */
#define CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST				1
#define CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST_DBGLOG		0
#else
#define CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST				0
#define CFG_SUPPORT_CAL_RESULT_BACKUP_TO_HOST_DBGLOG		0
#endif

/*------------------------------------------------------------------------------
 * Enable SDIO 1-bit Data Mode. (Usually debug only)
 *------------------------------------------------------------------------------
 */
#define CFG_SDIO_1BIT_DATA_MODE			0

/*------------------------------------------------------------------------------
 * Single Sku
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_SINGLE_SKU	0
#ifndef CFG_SUPPORT_SINGLE_SKU_LOCAL_DB
#define CFG_SUPPORT_SINGLE_SKU_LOCAL_DB 0
#endif


/*------------------------------------------------------------------------------
 * Direct Control for RF/PHY/BB/MAC for Manual Configuration via command/api
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_ADVANCE_CONTROL 1

#ifndef CFG_SUPPORT_TRAFFIC_REPORT
#define CFG_SUPPORT_TRAFFIC_REPORT 1
#endif

#ifndef CFG_SUPPORT_RX_TRAFFIC_REPORT
#define CFG_SUPPORT_RX_TRAFFIC_REPORT 0
#endif

#ifndef CFG_SUPPORT_TRAFFIC_REPORT_VER
#define CFG_SUPPORT_TRAFFIC_REPORT_VER 2
#endif
/*------------------------------------------------------------------------------
 * Driver pre-allocate total size of memory in one time
 *------------------------------------------------------------------------------
 */
#ifndef CFG_PRE_ALLOCATION_IO_BUFFER
#define CFG_PRE_ALLOCATION_IO_BUFFER 0
#endif

/*------------------------------------------------------------------------------
 * Auto enable SDIO asynchronous interrupt mode
 *------------------------------------------------------------------------------
 */
#define CFG_SDIO_ASYNC_IRQ_AUTO_ENABLE	1

/*------------------------------------------------------------------------------
 * Flags to force enable performance monitor even when screen is OFF
 *------------------------------------------------------------------------------
 */
#define CFG_FORCE_ENABLE_PERF_MONITOR	0

/*------------------------------------------------------------------------------
 * Flags to ignore invalid auth tsn issue (ex. ALPS03089071)
 *------------------------------------------------------------------------------
 */
#define CFG_IGNORE_INVALID_AUTH_TSN	1

/*------------------------------------------------------------------------------
 * Flags of Network Controlled HandOver(NCHO) support
 * TC10 only: To improve the voice quality during handover,
 *		the NCHO is required to precisely control scanning parameters
 * CFG_SUPPORT_NCHO: 1: support, 0: not support
 * CFG_SUPPORT_NCHO_AUTO_ENABLE: sub-feature depends with CFG_SUPPORT_NCHO
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_NCHO		0
#define CFG_SUPPORT_NCHO_AUTO_ENABLE	0

/*------------------------------------------------------------------------------
 * Support OWE (Opportunistic Wireless Encryption)
 *------------------------------------------------------------------------------
  */
#define CFG_SUPPORT_OWE			0

/*------------------------------------------------------------------------------
 * Flags of Key Word Exception Mechanism
 *------------------------------------------------------------------------------
 */
#define CFG_ENABLE_KEYWORD_EXCEPTION_MECHANISM  0

/*------------------------------------------------------------------------------
 * Flags of WPA3 support
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_WPA3
#define CFG_SUPPORT_WPA3	0
#endif
#ifndef CFG_SUPPORT_ICS
#define CFG_SUPPORT_ICS		0
#endif

#ifndef CFG_SUPPORT_DPP
#define CFG_SUPPORT_DPP	0
#endif

/*------------------------------------------------------------------------------
 * Driver supports preferred frequency list for p2p operating channel
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_P2P_PREFERRED_FREQ_LIST  0

/*-----------------------------------------------------------------------------
* Flags to support IOT AP blacklist
*------------------------------------------------------------------------------
*/
#ifndef CFG_SUPPORT_IOT_AP_BLACKLIST
#if CFG_SUPPORT_DBDC && CFG_SUPPORT_CFG_FILE
#define CFG_SUPPORT_IOT_AP_BLACKLIST 1
#else
#define CFG_SUPPORT_IOT_AP_BLACKLIST 0
#endif
#endif

#if CFG_SUPPORT_IOT_AP_BLACKLIST
#define CFG_IOT_AP_RULE_MAX_CNT 32
#define CFG_IOT_AP_DATA_MAX_LEN 16
#endif
/*------------------------------------------------------------------------------
 * Driver supports reporting max tx rate instead of current tx rate
 * in mtk_cfg80211_get_station
 *------------------------------------------------------------------------------
 */
#define CFG_REPORT_MAX_TX_RATE	0

/*------------------------------------------------------------------------------
 * Link Quality Monitor
 * Link quality monitor execution period base on performance monitor timer
 * CFG_LINK_QUALITY_MONITOR_UPDATE_FREQUENCY base on PERF_MON_UPDATE_INTERVAL
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_LINK_QUALITY_MONITOR
#define CFG_SUPPORT_LINK_QUALITY_MONITOR 1
#endif
#if CFG_SUPPORT_LINK_QUALITY_MONITOR
#define CFG_LINK_QUALITY_MONITOR_UPDATE_FREQUENCY	1
#else
#define CFG_LINK_QUALITY_MONITOR_UPDATE_FREQUENCY  0
#endif


/*------------------------------------------------------------------------------
 * Driver supports SYS DVT automation
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_WIFI_SYSDVT
#define CFG_SUPPORT_WIFI_SYSDVT  0
#endif

#ifndef CFG_SUPPORT_DMASHDL_SYSDVT
#define CFG_SUPPORT_DMASHDL_SYSDVT  0
#endif

/*------------------------------------------------------------------------------
 * Flags of using wlan_assistant to read/write NVRAM
 *------------------------------------------------------------------------------
 */
#define CFG_WLAN_ASSISTANT_NVRAM		0

/*------------------------------------------------------------------------------
 * Flags of WORKAROUND HWITS00012836 WTBL_SEARCH_FAIL
 *------------------------------------------------------------------------------
 */
#ifndef CFG_WIFI_WORKAROUND_HWITS00012836_WTBL_SEARCH_FAIL
#define CFG_WIFI_WORKAROUND_HWITS00012836_WTBL_SEARCH_FAIL 0
#endif

/*------------------------------------------------------------------------------
 * Flags of enabling check if TX ethernet-II frame has empty payload. If yes,
 * then driver drops it.
 *------------------------------------------------------------------------------
 */
#ifndef CFG_WIFI_TX_ETH_CHK_EMPTY_PAYLOAD
#define CFG_WIFI_TX_ETH_CHK_EMPTY_PAYLOAD    0
#endif

/*------------------------------------------------------------------------------
 * CONNINFRA SUPPORT (Without WMT)
 * CFG_SUPPORT_CONNINFRA: 1 : conninfra driver exist
 *                        0 : conninfra driver doesn't exist
 * We replace WMT driver with CONNINFRA driver in mobile project
 * after connac2.0.
 * CFG_MTK_ANDROID_WMT is used to separate between mobile project and others.
 * Although WMT driver is removed, CFG_MTK_ANDROID_WMT still need to be set 1
 * for some code flow in mobile gen4m.
 *------------------------------------------------------------------------------
 */
#define CFG_SUPPORT_CONNINFRA 0

/*------------------------------------------------------------------------------
 * Flags of Disconnect with disable channel based on REGD update
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_REGD_UPDATE_DISCONNECT_ALLOWED
#define CFG_SUPPORT_REGD_UPDATE_DISCONNECT_ALLOWED  0
#endif

/*------------------------------------------------------------------------------
 * Notify clients to reconnect when channel switch in hotspot mode to avoid IOT
 * issues, eg. cross band switch.
 *------------------------------------------------------------------------------
 */
#define CFG_SEND_DEAUTH_DURING_CHNL_SWITCH    1

#define CFG_SUPPORT_CUSTOM_NETLINK          0
#if CFG_SUPPORT_CUSTOM_NETLINK
#define CFG_SUPPORT_TX_BEACON_STA_MODE      0
#else
#define CFG_SUPPORT_TX_BEACON_STA_MODE      0
#endif

/*------------------------------------------------------------------------------
 * Flags of Driver Coredump Support
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_DRIVER_COREDUMP
#define CFG_SUPPORT_DRIVER_COREDUMP  1
#endif
/*--------------- * Support tx MGMT frame use ACQ or not. *--------------- */
#ifndef CFG_SUPPORT_TX_MGMT_USE_DATAQ
#define CFG_SUPPORT_TX_MGMT_USE_DATAQ  0
#endif
/*---------------- * Support bt/wifi isolation detect or not *------------ */
#ifndef CFG_WIFI_ISO_DETECT
#define CFG_WIFI_ISO_DETECT  1
#endif
/*---------------- * Driver supports WiFi get noise *----------------------- */
#ifndef CONFIG_WIFI_SUPPORT_GET_NOISE
#define CONFIG_WIFI_SUPPORT_GET_NOISE  1
#endif

/*-------------- * Support EFUSE / EEPROM Auto Detect *--------------------- */
#ifndef CFG_EFUSE_AUTO_MODE_SUPPORT
#define CFG_EFUSE_AUTO_MODE_SUPPORT 1
#endif

#ifndef CFG_TX_DYN_CMD_SUPPORT
#define CFG_TX_DYN_CMD_SUPPORT                  0
#endif


/*------------------------------------------------------------------------------
 * Support Debug SOP or not.
 *------------------------------------------------------------------------------
 */
#ifndef CFG_SUPPORT_DEBUG_SOP
#define CFG_SUPPORT_DEBUG_SOP  0
#endif

/*------------------------------------------------------------------------------
 * Suppoot to get Tx/Rx MCS Info
 *------------------------------------------------------------------------------
 */
#ifndef CFG_WIFI_GET_MCS_INFO
#define CFG_WIFI_GET_MCS_INFO 0
#endif

#if (CFG_WIFI_GET_MCS_INFO == 1)
#define MCS_INFO_SAMPLE_CNT                 10
#define MCS_INFO_SAMPLE_PERIOD              100 /* Unit: ms */
#endif

#ifndef CFG_WIFI_LP_IMPROVE
#define CFG_WIFI_LP_IMPROVE 0
#endif


/*------------------------------------------------------------------------------
 * RAM mode switch
 *------------------------------------------------------------------------------
 */

#if (CFG_ZIDATA_IN_MEM == 0) /* SYSRAM */
#define WLAN_ATTR_ZIDATA_IN_MEM ATTR_ZIDATA_IN_SYSRAM
#elif (CFG_ZIDATA_IN_MEM == 1) && (CFG_PSRAM_ENABLE == 1) /* PSRAM */
#define WLAN_ATTR_ZIDATA_IN_MEM ATTR_ZIDATA_IN_RAM
#else /* no attribute */
#define WLAN_ATTR_ZIDATA_IN_MEM
#endif

#if (CFG_CODE_IN_MEM_TX == 0) /* SYSRAM */
#define WLAN_ATTR_TEXT_IN_MEM_TX ATTR_TEXT_IN_SYSRAM
#elif (CFG_CODE_IN_MEM_TX == 1) && (CFG_PSRAM_ENABLE == 1) /* PSRAM */
#define WLAN_ATTR_TEXT_IN_MEM_TX ATTR_TEXT_IN_RAM
#else /* no attribute */
#define WLAN_ATTR_TEXT_IN_MEM_TX
#endif

#if (CFG_CODE_IN_MEM_RX == 0) /* SYSRAM */
#define WLAN_ATTR_TEXT_IN_MEM_RX ATTR_TEXT_IN_SYSRAM
#elif (CFG_CODE_IN_MEM_RX == 1) && (CFG_PSRAM_ENABLE == 1) /* PSRAM */
#define WLAN_ATTR_TEXT_IN_MEM_RX ATTR_TEXT_IN_RAM
#else /* no attribute */
#define WLAN_ATTR_TEXT_IN_MEM_RX
#endif

/*------------------------------------------------------------------------------
  * Flags of antenna diversity
 *------------------------------------------------------------------------------
 */
#if defined(MTK_RELEASE_MODE)
#define CFG_SUPPORT_ANT_DIV         0
#else
#define CFG_SUPPORT_ANT_DIV         1
#endif

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

/*******************************************************************************
 *                   F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */
#endif /* _CONFIG_H */
