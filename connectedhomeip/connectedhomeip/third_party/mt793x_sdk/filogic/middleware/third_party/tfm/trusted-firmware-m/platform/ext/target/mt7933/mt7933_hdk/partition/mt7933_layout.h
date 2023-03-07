/*
 * Copyright (c) 2017-2019 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MT7933_LAYOUT_H__
#define __MT7933_LAYOUT_H__

#include "region_defs.h"


/* Internal Trusted Storage (ITS) Service definitions */
#define FLASH_ITS_AREA_OFFSET           FLASH_AREA_BL2_SIZE
#define FLASH_ITS_AREA_SIZE             (0x4000)   /* 16 KB */

/* Secure Storage (SST) Service definitions */
#define FLASH_SST_AREA_OFFSET           (FLASH_ITS_AREA_OFFSET + FLASH_ITS_AREA_SIZE)
#define FLASH_SST_AREA_SIZE             (0x5000)   /* 20 KB */

/* NV Counters definitions */
#define FLASH_NV_COUNTERS_AREA_OFFSET   (FLASH_SST_AREA_OFFSET + FLASH_SST_AREA_SIZE)
#define FLASH_NV_COUNTERS_AREA_SIZE     (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* Offset and size definition in flash area used by assemble.py */
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
        SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME

/* Secure Storage (SST) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M SST Integration Guide.
 */
#define SST_FLASH_DEV_NAME TFM_Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define SST_FLASH_AREA_ADDR     FLASH_SST_AREA_OFFSET
/* Dedicated flash area for SST */
#define SST_FLASH_AREA_SIZE     FLASH_SST_AREA_SIZE
#define SST_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of SST_SECTOR_SIZE per block */
#define SST_SECTORS_PER_BLOCK   (0x1)
/* Specifies the smallest flash programmable unit in bytes */
#define SST_FLASH_PROGRAM_UNIT  (0x1)
/* The maximum asset size to be stored in the SST area */
#define SST_MAX_ASSET_SIZE      (1280)
/* The maximum number of assets to be stored in the SST area */
#define SST_NUM_ASSETS          (10)

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide. The ITS should be in the internal flash, but is
 * allocated in the external flash just for development platforms that don't
 * have internal flash available.
 */
#define ITS_FLASH_DEV_NAME TFM_Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define ITS_FLASH_AREA_ADDR     FLASH_ITS_AREA_OFFSET
/* Dedicated flash area for ITS */
#define ITS_FLASH_AREA_SIZE     FLASH_ITS_AREA_SIZE
#define ITS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of ITS_SECTOR_SIZE per block */
#define ITS_SECTORS_PER_BLOCK   (0x1)
/* Specifies the smallest flash programmable unit in bytes */
#define ITS_FLASH_PROGRAM_UNIT  (0x1)
/* The maximum asset size to be stored in the ITS area */
#define ITS_MAX_ASSET_SIZE      (1280)
/* The maximum number of assets to be stored in the ITS area */
#define ITS_NUM_ASSETS          (10)

/* NV Counters definitions */
#define TFM_NV_COUNTERS_AREA_ADDR    FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_AREA_SIZE    (0x18) /* 24 Bytes */
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_SECTOR_SIZE  FLASH_AREA_IMAGE_SECTOR_SIZE

/* Flash partition layout definitions */
#define FLASH_BOOTLOADER_REGION_START       (PHY_FLASH_MASK(XIP_BL_START))
#define FLASH_BOOTLOADER_REGION_LEN         (XIP_BL_LENGTH)
#define FLASH_RESERVE1_REGION_START_ADDR    (FLASH_BOOTLOADER_REGION_START + FLASH_BOOTLOADER_REGION_LEN)
#define FLASH_TFM_REGION_START              (PHY_FLASH_MASK(XIP_TFM_INT_START))
#define FLASH_RTOS_REGION_START             (PHY_FLASH_MASK(XIP_RTOS_START))
#define FLASH_RTOS_REGION_LEN               (XIP_RTOS_LENGTH)
#ifndef MTK_AUDIO_SUPPORT
#define FLASH_CUSTOMER_REGION_START         (FLASH_RTOS_REGION_START + FLASH_RTOS_REGION_LEN + 0x80)
#define FLASH_FOTA_REGION_START             (PHY_FLASH_MASK(XIP_FOTA_START))
#define FLASH_NVDM_REGION_START             (PHY_FLASH_MASK(XIP_NVDM_START))
#ifdef TFM_NORMAL_BUILD
#define FLASH_LOG_REGION_START              (PHY_FLASH_MASK(XIP_LOG_START))
#endif /* #ifdef TFM_NORMAL_BUILD */
#define FLASH_BT_REGION_START               (PHY_FLASH_MASK(XIP_BT_START))
#define FLASH_BT_REGION_LEN                 (XIP_BT_LENGTH)
#define FLASH_RESERVE2_REGION_START_ADDR    (FLASH_BT_REGION_START + FLASH_BT_REGION_LEN)
#define FLASH_WIFI_PWRTBL_REGION_LEN        (PHY_FLASH_MASK(XIP_WIFI_PWRTBL_START))
#define FLASH_WIFI_EXT_REGION_LEN           (PHY_FLASH_MASK(XIP_WIFI_EXT_START))
#define FLASH_BUFFER_BIN_REGION_LEN         (PHY_FLASH_MASK(XIP_BUFFER_BIN_START))
#else
#define FLASH_FOTA_REGION_START             (PHY_FLASH_MASK(XIP_FOTA_START))
#define FLASH_FOTA_REGION_LEN               (XIP_FOTA_LENGTH)
#define FLASH_CUSTOMER_REGION_START         (FLASH_FOTA_REGION_START + FLASH_FOTA_REGION_LEN)
#define FLASH_NVDM_REGION_START             (PHY_FLASH_MASK(XIP_NVDM_START))
#ifdef TFM_NORMAL_BUILD
#define FLASH_LOG_REGION_START              (PHY_FLASH_MASK(XIP_LOG_START))
#endif /* #ifdef TFM_NORMAL_BUILD */
#define FLASH_DSP_REGION_START              (PHY_FLASH_MASK(XIP_DSP_START))
#define FLASH_BT_REGION_START               (PHY_FLASH_MASK(XIP_BT_START))
#define FLASH_BT_REGION_LEN                 (XIP_BT_LENGTH)
#define FLASH_RESERVE2_REGION_START_ADDR    (FLASH_BT_REGION_START + FLASH_BT_REGION_LEN)
#define FLASH_WIFI_PWRTBL_REGION_LEN        (PHY_FLASH_MASK(XIP_WIFI_PWRTBL_START))
#define FLASH_WIFI_EXT_REGION_LEN           (PHY_FLASH_MASK(XIP_WIFI_EXT_START))
#define FLASH_BUFFER_BIN_REGION_LEN         (PHY_FLASH_MASK(XIP_BUFFER_BIN_START))
#endif

/* SYSRAM partition layout definitions */
#define SYSRAM_HW_CR_PATCH_E2_REGION_START     (SYSRAM_BASE)
#define SYSRAM_BOOTLOADER_E2_REGION_START      (SYSRAM_BL_BASE)
#define SYSRAM_TFM_E2_REGION_START             (SYSRAM_TFM_BASE)
#define SYSRAM_SYS_E2_REGION_START             (SYSRAM_SYS_BASE)

/* PSRAM partition layout definitions */
#ifndef MTK_AUDIO_SUPPORT
#define PSRAM_WIFI_REGION_START             (RAM_WIFI_FW_BASE)
#define PSRAM_OS_HEAP_REGION_START          (RAM_BASE)
#define PSRAM_OS_HEAP_REGION_LEN            (RAM_LENGTH)
#define PSRAM_RESERVE1_REGION_START         (PSRAM_OS_HEAP_REGION_START + PSRAM_OS_HEAP_REGION_LEN)
#else
#define PSRAM_BT_REGION_START               (RAM_BT_FW_BASE)
#define PSRAM_DSP_REGION_START              (RAM_DSP_BASE)
#define PSRAM_AUDIO_REGION_START            (RAM_AUDIO_BASE)
#define PSRAM_AUDIO_REGION_LEN              (RAM_AUDIO_LENGTH)
#define PSRAM_RESERVE1_REGION_START         (PSRAM_AUDIO_REGION_START + PSRAM_AUDIO_REGION_LEN)
#define PSRAM_WIFI_REGION_START             (RAM_WIFI_FW_BASE)
#define PSRAM_OS_HEAP_REGION_START          (RAM_BASE)
#endif

/* TCM partition layout definitions */
#define TCM_CACHE_E2_REGION_START               (TCM_CACHE_BASE)
#define TCM_RTOS_E2_REGION_START                (TCM_SYS_BASE)
#define TCM_SEC_SHM_REGION_START                (TCM_SEC_SHM_BASE)
#define TCM_TFM_REGION_START                    (TCM_TFM_BASE)
#define TCM_TFM_REGION_LEN                      (TCM_TFM_LENGTH)
#define TCM_RESERVE1_REGION_START               (TCM_TFM_REGION_START + TCM_TFM_REGION_LEN)
#define TCM_BL_E2_REGION_START                  (TCM_BL_BASE)

#endif /* __MT7933_LAYOUT_H__ */
