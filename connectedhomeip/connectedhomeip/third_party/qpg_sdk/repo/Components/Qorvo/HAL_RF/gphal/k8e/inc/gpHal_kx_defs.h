/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPHAL_KX_DEFS_H_
#define _GPHAL_KX_DEFS_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GPHAL_CHIP_ID_K8E           15
#ifndef GP_HAL_EXPECTED_CHIP_ID
#define GP_HAL_EXPECTED_CHIP_ID     GPHAL_CHIP_ID_K8E
#endif

#ifndef GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS
#define GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS 16
#endif

#ifndef GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS
#define GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS 1
#endif

#if GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS > 16
#error We only support up to 16 connections
#endif

#if GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS > GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS
#error Number of slave connections cannot be bigger than the total number of connections
#endif

#ifndef GP_DIVERSITY_BLE_MAX_NR_OF_WHITELIST_ENTRIES
#define GP_DIVERSITY_BLE_MAX_NR_OF_WHITELIST_ENTRIES    33
#endif //GP_DIVERSITY_BLE_MAX_NR_OF_WHITELIST_ENTRIES

#ifndef GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES
#define GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES    2
#endif //GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES

#if GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES > 2
#error We only support up to 2 resolving list entries (see SDB004-315).
#endif //GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES

#include "gpHal_OffsetChecks.h"
#include "gpHal_kx_regprop.h"
#include "gpHal_kx_regprop_basic.h"

#include "gpHal_kx_enum.h"
#include "gpHal_kx_enum_manual.h"

#if defined(GP_DIVERSITY_GPHAL_INTERN)
#include "gpHal_kx_mm.h"
#else
#include "gpHal_kx_mm_compressed.h"
#endif
#include "gpHal_kx_mm_manual.h"
#include "gpHal_kx_MSI_basic.h"

#if defined(GP_DIVERSITY_GPHAL_INTERN)
#else
#  error is gpHal running onchip or offchip?
#endif

#include "gpHal_kx_Pbm.h"

#include "gpHal_kx_public.h"

#include "gpHal_kx_Flash.h"
// some hardware constants from chip and RT
#include "gpHal_kx_hw_constants.h"

#include "gpHal_kx_Rap.h"


#endif //_GPHAL_KX_DEFS_H_

