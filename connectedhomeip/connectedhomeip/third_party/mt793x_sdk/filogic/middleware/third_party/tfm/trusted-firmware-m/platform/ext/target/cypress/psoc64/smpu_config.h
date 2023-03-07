/*
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
 * Copyright (c) 2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
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

#ifndef __SMPU_CONFIG_H__
#define __SMPU_CONFIG_H__

#include "flash_layout.h"
#include "region_defs.h"

#include "cy_prot.h"

/* This macro depends on the actual CY_PROT_REGIONSIZE_XXX values */
#define REGIONSIZE_TO_BYTES(X)      (1UL << (1 + (X)))

/* The actual SMPU configs */

/* SMPU configs can only be changed by privileged secure PC=1 bus masters */
#define COMMON_SMPU_MASTER_CONFIG {\
    .userPermission = CY_PROT_PERM_R, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = ONLY_BL2_SPM_MASK, \
}

/* SMPU0 - secure primary image in Flash */
#define SMPU0_BASE          S_ROM_ALIAS(SECURE_IMAGE_OFFSET)
#define SMPU0_REGIONSIZE    PROT_SIZE_512KB_BIT_SHIFT
#define SMPU0_SUBREGION_DIS (CY_PROT_SUBREGION_DIS5 | \
                             CY_PROT_SUBREGION_DIS6 | \
                             CY_PROT_SUBREGION_DIS7)
#define SMPU0_SLAVE_CONFIG {\
    .address = (void *)SMPU0_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU0_REGIONSIZE, \
    .subregions = SMPU0_SUBREGION_DIS, \
    .userPermission = CY_PROT_PERM_RWX, \
    .privPermission = CY_PROT_PERM_RWX, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU0_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU0_BASE % REGIONSIZE_TO_BYTES(SMPU0_REGIONSIZE)
#error "Flash layout has changed - SMPU0 needs updating"
#endif

/*
 * SMPU0 protected area should be exactly the size of the secure primary image
 */
#if FLASH_S_PARTITION_SIZE != (5 * REGIONSIZE_TO_BYTES(SMPU0_REGIONSIZE)/8)
#error "Flash layout has changed - FLASH_S_PARTITION_SIZE isn't 5/8 of SMPU0_REGIONSIZE"
#endif

/* SMPU1 - Internal Trusted Storage in Flash */
#define SMPU1_BASE         S_ROM_ALIAS(FLASH_ITS_AREA_OFFSET)
#define SMPU1_REGIONSIZE   PROT_SIZE_16KB_BIT_SHIFT
#define SMPU1_SLAVE_CONFIG {\
    .address = (void *)SMPU1_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU1_REGIONSIZE, \
    .subregions = ALL_ENABLED, \
    .userPermission = CY_PROT_PERM_RW, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU1_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU1_BASE % REGIONSIZE_TO_BYTES(SMPU1_REGIONSIZE)
#error "Flash layout has changed - SMPU1 needs updating"
#endif

/* Should exactly cover the ITS region */
#if FLASH_ITS_AREA_SIZE != REGIONSIZE_TO_BYTES(SMPU1_REGIONSIZE)
#error "Flash layout has changed - SMPU1_REGIONSIZE isn't FLASH_ITS_AREA_SIZE"
#endif

/* SMPU2 - NV counters in Flash */
#define SMPU2_BASE         S_ROM_ALIAS(FLASH_NV_COUNTERS_AREA_OFFSET)
#define SMPU2_REGIONSIZE   PROT_SIZE_512B_BIT_SHIFT
#define SMPU2_SLAVE_CONFIG {\
    .address = (void *)SMPU2_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU2_REGIONSIZE, \
    .subregions = ALL_ENABLED, \
    .userPermission = CY_PROT_PERM_RW, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU2_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU2_BASE % REGIONSIZE_TO_BYTES(SMPU2_REGIONSIZE)
#error "Flash layout has changed - SMPU2 needs updating"
#endif

/* Should exactly cover the NV Counters region */
#if FLASH_NV_COUNTERS_AREA_SIZE != REGIONSIZE_TO_BYTES(SMPU2_REGIONSIZE)
#error "Flash layout has changed - SMPU2_REGIONSIZE isn't FLASH_NV_COUNTERS_AREA_SIZE"
#endif

/* SMPU3 - SST in Flash */
#define SMPU3_BASE         S_ROM_ALIAS(0x1c8000)
#define SMPU3_REGIONSIZE   PROT_SIZE_32KB_BIT_SHIFT
#define SMPU3_SUBREGION_DIS (CY_PROT_SUBREGION_DIS0 | \
                             CY_PROT_SUBREGION_DIS1 | \
                             CY_PROT_SUBREGION_DIS2)
#define SMPU3_SLAVE_CONFIG {\
    .address = (void *)SMPU3_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU3_REGIONSIZE, \
    .subregions = SMPU3_SUBREGION_DIS, \
    .userPermission = CY_PROT_PERM_RW, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU3_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU3_BASE % REGIONSIZE_TO_BYTES(SMPU3_REGIONSIZE)
#error "Flash layout has changed - SMPU3 needs updating"
#endif

/*
 * SST_FLASH_AREA_ADDR must equal the base address of subregion 3 of
 * SMPU3
 */
#if S_ROM_ALIAS(SST_FLASH_AREA_ADDR) != (SMPU3_BASE + \
                          (3 * REGIONSIZE_TO_BYTES(SMPU3_REGIONSIZE) / 8))
#error "Flash layout has changed - S_DATA_PRIV_START isn't subregion 3 of SMPU3"
#endif

/* Should exactly cover the SST region */
#if FLASH_SST_AREA_SIZE != (5 * REGIONSIZE_TO_BYTES(SMPU3_REGIONSIZE) / 8)
#error "Flash layout has changed - SMPU3_REGIONSIZE isn't FLASH_SST_AREA_SIZE"
#endif

/* SMPU6 - 32KB of unprivileged secure data in SRAM */
#define SMPU6_BASE         S_DATA_START
#define SMPU6_REGIONSIZE   PROT_SIZE_32KB_BIT_SHIFT
#define SMPU6_SLAVE_CONFIG {\
    .address = (void *)SMPU6_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU6_REGIONSIZE, \
    .subregions = ALL_ENABLED, \
    .userPermission = CY_PROT_PERM_RW, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU6_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU6_BASE % REGIONSIZE_TO_BYTES(SMPU6_REGIONSIZE)
#error "Flash layout has changed - SMPU6 needs updating"
#endif

/* SMPU6 should exactly cover the unprivileged secure SRAM */
#if REGIONSIZE_TO_BYTES(SMPU6_REGIONSIZE) != S_UNPRIV_DATA_SIZE
#error "SMPU6_REGIONSIZE should match S_UNPRIV_DATA_SIZE"
#endif

/* SMPUs 7 and 10 - 160KB of privileged secure data at S_DATA_PRIV_START in SRAM */
#define SMPU7_BASE          S_RAM_ALIAS(0)
#define SMPU7_REGIONSIZE    PROT_SIZE_128KB_BIT_SHIFT
#define SMPU7_SUBREGION_DIS (CY_PROT_SUBREGION_DIS0 | \
                             CY_PROT_SUBREGION_DIS1)
#define SMPU7_SLAVE_CONFIG {\
    .address = (void *)SMPU7_BASE, \
    .regionSize = (cy_en_prot_size_t) SMPU7_REGIONSIZE, \
    .subregions = SMPU7_SUBREGION_DIS, \
    .userPermission = CY_PROT_PERM_DISABLED, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU7_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU7_BASE % REGIONSIZE_TO_BYTES(SMPU7_REGIONSIZE)
#error "Flash layout has changed - SMPU7 needs updating"
#endif

/*
 * S_DATA_PRIV_START must equal the base address of subregion 2 of
 * SMPU7
 */
#if S_DATA_PRIV_START != (SMPU7_BASE + \
                          (2 * REGIONSIZE_TO_BYTES(SMPU7_REGIONSIZE) / 8))
#error "Flash layout has changed - S_DATA_PRIV_START isn't subregion 2 of SMPU7"
#endif

#define SMPU10_BASE          S_RAM_ALIAS(0x20000)
#define SMPU10_REGIONSIZE    PROT_SIZE_64KB_BIT_SHIFT
#define SMPU10_SLAVE_CONFIG {\
    .address = (void *)SMPU10_BASE, \
    .regionSize = SMPU10_REGIONSIZE, \
    .subregions = ALL_ENABLED, \
    .userPermission = CY_PROT_PERM_DISABLED, \
    .privPermission = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMatch = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SMPU10_MASTER_CONFIG COMMON_SMPU_MASTER_CONFIG

/* SMPU requires base address aligned to size */
#if SMPU10_BASE % REGIONSIZE_TO_BYTES(SMPU10_REGIONSIZE)
#error "Flash layout has changed - SMPU10 needs updating"
#endif

/*
 * SMPU10 must immediately follow SMPU7
 */
#if SMPU10_BASE != (SMPU7_BASE + REGIONSIZE_TO_BYTES(SMPU7_REGIONSIZE))
#error "Flash layout has changed - SMPU10 doesn't immediately follow SMPU7"
#endif

/* SMPU7 and SMPU10 should exactly cover the privileged secure SRAM */
#if ((6*REGIONSIZE_TO_BYTES(SMPU7_REGIONSIZE)/8) + \
     REGIONSIZE_TO_BYTES(SMPU10_REGIONSIZE)) != S_PRIV_DATA_SIZE
#error "SMPU7_REGIONSIZE+SMPU10_REGIONSIZE should match S_PRIV_DATA_SIZE"
#endif

#endif /* __SMPU_CONFIG_H__ */
