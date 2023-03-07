/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*! @file
@brief This file contains basic platform-dependent type definitions.
*/
#ifndef _CC_PAL_TYPES_PLAT_H
#define _CC_PAL_TYPES_PLAT_H
/* Host specific types for standard (ISO-C99) compilant platforms */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uintptr_t       CCVirtAddr_t;
typedef uint32_t            CCBool_t;
typedef uint32_t            CCStatus;

#define CCError_t           CCStatus
#define CC_INFINITE         0xFFFFFFFF

#define CEXPORT_C
#define CIMPORT_C

#endif /*_CC_PAL_TYPES_PLAT_H*/
