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

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__


/* Flash layout on MPS2 AN521 with BL2 (multiple image boot):
 *
 * 0x0000_0000 BL2 - MCUBoot (0.5 MB)
 * 0x0008_0000 Secure image     primary slot (0.5 MB)
 * 0x0010_0000 Non-secure image primary slot (0.5 MB)
 * 0x0018_0000 Secure image     secondary slot (0.5 MB)
 * 0x0020_0000 Non-secure image secondary slot (0.5 MB)
 * 0x0028_0000 Scratch area (0.5 MB)
 * 0x0030_0000 Secure Storage Area (20 KB)
 * 0x0030_5000 Internal Trusted Storage Area (16 KB)
 * 0x0030_9000 NV counters area (4 KB)
 * 0x0030_A000 Unused (984 KB)
 *
 * Flash layout on MPS2 AN521 with BL2 (single image boot):
 *
 * 0x0000_0000 BL2 - MCUBoot (0.5 MB)
 * 0x0008_0000 Primary image area (1 MB):
 *    0x0008_0000 Secure     image primary
 *    0x0010_0000 Non-secure image primary
 * 0x0018_0000 Secondary image area (1 MB):
 *    0x0018_0000 Secure     image secondary
 *    0x0020_0000 Non-secure image secondary
 * 0x0028_0000 Scratch area (1 MB)
 * 0x0038_0000 Secure Storage Area (20 KB)
 * 0x0038_5000 Internal Trusted Storage Area (16 KB)
 * 0x0038_9000 NV counters area (4 KB)
 * 0x0038_A000 Unused (472 KB)
 *
 * Flash layout on MPS2 AN521, if BL2 not defined:
 *
 * 0x0000_0000 Secure     image (1 MB)
 * 0x0010_0000 Non-secure image (1 MB)
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Size of TFM internal secure storage partition  */
#define FLASH_S_INTERNAL_PARTITION_SIZE (0xA000) /* Internal flash partition: 40 KB, FLASH_SST_AREA_SIZE(20KB) + FLASH_ITS_AREA_SIZE(16KB) + TFM_NV_COUNTERS_SECTOR_SIZE(4KB) */

/* Size of a Secure and of a Non-secure image */
#ifdef TFM_NORMAL_BUILD
#define FLASH_S_PARTITION_SIZE          (0x60000 - FLASH_S_INTERNAL_PARTITION_SIZE) /* S partition: 384 - 40 = 344 KB */
#else
#define FLASH_S_PARTITION_SIZE          (0x46000 - FLASH_S_INTERNAL_PARTITION_SIZE) /* S partition: 280 - 40 = 260 KB */
#endif /* ifdef TFM_NORMAL_BUILD */
#define FLASH_NS_PARTITION_SIZE         (0x7A0000) /* NS partition: 8MB - 0x60000 */
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_S_PARTITION_SIZE >   \
                                          FLASH_NS_PARTITION_SIZE) ? \
                                         FLASH_S_PARTITION_SIZE :    \
                                         FLASH_NS_PARTITION_SIZE)

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)     /* 4 KB */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00400000) /* 4 MB */

/* Flash layout info for BL2 bootloader */
/* Same as FLASH0_BASE_S */
#define FLASH_BASE_ADDRESS              (0x90000000)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */
#define FLASH_AREA_BL2_OFFSET      (0x0)
#define FLASH_AREA_BL2_SIZE        (0x12000) /* 72 KB */

#if !defined(MCUBOOT_IMAGE_NUMBER) || (MCUBOOT_IMAGE_NUMBER == 1)
/* Secure + Non-secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE)
/* Secure + Non-secure secondary slot */
#define FLASH_AREA_2_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE)
/* Scratch area */
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE)
/* The maximum number of status entries supported by the bootloader. */
#define MCUBOOT_STATUS_MAX_ENTRIES ((FLASH_S_PARTITION_SIZE + \
                                     FLASH_NS_PARTITION_SIZE) / \
                                    FLASH_AREA_SCRATCH_SIZE)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    ((FLASH_S_PARTITION_SIZE + \
                                     FLASH_NS_PARTITION_SIZE) / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)
#elif (MCUBOOT_IMAGE_NUMBER == 2)
/* Secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image primary slot */
#define FLASH_AREA_1_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_1_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Secure image secondary slot */
#define FLASH_AREA_2_ID            (FLASH_AREA_1_ID + 1)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image secondary slot */
#define FLASH_AREA_3_ID            (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_3_OFFSET        (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Scratch area */
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_3_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (FLASH_MAX_PARTITION_SIZE)
/* The maximum number of status entries supported by the bootloader. */
#define MCUBOOT_STATUS_MAX_ENTRIES (FLASH_MAX_PARTITION_SIZE / \
                                    FLASH_AREA_SCRATCH_SIZE)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    (FLASH_MAX_PARTITION_SIZE / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)
#else /* MCUBOOT_IMAGE_NUMBER > 2 */
#error "Only MCUBOOT_IMAGE_NUMBER 1 and 2 are supported!"
#endif /* MCUBOOT_IMAGE_NUMBER */

/* Use SRAM1 memory to store Code data */
#define S_ROM_ALIAS_BASE  (0x18000000)
#define NS_ROM_ALIAS_BASE (0x18000000)
#define S_ROM_BASE  (0x90000000)
#define NS_ROM_BASE (0x90000000)

#define S_SYSRAM_BASE       (0x08000000)
#define S_SYSRAM_TFM_OFFSET (0x00013000)
#define S_SYSRAM_SIZE       (0xA000)

/* FIXME: Use SRAM2 memory to store RW data */
#define S_RAM_ALIAS_BASE  S_SYSRAM_BASE
#define NS_RAM_ALIAS_BASE (0x10000000)

#define TOTAL_ROM_SIZE FLASH_TOTAL_SIZE

#define TOTAL_PSRAM_SIZE (0x400000)     /* 4 MB */

#define TOTAL_SYSRAM_SIZE (0x100000)     /* 1 MB */

#define S_TCM_SEC_SHM_BASE  (0x10B000)
#define S_TCM_SEC_SHM_SIZE  (0x1000)

#define S_TCM_BASE  (0x10C000)
#define S_TCM_SIZE  (0xA000)
#define S_TCM_STACK_SIZE  (0x400)

#endif /* __FLASH_LAYOUT_H__ */
