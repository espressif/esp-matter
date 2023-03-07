/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_OTP_DEFS_H
#define _CC_OTP_DEFS_H

/*!
@file
@brief This file contains general OTP definitions and memory layout.
*/


#ifdef __cplusplus
extern "C"
{
#endif


/* NVM definitions */
#define CC_OTP_BASE_ADDR            0x2000UL
#define CC_OTP_START_OFFSET         0x00UL
#define CC_OTP_LAST_OFFSET          0x7FFUL

/* [0x00-0x07] Device root key (HUK) */
#define CC_OTP_HUK_OFFSET           0x00UL
#define CC_OTP_HUK_SIZE_IN_WORDS                8

/* [0x08-0x0B] ICV provisioning secret (KPICV) */
#define CC_OTP_KPICV_OFFSET         0x08UL
#define CC_OTP_KPICV_SIZE_IN_WORDS              4

/* [0x0C-0x0F] ICV Code encryption key (KCEICV) */
#define CC_OTP_KCEICV_OFFSET            0x0CUL
#define CC_OTP_KCEICV_SIZE_IN_WORDS             4

/* [0x10] Manufacturer-programmed flags */
#define CC_OTP_MANUFACTURE_FLAG_OFFSET      0x10UL

/* [0x11-0x18] Root-of-Trust Public Key.
* May be used in one of the following configurations:
* - A single 256-bit SHA256 digest of the Secure Boot public key (HBK).                                        :
* - Two 128-bit truncated SHA256 digests of Secure Boot public keys 0 and 1 (HBK0, HBK1) */
#define CC_OTP_HBK_OFFSET           0x11UL
#define CC_OTP_HBK_SIZE_IN_WORDS            8
#define CC_OTP_HBK0_OFFSET          0x11UL
#define CC_OTP_HBK0_SIZE_IN_WORDS           4
#define CC_OTP_HBK1_OFFSET          0x15UL
#define CC_OTP_HBK1_SIZE_IN_WORDS           4

/* [0x19-0x1C] OEM provisioning secret (Kcp) */
#define CC_OTP_KCP_OFFSET           0x19UL
#define CC_OTP_KCP_SIZE_IN_WORDS                4

/* OEM Code encryption key (KCE) */
#define CC_OTP_KCE_OFFSET           0x1DUL
#define CC_OTP_KCE_SIZE_IN_WORDS                4

/* OEM-programmed flags */
#define CC_OTP_OEM_FLAG_OFFSET          0x21UL

/* HBK Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK_MIN_VERSION_OFFSET       0x22UL
#define CC_OTP_HBK_MIN_VERSION_SIZE_IN_WORDS    5

/* HBK0 Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK0_MIN_VERSION_OFFSET      0x22UL
#define CC_OTP_HBK0_MIN_VERSION_SIZE_IN_WORDS   2

/* HBK1 Trusted Firmware minimum version (anti-rollback counter) */
#define CC_OTP_HBK1_MIN_VERSION_OFFSET      0x24UL
#define CC_OTP_HBK1_MIN_VERSION_SIZE_IN_WORDS   3

/* General purpose configuration flags */
#define CC_OTP_ICV_GENERAL_PURPOSE_FLAG_OFFSET  0x27UL
#define CC_OTP_ICV_GENERAL_PURPOSE_FLAG_SIZE_IN_WORDS   1

/* OTP DCU lock mask */
#define CC_OTP_DCU_OFFSET           0x28UL
#define CC_OTP_DCU_SIZE_IN_WORDS                4

/* First stage secure boot loader code and data sections (optional) */
#define CC_OTP_SB_LOADER_CODE_OFFSET        0x2CUL


/* Manufacturer-programmed flags */

/* [7:0] Number of "0" bits in HUK */
#define CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SHIFT        0
#define CC_OTP_MANUFACTURE_FLAG_HUK_ZERO_BITS_BIT_SIZE         8

/* [14:8] Number of "0" bits in KPICV (128 bit) */
#define CC_OTP_MANUFACTURE_FLAG_KPICV_ZERO_BITS_BIT_SHIFT      8
#define CC_OTP_MANUFACTURE_FLAG_KPICV_ZERO_BITS_BIT_SIZE       7

/* [15:15] KPICV "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_KPICV_NOT_IN_USE_BIT_SHIFT     15
#define CC_OTP_MANUFACTURE_FLAG_KPICV_NOT_IN_USE_BIT_SIZE       1

/* [22:16] Number of "0" bits in KCEICV */
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_ZERO_BITS_BIT_SHIFT     16
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_ZERO_BITS_BIT_SIZE       7

/* [23:23] KCEICV "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_NOT_IN_USE_BIT_SHIFT    23
#define CC_OTP_MANUFACTURE_FLAG_KCEICV_NOT_IN_USE_BIT_SIZE      1

/* [30:24] Number of "0" bits in HBK0 (in case it is used as 4 words of the ICV) */
#define CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SHIFT       24
#define CC_OTP_MANUFACTURE_FLAG_HBK0_ZERO_BITS_BIT_SIZE         7

/* [31:31] HBK0 "Not In Use" bit */
#define CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SHIFT      31
#define CC_OTP_MANUFACTURE_FLAG_HBK0_NOT_IN_USE_BIT_SIZE        1


/* OEM-programmed flags */

/* [7:0] Number of "0" bits in HBK1/HBK (128/256 bits public key) */
#define CC_OTP_OEM_FLAG_HBK_ZERO_BITS_BIT_SHIFT         0
#define CC_OTP_OEM_FLAG_HBK_ZERO_BITS_BIT_SIZE          8
#define CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SHIFT        0
#define CC_OTP_OEM_FLAG_HBK1_ZERO_BITS_BIT_SIZE         8

/* [14:8] Number of "0" bits in KCP (128 bit) */
#define CC_OTP_OEM_FLAG_KCP_ZERO_BITS_BIT_SHIFT         8
#define CC_OTP_OEM_FLAG_KCP_ZERO_BITS_BIT_SIZE          7

/* [15:15] KCP "Not In Use" bit */
#define CC_OTP_OEM_FLAG_KCP_NOT_IN_USE_BIT_SHIFT       15
#define CC_OTP_OEM_FLAG_KCP_NOT_IN_USE_BIT_SIZE         1

/* [22:16] Number of "0" bits in KCE */
#define CC_OTP_OEM_FLAG_KCE_ZERO_BITS_BIT_SHIFT        16
#define CC_OTP_OEM_FLAG_KCE_ZERO_BITS_BIT_SIZE          7

/* [23:23] KCE "Not In Use" bit */
#define CC_OTP_OEM_FLAG_KCE_NOT_IN_USE_BIT_SHIFT       23
#define CC_OTP_OEM_FLAG_KCE_NOT_IN_USE_BIT_SIZE         1

/* [29:24] Reserved */

/* [30:30] OEM RMA mode flag */
#define CC_OTP_OEM_FLAG_OEM_RMA_MODE_BIT_SHIFT         30
#define CC_OTP_OEM_FLAG_OEM_RMA_MODE_BIT_SIZE           1

/* [31:31] ICV RMA mode flag */
#define CC_OTP_OEM_FLAG_ICV_RMA_MODE_BIT_SHIFT         31
#define CC_OTP_OEM_FLAG_ICV_RMA_MODE_BIT_SIZE           1


#ifdef __cplusplus
}
#endif

#endif



