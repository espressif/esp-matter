/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
/**
 * @file minicli_cmd_table.h
 *
 * This file contains a list of all CLI commands that are globally availabe
 * to each project. If a project enables the corresponding feature and turns
 * on it, then the CLI command will be turned on in the list of table in this
 * file. Two sets of commands are listed at the end of this file, namely:
 *
 * NORMAL mode - MINICLI_NORMAL_MODE_CLI_CMDS
 * TEST mode - MINICLI_TEST_MODE_CLI_CMDS
 *
 * Normal and test are enabled by default. TEST mode commands can be turned
 * off by setting MTK_CLI_TEST_MODE_ENABLE to 'n' in feature.mk.
 */

#if defined(MTK_MINICLI_ENABLE)


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#ifdef MTK_CLI_FORK_ENABLE
/**
 * FOTA_CLI_ENTRY
 */
#include "cli_cli.h"
#else /* #ifdef MTK_CLI_FORK_ENABLE */
#define CLI_CLI_FORK_ENTRY
#endif /* #ifdef MTK_CLI_FORK_ENABLE */


#ifdef MTK_FOTA_ENABLE
/**
 * FOTA_CLI_ENTRY
 */
#include "fota_cli.h"
#else /* #ifdef MTK_FOTA_ENABLE */
#define FOTA_CLI_ENTRY
#endif /* #ifdef MTK_FOTA_ENABLE */


#ifdef MTK_FOTA_V3_ENABLE
/**
 * FOTA_V3_CLI_ENTRY
 */
#include "v3/fota_cli.h"
#else /* #ifdef MTK_FOTA_V3_ENABLE */
#define FOTA_V3_CLI_ENTRY
#endif /* #ifdef MTK_FOTA_V3_ENABLE */

/**
 * EFUSE_DRV_CLI_ENTRY
  */
#ifdef MTK_HAL_EFUSE_CLI_ENABLE
#include "efuse_cli.h"
#else /* #ifdef MTK_HAL_EFUSE_CLI_ENABLE */
#define EFUSE_DRV_CLI_ENTRY
#endif /* #ifdef MTK_HAL_EFUSE_CLI_ENABLE */

/**
 * MTK_BOOT_CLI_ENABLE
  */
#ifdef MTK_BOOT_CLI_ENABLE
#include "boot_cli.h"
#else /* #ifdef MTK_BOOT_CLI_ENABLE */
#define BOOT_CLI_ENTRY
#endif /* #ifdef MTK_BOOT_CLI_ENABLE */

#if 0
/**
 * INBAND_CLI_ENTRY
 */
#include "wifi_inband_cli.h"
#include "wifi_api.h"
#include "wifi_cli.h"
#include "wifi_private_cli.h"
#endif /* #if 0 */

#if 0
/**
 * WIFI_CONNSYS_CLI_ENTRY
 */
#include "connsys_util.h"
#endif /* #if 0 */

#ifdef MTK_LWIP_ENABLE
/**
 * IP_CLI_ENTRY
 */
#include "lwip_cli.h"
#else /* #ifdef MTK_LWIP_ENABLE */
#define IP_CLI_ENTRY
#endif /* #ifdef MTK_LWIP_ENABLE */


#ifdef MTK_IPERF_ENABLE
/**
 * IPERF_CLI_ENTRY
 */
#include "iperf_cli.h"
#else /* #ifdef MTK_IPERF_ENABLE */
#define IPERF_CLI_ENTRY
#endif /* #ifdef MTK_IPERF_ENABLE */


/**
 * IPERF_CLI_SPI_TPUT_ENTRY.
 */
#ifdef MTK_SPI_THROUGHPUT
#include "iperf_test.h"
#else /* #ifdef MTK_SPI_THROUGHPUT */
#define IPERF_CLI_SPI_TPUT_ENTRY
#endif /* #ifdef MTK_SPI_THROUGHPUT */

/**
 * SLP_MGR_CLI_ENTRY
 */
#if ((PRODUCT_VERSION == 7687)||(PRODUCT_VERSION == 7697))
#include "lp_ex_sleep.h"
#elif ((PRODUCT_VERSION == 7686)||(PRODUCT_VERSION == 7682)||(PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7933))
#include "sleep_manager_cli.h"
#endif /* #if ((PRODUCT_VERSION == 7687)||(PRODUCT_VERSION == 7697)) */

/**
 * MINISUPP_CLI_ENTRY
 */
#ifdef MTK_MINISUPP_ENABLE
#include "minisupp_cli.h"
#else /* #ifdef MTK_MINISUPP_ENABLE */
#define MINISUPP_CLI_ENTRY
#endif /* #ifdef MTK_MINISUPP_ENABLE */

/**
 * SIGMA_CLI_ENTRY
 */
#ifdef MTK_SIGMA_ENABLE
#include "sigma_cli.h"
#else /* #ifdef MTK_SIGMA_ENABLE */
#define SIGMA_CLI_ENTRY
#endif /* #ifdef MTK_SIGMA_ENABLE */
/**
 * MOD_HAP_CLI_ENTRY
 */
#ifdef MTK_HOMEKIT_ENABLE
#include "homekit_cli.h"
#else /* #ifdef MTK_HOMEKIT_ENABLE */
#define HOMEKIT_CLI_ENTRY
#endif /* #ifdef MTK_HOMEKIT_ENABLE */


#ifdef MTK_WIFI_ROM_ENABLE
#include "wifi_firmware_cli.h"
#else /* #ifdef MTK_WIFI_ROM_ENABLE */
#define MLME_CLI_ENTRY
#endif /* #ifdef MTK_WIFI_ROM_ENABLE */


#ifdef MTK_MDNS_ENABLE
/**
 * MOD_MDNS_CLI_ENTRY
 */
#include "mdns_cli.h"
#else /* #ifdef MTK_MDNS_ENABLE */
#define MOD_MDNS_CLI_ENTRY
#endif /* #ifdef MTK_MDNS_ENABLE */


/**
 * MPERF_CLI_ENTRY
 */
#ifdef MTK_MPERF_ENABLE
#include "mperf.h"
#else /* #ifdef MTK_MPERF_ENABLE */
#define MPERF_CLI_ENTRY
#endif /* #ifdef MTK_MPERF_ENABLE */


/*
 * MULTI_CLI_ENTRY
 */
#if defined(MTK_JOYLINK_ENABLE)   || defined(MTK_ALINK_ENABLE) || defined(MTK_AIRKISS_ENABLE)
#include "msc_cli.h"
#else /* #if defined(MTK_JOYLINK_ENABLE)   || defined(MTK_ALINK_ENABLE) || defined(MTK_AIRKISS_ENABLE) */
#define MULTI_CLI_ENTRY
#endif /* #if defined(MTK_JOYLINK_ENABLE)   || defined(MTK_ALINK_ENABLE) || defined(MTK_AIRKISS_ENABLE) */

/*
 * AWS_CLI_ENTRY
 */
#if defined(MTK_AWS_ENABLE)
#include "alink_cli.h"
#else /* #if defined(MTK_AWS_ENABLE) */
#define AWS_CLI_ENTRY
#endif /* #if defined(MTK_AWS_ENABLE) */

/*
 * ALINK_CLI_ENTRY
 */
#if defined(MTK_ALINK_SDS_CLI_ENABLE)
#include "alink_sds_cli.h"
#else /* #if defined(MTK_ALINK_SDS_CLI_ENABLE) */
#define ALINK_CLI_ENTRY
#endif /* #if defined(MTK_ALINK_SDS_CLI_ENABLE) */


#ifdef MTK_NVDM_ENABLE
/**
 * NVDM_CLI_ENTRY
 */
#include "nvdm_cli.h"
#else /* #ifdef MTK_NVDM_ENABLE */
#define NVDM_CLI_ENTRY
#endif /* #ifdef MTK_NVDM_ENABLE */


#ifdef MTK_OS_CLI_ENABLE
/**
 * OS_CLI_ENTRY
 */
#include "os_cli.h"
#else /* #ifdef MTK_OS_CLI_ENABLE */
#define OS_CLI_ENTRY
#endif /* #ifdef MTK_OS_CLI_ENABLE */


#ifdef MTK_PING_OUT_ENABLE
/**
 * PING_CLI_ENTRY
 */
#include "ping_cli.h"
#else /* #ifdef MTK_PING_OUT_ENABLE */
#define PING_CLI_ENTRY
#endif /* #ifdef MTK_PING_OUT_ENABLE */


#ifdef MTK_TFM_CLI_ENABLE
/**
 * TFM_CLI_ENTRY
 */
#include "tfm_cli.h"
#else /* #ifdef MTK_TFM_CLI_ENABLE */
#define TFM_CLI_ENTRY
#endif /* #ifdef MTK_TFM_CLI_ENABLE */


/**
 * REBOOT_CLI_ENTRY
 */


/**
 * SDK_CLI_ENTRY
 */


/**
 * TEST_REG_CLI_ENTRY
 */
#include "board_cli.h"


#ifdef MTK_BSPEXT_ENABLE
/**
 * SIE_CLI_ENTRY
 */
#include "sie_cli.h"
#else /* #ifdef MTK_BSPEXT_ENABLE */
#define SIE_CLI_ENTRY
#endif /* #ifdef MTK_BSPEXT_ENABLE */


/**
 * SYSLOG_CLI_ENTRY
 */
#include "syslog_cli.h"


#ifdef MTK_BLE_CLI_ENABLE
/**
 * BLE_CLI_ENTRY
 */
#include "bt_cli.h"
#else /* #ifdef MTK_BLE_CLI_ENABLE */
#define BLE_CLI_ENTRY
#endif /* #ifdef MTK_BLE_CLI_ENABLE */


#ifdef MTK_BLE_BQB_CLI_ENABLE
/**
 * BLE_BQB_CLI_ENTRY
 */
#include "bt_cli.h"
#else /* #ifdef MTK_BLE_BQB_CLI_ENABLE */
#define BLE_BQB_CLI_ENTRY
#endif /* #ifdef MTK_BLE_BQB_CLI_ENABLE */


#ifdef MTK_BT_MESH_CLI_ENABLE
/**
 * BLE_MESH_ENTRY
 */
#include "bt_mesh_cli.h"
#else /* #ifdef MTK_BT_MESH_CLI_ENABLE */
#define BLE_MESH_ENTRY
#endif /* #ifdef MTK_BT_MESH_CLI_ENABLE */


#ifdef MTK_GPIO_CLI_ENABLE
/**
 * GPIO_CLI_ENTRY
 */
#include "gpio_cli.h"
#else /* #ifdef MTK_GPIO_CLI_ENABLE */
#define GPIO_CLI_ENTRY
#endif /* #ifdef MTK_GPIO_CLI_ENABLE */


#ifdef MTK_THERMAL_CLI_ENABLE
/**
 * THERMAL_CLI_ENTRY
 */
#include "thermal_cli.h"
#else /* #ifdef MTK_THERMAL_CLI_ENABLE */
#define THERMAL_CLI_ENTRY
#endif /* #ifdef MTK_THERMAL_CLI_ENABLE */


#ifdef MTK_BT_DRV_CLI_ENABLE
/**
 * BT_DRV_CLI_ENTRY
 */
#include "bt_driver_cli.h"
#else /* #ifdef MTK_BT_DRV_CLI_ENABLE */
#define BT_DRV_CLI_ENTRY
#endif /* #ifdef MTK_BT_DRV_CLI_ENABLE */


#ifdef MTK_WF_DRV_CLI_ENABLE
/**
 * WF_DRV_CLI_ENTRY
 */
#include "gl_wifi_cli.h"
#else /* #ifdef MTK_WF_DRV_CLI_ENABLE */
#define WF_DRV_CLI_ENTRY
#endif /* #ifdef MTK_WF_DRV_CLI_ENABLE */


#ifdef MTK_BT_BOOTS_CLI_ENABLE
/**
 * BTCMD_CLI_ENTRY
 */
#include "bt_boots_cli.h"
#else /* #ifdef MTK_BT_BOOTS_CLI_ENABLE */
#define BTCMD_CLI_ENTRY
#endif /* #ifdef MTK_BT_BOOTS_CLI_ENABLE */


#ifdef MTK_BT_PICUS_ENABLE
/**
 * BT_PICUS_CLI_ENTRY
 */
#include "bt_picus_cli.h"
#else /* #ifdef MTK_BT_PICUS_ENABLE */
#define BT_PICUS_CLI_ENTRY
#endif /* #ifdef MTK_BT_PICUS_ENABLE */


#ifdef MTK_LP_DVT_CLI_ENABLE
/**
 * LP_DVT_CLI_CMDS
 */
#include "lp_dvt_cli.h"
#else /* #ifdef MTK_LP_DVT_CLI_ENABLE */
#define LP_DVT_CLI_ENTRY
#endif /* #ifdef MTK_LP_DVT_CLI_ENABLE */

/**
 * BSP_DVT_CLI
 */
#ifdef MTK_BSP_DVT_CLI_ENABLE
#include "dvt_cli.h"
#else /* #ifdef MTK_BSP_DVT_CLI_ENABLE */
#define BSP_DVT_CLI_CMDS
#endif /* #ifdef MTK_BSP_DVT_CLI_ENABLE */

/**
 * ATED_CLI_ENTRY
 */
#ifdef MTK_ATED_ENABLE
#include "ated_init.h"
#else /* #ifdef MTK_ATED_ENABLE */
#define ATED_CLI_ENTRY
#endif /* #ifdef MTK_ATED_ENABLE */
/**
 * WIFITEST_CLI_ENTRY
 */
#ifdef MTK_WIFI_TEST_TOOL_ENABLE
#include "wifitest_cli.h"
#else /* #ifdef MTK_WIFI_TEST_TOOL_ENABLE */
#define WIFITEST_CLI_ENTRY
#endif /* #ifdef MTK_WIFI_TEST_TOOL_ENABLE */

/**
 * PKCS11_TEST_CLI_ENTRY
 */
#if defined(MTK_CORE_PKCS11_ENABLE) && defined(MTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE)
#include "pkcs11_system_test_cli.h"
#else /* #if defined(MTK_CORE_PKCS11_ENABLE) && defined(MTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE) */
#define PKCS11_TEST_CLI_ENTRY
#endif /* #if defined(MTK_CORE_PKCS11_ENABLE) && defined(MTK_CORE_PKCS11_SYSTEM_TESTS_ENABLE) */

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)

#define MINICLI_TEST_MODE_CLI_CMDS      INBAND_CLI_ENTRY         \
                                        SIE_CLI_ENTRY            \
                                        MINISUPP_CLI_ENTRY       \
                                        WIFI_CONNSYS_CLI_ENTRY   \
                                        MOD_MDNS_CLI_ENTRY       \
                                        HOMEKIT_CLI_ENTRY        \
                                        IP_CLI_ENTRY             \
                                        CLI_CLI_FORK_ENTRY       \
                                        WIFI_PRIV_CLI_ENTRY      \
                                        TEST_REG_CLI_ENTRY       \
                                        SLP_MGR_CLI_ENTRY        \
                                        OS_CLI_ENTRY             \
                                        REBOOT_CLI_ENTRY         \
                                        MPERF_CLI_ENTRY          \
                                        N9_LOG_CLI_ENTRY

#elif (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)


#define MINICLI_TEST_MODE_CLI_CMDS      INBAND_CLI_ENTRY         \
                                        SIE_CLI_ENTRY            \
                                        MINISUPP_CLI_ENTRY       \
                                        WIFI_CONNSYS_CLI_ENTRY   \
                                        MOD_MDNS_CLI_ENTRY       \
                                        HOMEKIT_CLI_ENTRY        \
                                        IP_CLI_ENTRY             \
                                        CLI_CLI_FORK_ENTRY       \
                                        WIFI_PRIV_CLI_ENTRY      \
                                        TEST_REG_CLI_ENTRY       \
                                        SLP_MGR_CLI_ENTRY        \
                                        OS_CLI_ENTRY             \
                                        REBOOT_CLI_ENTRY         \
                                        MPERF_CLI_ENTRY          \
                                        N9_LOG_CLI_ENTRY

#elif (PRODUCT_VERSION == 7933)

#define MINICLI_TEST_MODE_CLI_CMDS      TEST_REG_CLI_ENTRY       \
                                        OS_CLI_ENTRY             \
                                        REBOOT_CLI_ENTRY         \
                                        BLE_CLI_ENTRY            \
                                        BT_DRV_CLI_ENTRY         \
                                        BTCMD_CLI_ENTRY          \
                                        BT_PICUS_CLI_ENTRY       \
                                        WF_DRV_CLI_ENTRY         \
                                        SLP_MGR_CLI_ENTRY        \
                                        LP_DVT_CLI_ENTRY         \
                                        IPERF_CLI_ENTRY          \
                                        ATED_CLI_ENTRY           \
                                        IP_CLI_ENTRY             \
                                        BOOT_CLI_ENTRY           \
                                        WIFITEST_CLI_ENTRY       \
                                        MINISUPP_CLI_ENTRY       \
                                        SIGMA_CLI_ENTRY          \
                                        BSP_DVT_CLI_CMDS         \
                                        MEM_DEBUG_CLI_ENTRY
#else /* #if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) */

#define MINICLI_TEST_MODE_CLI_CMDS      TEST_REG_CLI_ENTRY

#endif /* #if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) */


#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)

#define MINICLI_NORMAL_MODE_CLI_CMDS    FOTA_CLI_ENTRY           \
                                        IP_CLI_ENTRY             \
                                        PING_CLI_ENTRY           \
                                        IPERF_CLI_ENTRY          \
                                        IPERF_CLI_SPI_TPUT_ENTRY \
                                        NVDM_CLI_ENTRY           \
                                        WIFI_PUB_CLI_ENTRY       \
                                        WIFI_PUB_CLI_SMNT_ENTRY  \
                                        AWS_CLI_ENTRY            \
                                        ALINK_CLI_ENTRY          \
                                        MULTI_CLI_ENTRY          \
                                        BLE_CLI_ENTRY            \
                                        BLE_MESH_ENTRY           \
                                        BLE_BQB_CLI_ENTRY        \
                                        SYSLOG_CLI_ENTRY         \
                                        GPIO_CLI_ENTRY           \
                                        SDK_CLI_ENTRY

#elif (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)

#define MINICLI_NORMAL_MODE_CLI_CMDS    FOTA_CLI_ENTRY           \
                                        IP_CLI_ENTRY             \
                                        PING_CLI_ENTRY           \
                                        IPERF_CLI_ENTRY          \
                                        IPERF_CLI_SPI_TPUT_ENTRY \
                                        NVDM_CLI_ENTRY           \
                                        WIFI_PUB_CLI_ENTRY       \
                                        WIFI_PUB_CLI_SMNT_ENTRY  \
                                        AWS_CLI_ENTRY            \
                                        ALINK_CLI_ENTRY          \
                                        MULTI_CLI_ENTRY          \
                                        BLE_CLI_ENTRY            \
                                        BLE_BQB_CLI_ENTRY        \
                                        SYSLOG_CLI_ENTRY         \
                                        SDK_CLI_ENTRY            \
                                        MLME_CLI_ENTRY
#elif (PRODUCT_VERSION == 7933)

#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define MINICLI_NORMAL_MODE_CLI_CMDS    SDK_CLI_ENTRY            \
                                        SYSLOG_CLI_ENTRY         \
                                        NVDM_CLI_ENTRY           \
                                        TFM_CLI_ENTRY            \
                                        BLE_CLI_ENTRY            \
                                        BLE_MESH_ENTRY           \
                                        BT_DRV_CLI_ENTRY         \
                                        BTCMD_CLI_ENTRY          \
                                        BT_PICUS_CLI_ENTRY       \
                                        WF_DRV_CLI_ENTRY         \
                                        PING_CLI_ENTRY           \
                                        IPERF_CLI_ENTRY          \
                                        ATED_CLI_ENTRY           \
                                        IP_CLI_ENTRY             \
                                        FOTA_V3_CLI_ENTRY        \
                                        WIFITEST_CLI_ENTRY       \
                                        EFUSE_DRV_CLI_ENTRY      \
                                        MINISUPP_CLI_ENTRY       \
                                        THERMAL_CLI_ENTRY        \
                                        SIGMA_CLI_ENTRY          \
                                        PKCS11_TEST_CLI_ENTRY
#else
#define MINICLI_NORMAL_MODE_CLI_CMDS    SDK_CLI_ENTRY            \
                                        SYSLOG_CLI_ENTRY         \
                                        NVDM_CLI_ENTRY           \
                                        TFM_CLI_ENTRY            \
                                        BLE_CLI_ENTRY            \
                                        BLE_MESH_ENTRY           \
                                        BT_DRV_CLI_ENTRY         \
                                        BTCMD_CLI_ENTRY          \
                                        BT_PICUS_CLI_ENTRY       \
                                        WF_DRV_CLI_ENTRY         \
                                        PING_CLI_ENTRY           \
                                        IPERF_CLI_ENTRY          \
                                        ATED_CLI_ENTRY           \
                                        FOTA_V3_CLI_ENTRY        \
                                        WIFITEST_CLI_ENTRY       \
                                        EFUSE_DRV_CLI_ENTRY      \
                                        THERMAL_CLI_ENTRY        \
                                        SIGMA_CLI_ENTRY          \
                                        PKCS11_TEST_CLI_ENTRY
#endif
#else /* #if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) */

#define MINICLI_NORMAL_MODE_CLI_CMDS    IP_CLI_ENTRY            \
                                        PING_CLI_ENTRY          \
                                        OS_CLI_ENTRY

#endif /* #if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) */

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if defined(MTK_MINICLI_ENABLE) */

