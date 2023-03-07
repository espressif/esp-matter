/*
 * Copyright (c) 2001-2020, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DX_REG_BASE_HOST_H__
#define __DX_REG_BASE_HOST_H__

/* Identify platform: ARM MUSCA_S1 */
#define DX_PLAT_MUSCA_S1 1

#define DX_BASE_CC 0x50088000 /* Cryptocell secure address */
#define DX_BASE_CODE 0x50030000 //# not used

/* The address of the crypto keys in the MRAM. The S1 uses this area for
 * persistent storage since it doesn't have true OTP memory.
 * Same as FLASH_TFM_CRYPTO_KEY_AREA as found in
 * platform/ext/target/musca_s1/partition/flash_layout.h
 */
#define DX_MRAM_CC 0x1A1EA000

#define DX_BASE_ENV_REGS 0x500A0000 //TODO need confirm

#define DX_BASE_HOST_RGF 0x0UL
#define DX_BASE_CRY_KERNEL     0x0UL

#define DX_BASE_RNG 0x0000UL
#endif /*__DX_REG_BASE_HOST_H__*/
