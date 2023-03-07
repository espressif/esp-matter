/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SECUREBOOT_GENERAL_HWDEFS_H
#define SECUREBOOT_GENERAL_HWDEFS_H


#include "cc_regs.h"
#include "dx_host.h"
#include "dx_crys_kernel.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/
#define SB_REG_ADDR(base, reg_name)     (base + CC_REG_OFFSET(CRY_KERNEL, reg_name))
#define SB_REG_ADDR_UNIT(base, reg_name, unit)  (base + CC_REG_OFFSET(unit, reg_name))



#ifdef __cplusplus
}
#endif

#endif

