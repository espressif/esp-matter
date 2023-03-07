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
/* Host specific types for standard (ISO-C99) compliant platforms */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*! Type definition for virtual address. */
typedef uintptr_t       CCVirtAddr_t;
/*! Type Definition for boolean variable. */
typedef uint32_t            CCBool_t;
/*! Type definition for return status. */
typedef uint32_t            CCStatus;

/*! Type definition for error return. */
#define CCError_t           CCStatus
/*! Defines inifinite value, used to define unlimited time frame. */
#define CC_INFINITE         0xFFFFFFFF

/*! Type definition for C export. */
#define CEXPORT_C
/*! Type definition for C import. */
#define CIMPORT_C

#endif /*_CC_PAL_TYPES_PLAT_H*/
