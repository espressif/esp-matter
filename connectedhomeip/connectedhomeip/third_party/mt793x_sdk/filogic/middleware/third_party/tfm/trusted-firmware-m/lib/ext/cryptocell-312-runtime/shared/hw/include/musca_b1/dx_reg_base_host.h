/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __DX_REG_BASE_HOST_H__
#define __DX_REG_BASE_HOST_H__

/* Identify platform: ARM MUSCA_B1 */
#define DX_PLAT_MUSCA_B1 1

#define DX_BASE_CC 0x50088000
#define DX_BASE_CODE 0x50030000 //# not used

#define DX_BASE_ENV_REGS 0x500A0000 //TODO need confirm

#define DX_BASE_HOST_RGF 0x0UL
#define DX_BASE_CRY_KERNEL     0x0UL

#define DX_BASE_RNG 0x0000UL
#endif /*__DX_REG_BASE_HOST_H__*/
