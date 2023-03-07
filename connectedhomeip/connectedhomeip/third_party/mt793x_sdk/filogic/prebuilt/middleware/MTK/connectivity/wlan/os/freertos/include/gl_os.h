
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
 ** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/include
 *      /gl_os.h#4
 */

/*! \file   gl_os.h
 *    \brief  List the external reference to OS for GLUE Layer.
 *
 *    In this file we define the data structure - GLUE_INFO_T to store those
 *    objects
 *    we acquired from OS - e.g. TIMER, SPINLOCK, NET DEVICE ... . And all the
 *    external reference (header file, extern func() ..) to OS for GLUE Layer
 *    should also list down here.
 */


#ifndef _GL_OS_H
#define _GL_OS_H
#include "connsys_wifi_queue.h"
#include "connsys_wifi_config.h"
#include "gl_upperlayer.h"
#include "hif.h"
#include "hal_nvic.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "memory_map.h"
#include "memory_attribute.h"
#include "hal_cache.h"

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */
/*------------------------------------------------------------------------------
 * Flags for LINUX(OS) dependent
 *------------------------------------------------------------------------------
 */
#define CFG_MAX_WLAN_DEVICES 1 /* number of wlan card will coexist */

#define CFG_MAX_TXQ_NUM 4 /* number of tx queue for support multi-queue h/w  */

/* 1: Enable use of SPIN LOCK Bottom Half for LINUX */
/* 0: Disable - use SPIN LOCK IRQ SAVE instead */
#define CFG_USE_SPIN_LOCK_BOTTOM_HALF       0

/* 1: Enable - Drop ethernet packet if it < 14 bytes.
 * And pad ethernet packet with dummy 0 if it < 60 bytes.
 * 0: Disable
 */
#define CFG_TX_PADDING_SMALL_ETH_PACKET     0

#define CFG_TX_STOP_NETIF_QUEUE_THRESHOLD   256	/* packets */

#define CFG_TX_STOP_NETIF_PER_QUEUE_THRESHOLD   256	/* packets */
#define CFG_TX_START_NETIF_PER_QUEUE_THRESHOLD  128	/* packets */

#define CHIP_NAME    "MT6632"

#define DRV_NAME "["CHIP_NAME"]: "

/* Define if target platform is Android.
 * It should already be defined in Android kernel source
 */
#ifndef CONFIG_ANDROID
/* #define CONFIG_ANDROID      0 */

#endif

/* for CFG80211 IE buffering mechanism */
#define	CFG_CFG80211_IE_BUF_LEN		(768)
#define	GLUE_INFO_WSCIE_LENGTH		(500)
/* for non-wfa vendor specific IE buffer */
#define NON_WFA_VENDOR_IE_MAX_LEN	(128)


/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

#include "limits.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "string.h"
#include "projdefs.h"
#include "timers.h"
#include "event_groups.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "hal_flash.h"
#include "syslog.h"

#include "version.h"

#include "gl_typedef.h"
#include "typedef.h"
#include "gl_kal.h"
/* #include "gl_rst.h" */
/* #include "hif.h" */

#if CFG_SUPPORT_TDLS
#include "tdls.h"
#endif

#include "debug.h"
#include "syslog.h"

#include "wlan_lib.h"
#include "wlan_oid.h"

#if (CONFIG_WLAN_SERVICE == 1)
#include "agent.h"
#endif

extern uint8_t fgIsBusAccessFailed;
extern struct wireless_dev *gprWdev[KAL_AIS_NUM];

#ifdef CONFIG_MTK_CONNSYS_DEDICATED_LOG_PATH
typedef void (*wifi_fwlog_event_func_cb)(int, int);
/* adaptor ko */
extern int  wifi_fwlog_onoff_status(void);
extern void wifi_fwlog_event_func_register(wifi_fwlog_event_func_cb pfFwlog);
#endif

long KAL_NEED_IMPLEMENT(const char *file, const char *func, int line, ...);
int kal_scnprintf(char *buf, size_t size, const char *fmt, ...);
#if CFG_STATIC_MEM_ALLOC
void dup_completion(struct completion *from, struct completion *to);
#endif
void init_completion(struct completion *comp);
#define wait_for_completion_timeout(_comp, _to) \
	_wait_for_completion_timeout(_comp, _to, __func__)
#define wait_for_completion(_comp) \
	_wait_for_completion(_comp, __func__)
void _wait_for_completion_timeout(struct completion *comp,
	uint32_t to, const char *name);
void _wait_for_completion(struct completion *comp, const char *name);

#define wait_for_completion_interruptible(_comp) wait_for_completion(_comp)
int completion_done(struct completion *comp);
#define complete(_comp) _complete(_comp, __func__)
void _complete(struct completion *comp, const char *name);

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */
#define GLUE_FLAG_HALT                  BIT(0)
#define GLUE_FLAG_INT                   BIT(1)
#define GLUE_FLAG_OID                   BIT(2)
#define GLUE_FLAG_TIMEOUT               BIT(3)
#define GLUE_FLAG_TXREQ                 BIT(4)
#define GLUE_FLAG_SUB_MOD_MULTICAST     BIT(7)
#define GLUE_FLAG_FRAME_FILTER          BIT(8)
#define GLUE_FLAG_FRAME_FILTER_AIS      BIT(9)
#define GLUE_FLAG_HIF_PRT_HIF_DBG_INFO	BIT(16)
#define GLUE_FLAG_UPDATE_WMM_QUOTA		BIT(17)
#define GLUE_FLAG_DELAY_DEAGG			BIT(20)

#define GLUE_FLAG_HALT_BIT              (0)
#define GLUE_FLAG_INT_BIT               (1)
#define GLUE_FLAG_OID_BIT               (2)
#define GLUE_FLAG_TIMEOUT_BIT           (3)
#define GLUE_FLAG_TXREQ_BIT             (4)
#define GLUE_FLAG_SUB_MOD_MULTICAST_BIT (7)
#define GLUE_FLAG_FRAME_FILTER_BIT      (8)
#define GLUE_FLAG_FRAME_FILTER_AIS_BIT  (9)
#define GLUE_FLAG_RX_BIT				(10)
#define GLUE_FLAG_HIF_PRT_HIF_DBG_INFO_BIT	(16)


#if CFG_SUPPORT_MULTITHREAD
#define GLUE_FLAG_RX					BIT(10)
#define GLUE_FLAG_TX_CMD_DONE			BIT(11)
#define GLUE_FLAG_HIF_TX				BIT(12)
#define GLUE_FLAG_HIF_TX_CMD			BIT(13)
#define GLUE_FLAG_RX_TO_OS				BIT(14)
#define GLUE_FLAG_HIF_FW_OWN			BIT(15)

#define GLUE_FLAG_TX_CMD_DONE_BIT			(11)
#define GLUE_FLAG_HIF_TX_BIT				(12)
#define GLUE_FLAG_HIF_TX_CMD_BIT			(13)
#define GLUE_FLAG_RX_TO_OS_BIT				(14)
#define GLUE_FLAG_HIF_FW_OWN_BIT			(15)
#else
/*#if CFG_SUPPORT_RXTHREAD
#define GLUE_FLAG_RX_TO_OS_BIT				(14)
#define GLUE_FLAG_RX_WAIT_BIT (BIT(GLUE_FLAG_RX_TO_OS_BIT) | \
	GLUE_FLAG_HALT | GLUE_FLAG_INT | GLUE_FLAG_DELAY_DEAGG)
#endif
*/
#define GLUE_FLAG_WAIT_BIT (GLUE_FLAG_HALT | GLUE_FLAG_INT | GLUE_FLAG_OID \
			| GLUE_FLAG_TIMEOUT | GLUE_FLAG_TXREQ \
			| GLUE_FLAG_SUB_MOD_MULTICAST | GLUE_FLAG_FRAME_FILTER \
			| GLUE_FLAG_FRAME_FILTER_AIS | GLUE_FLAG_DELAY_DEAGG)
#endif

#define GLUE_FLAG_RST_START BIT(18)
#define GLUE_FLAG_RST_START_BIT 18
#define GLUE_FLAG_RST_END BIT(19)
#define GLUE_FLAG_RST_END_BIT 19

#define GLUE_BOW_KFIFO_DEPTH        (1024)
/* #define GLUE_BOW_DEVICE_NAME        "MT6620 802.11 AMP" */
#define GLUE_BOW_DEVICE_NAME        "ampc0"

#define WAKE_LOCK_RX_TIMEOUT                            300	/* ms */
#define WAKE_LOCK_THREAD_WAKEUP_TIMEOUT                 50	/* ms */

/* FreeRTOS Threading */
#define WF_THREAD_INIT_DONE_BIT		BIT(0)
#define WLAN_THREAD_INIT_BIT		BIT(1)
#if CFG_SUPPORT_RXTHREAD
#define RX_THREAD_INIT_BIT			BIT(2)
#endif

/* If not setting the priority, 0 is the default */
#ifndef WLAN_THREAD_TASK_PRI
#ifdef TCPIP_THREAD_PRIO
#define WLAN_THREAD_TASK_PRI        (TCPIP_THREAD_PRIO)
#else
#define WLAN_THREAD_TASK_PRI        (TASK_PRIORITY_HIGH)
#endif
#endif
#if CFG_SUPPORT_RXTHREAD
#define RX_THREAD_TASK_PRI			(TASK_PRIORITY_NORMAL)
#endif
#define WLAN_INIT_TASK_PRI  (WLAN_THREAD_TASK_PRI)

#if CFG_STATIC_MEM_ALLOC
#define WLAN_INIT_STACK_SIZE (1000) /* unit in word */
#else
#define WLAN_INIT_STACK_SIZE (configMINIMAL_STACK_SIZE + 10) /* unit in word */
#endif
#define WLAN_THREAD_STACK_SIZE		(800)	/* unit in word */
#define WLAN_WAIT_LOCK_TIME			(5000/portTICK_PERIOD_MS)
/* if not get semaphor, get into suspend right away */
#define WLAN_TIMER_EXPIRE_REQ		(2000)
#define WLAN_TIMER_SET_BLOCKTIME	(100)
#define LWIP_STA_MODE		(0)
#define LWIP_AP_MODE		(1)
#if CFG_SUPPORT_SNIFFER
#define LWIP_SNIFFER_MODE	(2)
#endif

extern SemaphoreHandle_t g_halt_sem;
extern EventGroupHandle_t g_init_wait;
typedef err_t (*wlan_netif_input_fn)(struct netif *inp, struct pbuf *p);

#define scnprintf  kal_scnprintf
#define pr_info(fmt, ...) LOG_I(WIFI, fmt"\r\t", ##__VA_ARGS__)
#define do_gettimeofday(_tv) KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)

#define kal_sched_clock() KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)
#define sched_clock() kalGetTimeTick()

#define get_random_bytes(_buf, _nbytes) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)

#define kal_test_bit(bit, pr) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)
#ifndef test_bit
#define test_bit(_offset, _val) kal_test_bit(_offset, _val)
#endif

#define kal_spin_lock_bh(prlock) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)
#define spin_lock_bh(_lock) xSemaphoreTake(*_lock, (TickType_t) WLAN_WAIT_LOCK_TIME)

#define kal_spin_unlock_bh(prlock) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)
#define spin_unlock_bh(_lock) xSemaphoreGive(*_lock)

/* usleep_range(u4MinUSec, u4MaxUSec) */
#define kalUsleep_range(_u4MinUSec, _u4MaxUSec)  \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)

#define mdelay(_ms) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)

#ifndef readl
#define readl(_addr) READ_REG(_addr)
#endif
#ifndef writel
#define writel(_val, _addr) WRITE_REG(_val, _addr)
#endif

#if CFG_WLAN_MEM_CACHE_ALIGN
#define MEM_BUF_RESERVE_LEN	HAL_CACHE_LINE_SIZE
#else
#define MEM_BUF_RESERVE_LEN	8	/* HIF DMA */
#endif
#define GET_INTERNAL_BUF_RESV_LEN()	MEM_BUF_RESERVE_LEN

/* EFUSE Auto Mode Support */
#define LOAD_EFUSE 0
#define LOAD_EEPROM_BIN 1
#define LOAD_AUTO 2

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

struct GLUE_INFO;

struct GL_WPA_INFO {
	uint32_t u4WpaVersion;
	uint32_t u4KeyMgmt;
	uint32_t u4CipherGroup;
	uint32_t u4CipherPairwise;
	uint32_t u4AuthAlg;
	uint8_t fgPrivacyInvoke;
#if CFG_SUPPORT_802_11W
	uint32_t u4CipherGroupMgmt;
	uint32_t u4Mfp;
	uint8_t ucRSNMfpCap;
#endif
	uint8_t ucRsneLen;
	uint8_t aucKek[NL80211_KEK_LEN];
	uint8_t aucKck[NL80211_KCK_LEN];
	uint8_t aucReplayCtr[NL80211_REPLAY_CTR_LEN];
};

#if CFG_SUPPORT_REPLAY_DETECTION
struct GL_REPLEY_PN_INFO {
	uint8_t auPN[16];
	uint8_t fgRekey;
	uint8_t fgFirstPkt;
};
struct GL_DETECT_REPLAY_INFO {
	uint8_t ucCurKeyId;
	uint8_t ucKeyType;
	struct GL_REPLEY_PN_INFO arReplayPNInfo[4];
};
#endif

enum ENUM_NET_DEV_IDX {
	NET_DEV_WLAN_IDX = 0,
	NET_DEV_P2P_IDX,
	NET_DEV_BOW_IDX,
	NET_DEV_NUM
};

enum ENUM_RSSI_TRIGGER_TYPE {
	ENUM_RSSI_TRIGGER_NONE,
	ENUM_RSSI_TRIGGER_GREATER,
	ENUM_RSSI_TRIGGER_LESS,
	ENUM_RSSI_TRIGGER_TRIGGERED,
	ENUM_RSSI_TRIGGER_NUM
};

#if CFG_ENABLE_WIFI_DIRECT
enum ENUM_NET_REG_STATE {
	ENUM_NET_REG_STATE_UNREGISTERED,
	ENUM_NET_REG_STATE_REGISTERING,
	ENUM_NET_REG_STATE_REGISTERED,
	ENUM_NET_REG_STATE_UNREGISTERING,
	ENUM_NET_REG_STATE_NUM
};
#endif

enum ENUM_PKT_FLAG {
	ENUM_PKT_802_11,	/* 802.11 or non-802.11 */
	ENUM_PKT_802_3,		/* 802.3 or ethernetII */
	ENUM_PKT_1X,		/* 1x frame or not */
	ENUM_PKT_PROTECTED_1X,	/* protected 1x frame */
	ENUM_PKT_NON_PROTECTED_1X,	/* Non protected 1x frame */
	ENUM_PKT_VLAN_EXIST,	/* VLAN tag exist */
	ENUM_PKT_DHCP,		/* DHCP frame */
	ENUM_PKT_ARP,		/* ARP */
	ENUM_PKT_ICMP,		/* ICMP */
	ENUM_PKT_TDLS,		/* TDLS */
	ENUM_PKT_DNS,		/* DNS */

	ENUM_PKT_FLAG_NUM
};

enum ENUM_WLAN_DRV_BUF_TYPE_T {
	ENUM_BUF_TYPE_NVRAM,
	ENUM_BUF_TYPE_DRV_CFG,
	ENUM_BUF_TYPE_FW_CFG,
	ENUM_BUF_TYPE_NUM
};

struct GL_IO_REQ {
	struct QUE_ENTRY rQueEntry;
	/* wait_queue_head_t       cmdwait_q; */
	uint8_t fgRead;
	uint8_t fgWaitResp;
	struct ADAPTER *prAdapter;
	PFN_OID_HANDLER_FUNC pfnOidHandler;
	void *pvInfoBuf;
	uint32_t u4InfoBufLen;
	uint32_t *pu4QryInfoLen;
	uint32_t rStatus;
	uint32_t u4Flag;
	uint8_t ucBssIndex;
};

#if CFG_SUPPORT_PERF_IND
	struct GL_PERF_IND_INFO {
		uint32_t u4CurTxBytes[BSSID_NUM]; /* Byte */
		uint32_t u4CurRxBytes[BSSID_NUM]; /* Byte */
		uint16_t u2CurRxRate[BSSID_NUM]; /* Unit 500 Kbps */
		uint8_t ucCurRxRCPI0[BSSID_NUM];
		uint8_t ucCurRxRCPI1[BSSID_NUM];
		uint8_t ucCurRxNss[BSSID_NUM];
	};
#endif /* CFG_SUPPORT_SCAN_CACHE_RESULT */

struct FT_IES {
	uint16_t u2MDID;
	struct IE_MOBILITY_DOMAIN *prMDIE;
	struct IE_FAST_TRANSITION *prFTIE;
	struct IE_TIMEOUT_INTERVAL *prTIE;
	struct RSN_INFO_ELEM *prRsnIE;
	uint8_t *pucIEBuf;
	uint32_t u4IeLength;
};

/*
 * needed by mgmt/auth.c
 * struct cfg80211_ft_event - FT Information Elements
 * @ies: FT IEs
 * @ies_len: length of the FT IE in bytes
 * @target_ap: target AP's MAC address
 * @ric_ies: RIC IE
 * @ric_ies_len: length of the RIC IE in bytes
 */
struct cfg80211_ft_event_params {
	const u8 *ies;
	size_t ies_len;
	const u8 *target_ap;
	const u8 *ric_ies;
	size_t ric_ies_len;
};

/*
 * needed by common/wlan_oid.c
 * struct cfg80211_update_ft_ies_params - FT IE Information
 * This structure provides information needed to update the fast transition IE
 *
 * @md: The Mobility Domain ID, 2 Octet value
 * @ie: Fast Transition IEs
 * @ie_len: Length of ft_ie in octets
 */
struct cfg80211_update_ft_ies_params {
	u16 md;
	const u8 *ie;
	size_t ie_len;
};

struct net_device {
	struct netif *netif;
	wlan_netif_input_fn netif_rxcb;
	uint8_t bss_idx;
	struct GLUE_INFO *gl_info;
};

/*
 * type definition of pointer to p2p structure
 */
struct GL_P2P_INFO;	/* declare GL_P2P_INFO_T */
struct GL_P2P_DEV_INFO;	/* declare GL_P2P_DEV_INFO_T */

/* sizeof(section) * num_of_sec */
#define DL_REGION_INFO_SIZE (20 * 3)

struct GLUE_INFO {
	/* Device handle */
	struct net_device *prDevHandler;
	/* change spinlock to semaphor to sync power save mechanism */
	SemaphoreHandle_t rSpinLock[SPIN_LOCK_NUM];
	/* semaphore for ioctl */
	SemaphoreHandle_t ioctl_sem;
	SemaphoreHandle_t TxStop_sem;

	uint64_t u8Cookie;

	unsigned long ulFlag;		/* GLUE_FLAG_XXX */
	uint32_t u4PendFlag;
	/* UINT_32 u4TimeoutFlag; */
	uint32_t u4OidCompleteFlag;
	uint32_t u4ReadyFlag;	/* check if card is ready */

	uint32_t u4OsMgmtFrameFilter;

	/* Number of pending frames, also used for debuging if any frame is
	 * missing during the process of unloading Driver.
	 *
	 * NOTE(Kevin): In Linux, we also use this variable as the threshold
	 * for manipulating the netif_stop(wake)_queue() func.
	 */
	int32_t ai4TxPendingFrameNumPerQueue[MAX_BSSID_NUM][CFG_MAX_TXQ_NUM];
	int32_t i4TxPendingFrameNum;
	int32_t i4TxPendingSecurityFrameNum;
	int32_t i4TxPendingCmdNum;

	/* Tx: for NetDev to BSS index mapping */
	struct NET_INTERFACE_INFO arNetInterfaceInfo[MAX_BSSID_NUM];

	/* Rx: for BSS index to NetDev mapping */
	/* P_NET_INTERFACE_INFO_T  aprBssIdxToNetInterfaceInfo[HW_BSSID_NUM]; */

	/* current IO request for kalIoctl */
	struct GL_IO_REQ OidEntry;

	/* registry info */
	struct REG_INFO rRegInfo;

	/* firmware header wrapper by multipatch header */
	struct patch_dl_target rfw_info;
#if CFG_STATIC_MEM_ALLOC
	/* struct patch_dl_buf *patch_region */
	uint8_t fw_region_buf[DL_REGION_INFO_SIZE];
#endif

	/* Host interface related information */
	/* defined in related hif header file */
	struct GL_HIF_INFO rHifInfo;

	/*! \brief wext wpa related information */
	struct GL_WPA_INFO rWpaInfo[KAL_AIS_NUM];
#if CFG_SUPPORT_REPLAY_DETECTION
	struct GL_DETECT_REPLAY_INFO prDetRplyInfo[KAL_AIS_NUM];
#endif

	/* Pointer to ADAPTER_T - main data structure of internal protocol
	 * stack
	 */
	struct ADAPTER *prAdapter;

	/* Indicated media state */
	enum ENUM_PARAM_MEDIA_STATE eParamMediaStateIndicated[KAL_AIS_NUM];

	/* Device power state D0~D3 */
	enum PARAM_DEVICE_POWER_STATE ePowerState;
	/* indicate scan complete */
	struct completion rScanComp;
	/* indicate main thread halt complete */
	struct completion rHaltComp;
	/* indicate main thread halt complete */
	struct completion rPendComp;
#if CFG_SUPPORT_MULTITHREAD
	/* indicate hif_thread halt complete */
	struct completion rHifHaltComp;
	/* indicate hif_thread halt complete */
	struct completion rRxHaltComp;

	uint32_t u4TxThreadPid;
	uint32_t u4RxThreadPid;
	uint32_t u4HifThreadPid;
#endif

#if CFG_SUPPORT_NCHO
	/* indicate Ais channel grant complete */
	struct completion rAisChGrntComp;	
#endif

	uint32_t rPendStatus;

	struct QUE rTxQueue;

	/* OID related */
	struct QUE rCmdQueue;
	/* PVOID                   pvInformationBuffer; */
	/* UINT_32                 u4InformationBufferLength; */
	/* PVOID                   pvOidEntry; */
	/* Puint8_t                 pucIOReqBuff; */
	/* QUE_T                   rIOReqQueue; */
	/* QUE_T                   rFreeIOReqQueue; */

	TaskHandle_t main_thread;
	EventGroupHandle_t event_main_thread;

	TaskHandle_t wlan_reset_thread_main;

	TaskHandle_t rx_thread;
	EventGroupHandle_t rx_event;
#if CFG_SUPPORT_MULTITHREAD
	wait_queue_head_t waitq_hif;
	struct task_struct *hif_thread;
#endif

	TimerHandle_t tickfn;


#if CFG_SUPPORT_EXT_CONFIG
	uint16_t au2ExtCfg[256];	/* NVRAM data buffer */
	uint32_t u4ExtCfgLength;	/* 0 means data is NOT valid */
#endif

#if CFG_ENABLE_BT_OVER_WIFI
	struct GL_BOW_INFO rBowInfo;
#endif

#if CFG_ENABLE_WIFI_DIRECT
	struct GL_P2P_DEV_INFO *prP2PDevInfo;
	struct GL_P2P_INFO *prP2PInfo[KAL_P2P_NUM];
#endif

	/* NVRAM availability */
	uint8_t fgNvramAvailable;

	uint8_t fgMcrAccessAllowed;

	/* MAC Address Overridden by IOCTL */
	uint8_t fgIsMacAddrOverride;
	uint8_t rMacAddrOverride[PARAM_MAC_ADDR_LEN];

	struct SET_TXPWR_CTRL rTxPwr;

	/* for cfg80211 scan done indication */
	 struct PARAM_SCAN_REQUEST_ADV *prScanRequest;

#if CFG_SUPPORT_SCHED_SCAN
	struct PARAM_SCHED_SCAN_REQUEST *prSchedScanRequest;
#endif

	/* to indicate registered or not */
	uint8_t fgIsRegistered;

#if CFG_SUPPORT_SDIO_READ_WRITE_PATTERN
	uint8_t fgEnSdioTestPattern;
	uint8_t fgSdioReadWriteMode;
	uint8_t fgIsSdioTestInitialized;
	uint8_t aucSdioTestBuffer[256];
#endif

	uint8_t fgIsInSuspendMode;

#if (CFG_SUPPORT_SUPPLICANT_MBO == 1)
	uint8_t aucSupOpClassIE[200];    /*for Assoc req */
	uint16_t u2SupOpClassIELen;
	uint8_t aucMboIE[200];    /*for Assoc req */
	uint16_t u2MboIELen;
#endif

#if CFG_SUPPORT_PASSPOINT
	uint8_t fgIsDad;
	uint8_t aucDADipv4[4];
	uint8_t fgIs6Dad;
	uint8_t aucDADipv6[16];
#endif				/* CFG_SUPPORT_PASSPOINT */

	KAL_WAKE_LOCK_T rIntrWakeLock;
	KAL_WAKE_LOCK_T rTimeoutWakeLock;

#if CFG_MET_PACKET_TRACE_SUPPORT
	uint8_t fgMetProfilingEn;
	uint16_t u2MetUdpPort;
#endif

#if CFG_SUPPORT_SNIFFER
	uint8_t fgIsEnableMon;
	struct net_device *prMonDevHandler;
#endif

	int32_t i4RssiCache;
	uint32_t u4LinkSpeedCache;


	uint32_t u4InfType;

	uint32_t IsrCnt;
	uint32_t IsrPassCnt;
	uint32_t TaskIsrCnt;

	uint32_t IsrAbnormalCnt;
	uint32_t IsrSoftWareCnt;
	uint32_t IsrTxCnt;
	uint32_t IsrRxCnt;
	uint64_t u8HifIntTime;

	/* save partial scan channel information */
	/* PARTIAL_SCAN_INFO rScanChannelInfo; */
	uint8_t *pucScanChannel;

#if CFG_SUPPORT_SCAN_CACHE_RESULT
	struct GL_SCAN_CACHE_INFO scanCache;
#endif /* CFG_SUPPORT_SCAN_CACHE_RESULT */
#if (CFG_SUPPORT_PERF_IND == 1)
	struct GL_PERF_IND_INFO PerfIndCache;
#endif

	/* Full2Partial */
	OS_SYSTIME u4LastFullScanTime;
	/* full scan or partial scan */
	uint8_t ucTrScanType;
	/* uint8_t aucChannelNum[FULL_SCAN_MAX_CHANNEL_NUM]; */
	/* PARTIAL_SCAN_INFO rFullScanApChannel; */
	uint8_t *pucFullScan2PartialChannel;

	uint32_t u4RoamFailCnt;
	uint64_t u8RoamFailTime;
	uint8_t fgTxDoneDelayIsARP;
	uint32_t u4ArriveDrvTick;
	uint32_t u4EnQueTick;
	uint32_t u4DeQueTick;
	uint32_t u4LeaveDrvTick;
	uint32_t u4CurrTick;
	uint64_t u8CurrTime;

	/* FW Roaming */
	/* store the FW roaming enable state which FWK determines */
	/* if it's = 0, ignore the black/whitelists settings from FWK */
	uint32_t u4FWRoamingEnable;

	/*service for test mode*/
#if (CONFIG_WLAN_SERVICE == 1)
	struct service rService;
#endif
};
extern struct GLUE_INFO *g_prGlueInfo;
extern struct ADAPTER *g_prAdpater;
#define netdev_priv() (g_prGlueInfo)
#define GLUE_CHK_DEV(_pr) \
	((_pr) ? TRUE : FALSE)

#define GLUE_CHK_PR2(_pr, pr2) \
	((GLUE_CHK_DEV(_pr) && pr2) ? TRUE : FALSE)

#define GLUE_CHK_PR3(_pr, pr2, pr3) \
	((GLUE_CHK_PR2(_pr, pr2) && pr3) ? TRUE : FALSE)

#define GLUE_CHK_PR4(_pr, pr2, pr3, pr4) \
	((GLUE_CHK_PR3(_pr, pr2, pr3) && pr4) ? TRUE : FALSE)

/* generic sub module init/exit handler
 *   now, we only have one sub module, p2p
 */
#if CFG_ENABLE_WIFI_DIRECT
typedef uint8_t(*SUB_MODULE_INIT) (struct GLUE_INFO
				    *prGlueInfo);
typedef uint8_t(*SUB_MODULE_EXIT) (struct GLUE_INFO
				    *prGlueInfo);

struct SUB_MODULE_HANDLER {
	SUB_MODULE_INIT subModInit;
	SUB_MODULE_EXIT subModExit;
	uint8_t fgIsInited;
};

#endif

#ifdef CONFIG_NL80211_TESTMODE

enum TestModeCmdType {
	TESTMODE_CMD_ID_SW_CMD = 1,
	TESTMODE_CMD_ID_WAPI = 2,
	TESTMODE_CMD_ID_HS20 = 3,

	/* Hotspot managerment testmode command */
	TESTMODE_CMD_ID_HS_CONFIG = 51,

	TESTMODE_CMD_ID_STR_CMD = 102,
	NUM_OF_TESTMODE_CMD_ID
};

#if CFG_SUPPORT_PASSPOINT
enum Hs20CmdType {
	HS20_CMD_ID_SET_BSSID_POOL = 0,
	NUM_OF_HS20_CMD_ID
};
#endif /* CFG_SUPPORT_PASSPOINT */

struct NL80211_DRIVER_TEST_MODE_PARAMS {
	uint32_t index;
	uint32_t buflen;
};

/*SW CMD */
struct NL80211_DRIVER_SW_CMD_PARAMS {
	struct NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	uint8_t set;
	uint32_t adr;
	uint32_t data;
};

struct iw_encode_exts {
	__u32 ext_flags;	/*!< IW_ENCODE_EXT_* */
	__u8 tx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*!< LSB first */
	__u8 rx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*!< LSB first */
	/*!< ff:ff:ff:ff:ff:ff for broadcast/multicast
	 *   (group) keys or unicast address for
	 *   individual keys
	 */
	__u8 addr[MAC_ADDR_LEN];
	__u16 alg;		/*!< IW_ENCODE_ALG_* */
	__u16 key_len;
	__u8 key[32];
};

/*SET KEY EXT */
struct NL80211_DRIVER_SET_KEY_EXTS {
	struct NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	uint8_t key_index;
	uint8_t key_len;
	struct iw_encode_exts ext;
};

#if CFG_SUPPORT_PASSPOINT

struct param_hs20_set_bssid_pool {
	uint8_t fgBssidPoolIsEnable;
	uint8_t ucNumBssidPool;
	uint8_t arBssidPool[8][ETH_ALEN];
};

struct wpa_driver_hs20_data_s {
	struct NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	enum Hs20CmdType CmdType;
	struct param_hs20_set_bssid_pool hs20_set_bssid_pool;
};

#endif /* CFG_SUPPORT_PASSPOINT */

#endif

struct NETDEV_PRIVATE_GLUE_INFO {
	struct GLUE_INFO *prGlueInfo;
	uint8_t ucBssIdx;
#if CFG_ENABLE_UNIFY_WIPHY
	uint8_t ucIsP2p;
#endif
};

struct PACKET_PRIVATE_DATA {
	/* tx/rx both use cb */
	struct QUE_ENTRY rQueEntry;  /* 16byte total:16 */

	uint8_t ucBssIdx;	/* 1byte */
	/* only rx use cb */
	uint8_t fgIsIndependentPkt; /* 1byte */
	/* only tx use cb */
	uint8_t ucTid;		/* 1byte */
	uint8_t ucHeaderLen;	/* 1byte */
	uint8_t ucProfilingFlag;	/* 1byte */
	uint8_t ucSeqNo;		/* 1byte */
	uint16_t u2Flag;		/* 2byte total:24 */

	uint16_t u2IpId;		/* 2byte */
	uint16_t u2FrameLen;	/* 2byte */
	OS_SYSTIME rArrivalTime;/* 4byte total:32 */

	uint64_t u8ArriveTime;	/* 8byte total:40 */
};

struct PACKET_PRIVATE_RX_DATA {
	uint64_t u8IntTime;	/* 8byte */
	uint64_t u8RxTime;	/* 8byte */
};
/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */
#if (CFG_SUPPORT_FW_BUILDIN)
/* use array from $CHIP_fw.c under chip folder */
extern uint8_t uacFWImage[];
extern unsigned int u4LenFWImage;
#endif
#if (CFG_SUPPORT_BUFFER_MODE) && (CFG_SUPPORT_BUFFER_MODE_HDR == 1)
/* use array from $CHIP_eeprom.c under chip folder */
extern uint8_t uacEEPROMImage[];
extern unsigned int u4LenEEPROMImage;
#endif
/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */
/*----------------------------------------------------------------------------*/
/* Macros of SPIN LOCK operations for using in Glue Layer                     */
/*----------------------------------------------------------------------------*/
#define GLUE_SPIN_LOCK_DECLARATION()
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
			DBGLOG(INIT, LOUD, "SPIN_LOCK[%u] Try to acquire as mutex\n", rLockCategory); \
			while (pdTRUE != xSemaphoreTake(prGlueInfo->rSpinLock[rLockCategory], (TickType_t) WLAN_WAIT_LOCK_TIME))\
			DBGLOG(INIT, LOUD, "SPIN_LOCK[%u] Acquired as mutex\n", rLockCategory);	\
	    }
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
			xSemaphoreGive(prGlueInfo->rSpinLock[rLockCategory]); \
	    }

/*----------------------------------------------------------------------------*/
/* Macros for accessing Reserved Fields of native packet                      */
/*----------------------------------------------------------------------------*/

#define GLUE_GET_PKT_PRIVATE_DATA(_p) \
	((struct PACKET_PRIVATE_DATA *)(&(((struct pkt_buf *)(_p))->cb[0])))

#define GLUE_GET_PKT_QUEUE_ENTRY(_p)    \
	    (&(GLUE_GET_PKT_PRIVATE_DATA(_p)->rQueEntry))

#define GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)  \
	    ((void *) (((unsigned long)_prQueueEntry) \
	    - offsetof(struct pkt_buf, cb)))

#define GLUE_SET_PKT_TID(_p, _tid) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucTid = (uint8_t)(_tid))

#define GLUE_GET_PKT_TID(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucTid)

#define GLUE_SET_PKT_FLAG(_p, _flag) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u2Flag |= BIT(_flag))

#define GLUE_TEST_PKT_FLAG(_p, _flag) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u2Flag & BIT(_flag))

#define GLUE_IS_PKT_FLAG_SET(_p) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u2Flag)

#define GLUE_SET_PKT_BSS_IDX(_p, _ucBssIndex) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucBssIdx = (uint8_t)(_ucBssIndex))

#define GLUE_GET_PKT_BSS_IDX(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucBssIdx)

#define GLUE_SET_PKT_HEADER_LEN(_p, _ucMacHeaderLen) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucHeaderLen = \
	    (uint8_t)(_ucMacHeaderLen))

#define GLUE_GET_PKT_HEADER_LEN(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucHeaderLen)

#define GLUE_SET_PKT_FRAME_LEN(_p, _u2PayloadLen) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u2FrameLen = (uint16_t)(_u2PayloadLen))

#define GLUE_GET_PKT_FRAME_LEN(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->u2FrameLen)

#define GLUE_SET_PKT_ARRIVAL_TIME(_p, _rSysTime) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->rArrivalTime = (OS_SYSTIME)(_rSysTime))

#define GLUE_GET_PKT_ARRIVAL_TIME(_p)    \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->rArrivalTime)

#define GLUE_SET_PKT_IP_ID(_p, _u2IpId) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u2IpId = (uint16_t)(_u2IpId))

#define GLUE_GET_PKT_IP_ID(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->u2IpId)

#define GLUE_SET_PKT_SEQ_NO(_p, _ucSeqNo) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->ucSeqNo = (uint8_t)(_ucSeqNo))

#define GLUE_GET_PKT_SEQ_NO(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucSeqNo)

#define GLUE_SET_PKT_FLAG_PROF_MET(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucProfilingFlag |= BIT(0))

#define GLUE_GET_PKT_IS_PROF_MET(_p) \
	    (GLUE_GET_PKT_PRIVATE_DATA(_p)->ucProfilingFlag & BIT(0))

#define GLUE_SET_PKT_XTIME(_p, _rSysTime) \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u8ArriveTime = (uint64_t)(_rSysTime))

#define GLUE_GET_PKT_XTIME(_p)    \
	(GLUE_GET_PKT_PRIVATE_DATA(_p)->u8ArriveTime)


#define GLUE_GET_PKT_PRIVATE_RX_DATA(_p) \
	((struct PACKET_PRIVATE_RX_DATA *)(&(((struct pkt_buf *)(_p))->cb[24])))

#define GLUE_RX_SET_PKT_INT_TIME(_p, _rTime) \
	(GLUE_GET_PKT_PRIVATE_RX_DATA(_p)->u8IntTime = (uint64_t)(_rTime))

#define GLUE_RX_SET_PKT_RX_TIME(_p, _rTime) \
	(GLUE_GET_PKT_PRIVATE_RX_DATA(_p)->u8RxTime = (uint64_t)(_rTime))

#if 0 /* no device at freertos */
/* Check validity of prDev, private data, and pointers */
#define GLUE_CHK_DEV(prDev) \
	((prDev && *((struct GLUE_INFO **) netdev_priv(prDev))) ? TRUE : FALSE)

#define GLUE_CHK_PR2(prDev, pr2) \
	((GLUE_CHK_DEV(prDev) && pr2) ? TRUE : FALSE)

#define GLUE_CHK_PR3(prDev, pr2, pr3) \
	((GLUE_CHK_PR2(prDev, pr2) && pr3) ? TRUE : FALSE)

#define GLUE_CHK_PR4(prDev, pr2, pr3, pr4) \
	((GLUE_CHK_PR3(prDev, pr2, pr3) && pr4) ? TRUE : FALSE)
#endif

#define GLUE_SET_EVENT(pr) \
	kalSetEvent(pr)

#define GLUE_INC_REF_CNT(_refCount)	\
	do {\
	taskENTER_CRITICAL();\
	_refCount++;\
	taskEXIT_CRITICAL();\
	} while (0)
#define GLUE_DEC_REF_CNT(_refCount) \
	do {\
	taskENTER_CRITICAL();\
	_refCount--;\
	taskEXIT_CRITICAL();\
	} while (0)
#define GLUE_GET_REF_CNT(_refCount)	_refCount


#define DbgPrint(...)

#if CFG_MET_TAG_SUPPORT
#define GL_MET_TAG_START(_id, _name)	met_tag_start(_id, _name)
#define GL_MET_TAG_END(_id, _name)	met_tag_end(_id, _name)
#define GL_MET_TAG_ONESHOT(_id, _name, _value) \
	met_tag_oneshot(_id, _name, _value)
#define GL_MET_TAG_DISABLE(_id)		met_tag_disable(_id)
#define GL_MET_TAG_ENABLE(_id)		met_tag_enable(_id)
#define GL_MET_TAG_REC_ON()		met_tag_record_on()
#define GL_MET_TAG_REC_OFF()		met_tag_record_off()
#define GL_MET_TAG_INIT()		met_tag_init()
#define GL_MET_TAG_UNINIT()		met_tag_uninit()
#else
#define GL_MET_TAG_START(_id, _name)
#define GL_MET_TAG_END(_id, _name)
#define GL_MET_TAG_ONESHOT(_id, _name, _value)
#define GL_MET_TAG_DISABLE(_id)
#define GL_MET_TAG_ENABLE(_id)
#define GL_MET_TAG_REC_ON()
#define GL_MET_TAG_REC_OFF()
#define GL_MET_TAG_INIT()
#define GL_MET_TAG_UNINIT()
#endif

#define MET_TAG_ID	0


/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */
#if CFG_ENABLE_BT_OVER_WIFI
uint8_t glRegisterAmpc(struct GLUE_INFO *prGlueInfo);

uint8_t glUnregisterAmpc(struct GLUE_INFO *prGlueInfo);
#endif

#if CFG_ENABLE_WIFI_DIRECT
void p2pSetMulticastListWorkQueueWrapper(struct GLUE_INFO
		*prGlueInfo);
#endif

struct GLUE_INFO *wlanGetGlueInfo(void);

void wlanDebugInit(void);

uint32_t wlanSetDriverDbgLevel(IN uint32_t u4DbgIdx,
			       IN uint32_t u4DbgMask);

uint32_t wlanGetDriverDbgLevel(IN uint32_t u4DbgIdx,
			       OUT uint32_t *pu4DbgMask);

void wlanSetSuspendMode(struct GLUE_INFO *prGlueInfo,
			uint8_t fgEnable);

void wlanGetConfig(struct ADAPTER *prAdapter);

uint32_t wlanDownloadBufferBin(struct ADAPTER *prAdapter);

uint32_t wlanConnacDownloadBufferBin(struct ADAPTER
				     *prAdapter);

uint32_t wlanConnac2XDownloadBufferBin(struct ADAPTER
					*prAdapter);
/*******************************************************************************
 *			 E X T E R N A L   F U N C T I O N S / V A R I A B L E
 *******************************************************************************
 */
extern struct net_device *gPrP2pDev[KAL_P2P_NUM];
extern struct net_device *gPrDev;

#ifdef CFG_DRIVER_INF_NAME_CHANGE
extern char *gprifnameap;
extern char *gprifnamep2p;
extern char *gprifnamesta;
#endif /* CFG_DRIVER_INF_NAME_CHANGE */

extern void wlanRegisterNotifier(void);
extern void wlanUnregisterNotifier(void);

#if CFG_MET_PACKET_TRACE_SUPPORT
void kalMetTagPacket(IN struct GLUE_INFO *prGlueInfo,
		     IN void *prPacket, IN enum ENUM_TX_PROFILING_TAG eTag);

void kalMetInit(IN struct GLUE_INFO *prGlueInfo);
#endif

void wlanUpdateChannelTable(struct GLUE_INFO *prGlueInfo);

#if CFG_SUPPORT_SAP_DFS_CHANNEL
void wlanUpdateDfsChannelTable(struct GLUE_INFO *prGlueInfo,
		uint8_t ucRoleIdx, uint8_t ucChannel, uint8_t ucBandWidth,
		enum ENUM_CHNL_EXT eBssSCO, uint32_t u4CenterFreq);
#endif

#if CFG_ENABLE_UNIFY_WIPHY
const struct net_device_ops *wlanGetNdevOps(void);
#endif

int wlanHardStartXmit(struct pbuf *p, struct netif *netif, struct GLUE_INFO *prGlueInfo);

/*
typedef uint8_t (*file_buf_handler) (void *ctx,
			const char __user *buf,
			uint16_t length);
extern void register_file_buf_handler(file_buf_handler handler,
			void *ctx,
			uint8_t ucType);
*/
#if CFG_SUPPORT_CUSTOM_NETLINK
extern void glCustomGenlInit(void);
extern void glCustomGenlDeinit(void);
#endif

#endif /* _GL_OS_H */
