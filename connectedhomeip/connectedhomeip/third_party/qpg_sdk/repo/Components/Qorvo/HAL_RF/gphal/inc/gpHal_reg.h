/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 * gpHal_reg.h
 *
 *  The file gpHal_reg.h is a wrapper around the correct register definitions
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GP_HAL_REG_H_
#define _GP_HAL_REG_H_

/**
 * @file gpHal_reg.h
 *
 * Wrapper around the register definitions.
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#if   defined(GP_DIVERSITY_GPHAL_K8E) 
#include "gpHal_kx_defs.h"
#else
#ifdef GP_COMP_UNIT_TEST
//Only be used for gpHal as Tbc
#include "utTbc_gpHal_defs.h"
#endif
#endif

#endif //_GP_HAL_REG_H_

