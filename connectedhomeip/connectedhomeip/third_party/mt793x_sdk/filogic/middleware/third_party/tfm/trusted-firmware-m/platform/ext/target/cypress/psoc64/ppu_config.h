/*
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
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

#ifndef __PPU_CONFIG_H__
#define __PPU_CONFIG_H__

/* PPU configs */

/* The majority will be configured in similar ways */
#define NON_SECURE_READ_ONLY_CONFIG { \
    .user = CY_PROT_PERM_R, \
    .priv = CY_PROT_PERM_R, \
    .secure = false, \
    .pcMask = ALL_PCS_MASK, \
}
#define NON_SECURE_READ_WRITE_CONFIG { \
    .user = CY_PROT_PERM_RW, \
    .priv = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMask = ALL_PCS_MASK, \
}
#define NON_SECURE_PRIV_ONLY_CONFIG { \
    .user = CY_PROT_PERM_DISABLED, \
    .priv = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMask = ALL_PCS_MASK, \
}
#define NON_SECURE_EXCEPT_TC_READ_WRITE_CONFIG { \
    .user = CY_PROT_PERM_RW, \
    .priv = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMask = ALL_PCS_EXCEPT_TC_MASK, \
}
#define SECURE_READ_ONLY_CONFIG { \
    .user = CY_PROT_PERM_R, \
    .priv = CY_PROT_PERM_R, \
    .secure = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SECURE_READ_WRITE_CONFIG { \
    .user = CY_PROT_PERM_R, \
    .priv = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMask = SECURE_PCS_MASK, \
}
#define SECURE_PRIV_ONLY_CONFIG { \
    .user = CY_PROT_PERM_DISABLED, \
    .priv = CY_PROT_PERM_RW, \
    .secure = false, \
    .pcMask = SECURE_PCS_MASK, \
}

/* Actual configs for each PPU */

/* To provide a config for a programmable PPU,
 * use something like this:
#define PPU_PR7_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_PR7_SLAVE_CONFIG { \
    .user = CY_PROT_PERM_R, \
    .priv = CY_PROT_PERM_R, \
    .secure = true, \
    .pcMask = SECURE_PCS_MASK, \
}
#define PPU_PR7_SLAVE_ADDRESS CPUSS_BASE
#define PPU_PR7_SLAVE_REGION_SIZE CY_PROT_SIZE_2KB
*/

#define PPU_CRYPTO_MAIN_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_MAIN_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#define PPU_CRYPTO_CRYPTO_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_CRYPTO_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#define PPU_CRYPTO_BOOT_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_BOOT_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#define PPU_CRYPTO_KEY0_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_KEY0_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#define PPU_CRYPTO_KEY1_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_KEY1_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#define PPU_CRYPTO_BUF_MASTER_CONFIG NON_SECURE_READ_ONLY_CONFIG
#define PPU_CRYPTO_BUF_SLAVE_CONFIG SECURE_PRIV_ONLY_CONFIG

#endif /* __PPU_CONFIG_H__ */
