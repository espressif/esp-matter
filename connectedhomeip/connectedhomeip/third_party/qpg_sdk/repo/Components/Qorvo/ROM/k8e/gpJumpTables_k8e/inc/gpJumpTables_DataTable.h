/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
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

#ifndef _GPJUMPTABLES_DATATABLE_H_
#define _GPJUMPTABLES_DATATABLE_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define GPSCHED_DATAJUMPTABLE_MEMB_LIST \
    void* gpSched_globals_ptr;

#define GPMACCORE_DATAJUMPTABLE_MEMB_LIST \
    void* gpMacCore_GlobalVars_ptr;       \
    void* gpMacCore_ConstGlobalVars_ptr;

#define GPRF4CE_DATAJUMPTABLE_MEMB_LIST \
    void* gpRf4ce_GlobalVars_ptr;       \
    void* gpRf4ce_ConstGlobalVars_ptr;

#define GPBLEADDRESSRESOLVER_DATAJUMPTABLE_CONST_MEMB_LIST \
    void* gpBleAddressResolver_ConstGlobalVars_ptr;

#define GPBLEADVERTISER_DATAJUMPTABLE_MEMB_LIST \
    void* gpBleAdvertiser_GlobalVars_ptr;

#define SILEX_CRYPTOSOC_DATAJUMPTABLE_MEMB_LIST \
    void* silexCryptoSoc_GlobalVars_ptr;

struct gpJumpTables_DataTable_t
{
    GPSCHED_DATAJUMPTABLE_MEMB_LIST
    GPMACCORE_DATAJUMPTABLE_MEMB_LIST
    GPRF4CE_DATAJUMPTABLE_MEMB_LIST
    GPBLEADDRESSRESOLVER_DATAJUMPTABLE_CONST_MEMB_LIST
    GPBLEADVERTISER_DATAJUMPTABLE_MEMB_LIST
    SILEX_CRYPTOSOC_DATAJUMPTABLE_MEMB_LIST
};

extern const struct gpJumpTables_DataTable_t JumpTables_DataTable LINKER_SECTION(".datajumptable");

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#endif //_GPJUMPTABLES_DATATABLE_H_

