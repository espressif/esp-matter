/*
 * Copyright (c) 2021, Qorvo Inc
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_JUMPTABLES_K8E

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpJumpTables_DataTable.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

// include the references to the globals struct
#ifdef GP_COMP_SCHED
#include "gpSched_defs.h"
#include "gpSched_DataJumpTable.h"
#endif //GP_COMP_SCHED
#ifdef GP_COMP_MACCORE
#include "gpMacCore_defs.h"
#include "gpMacCore_DataJumpTable.h"
#endif //GP_COMP_MACCORE
#ifdef GP_COMP_RF4CE
#include "gpRf4ce_defs.h"
#include "gpRf4ce_DataJumpTable.h"
#endif //GP_COMP_RF4CE
#ifdef GP_COMP_BLEADDRESSRESOLVER
#include "gpBleAddressResolver_defs.h"
#include "gpBleAddressResolver_DataJumpTable.h"
#endif //GP_COMP_RF4CE
#ifdef GP_COMP_BLEADVERTISER
#include "gpBleAdvertiser_DataJumpTable.h"
#endif //GP_COMP_BLEADVERTISER
#ifdef GP_COMP_SILEXCRYPTOSOC
#include "silexCryptoSoc_DataJumpTable.h"
#endif

const struct gpJumpTables_DataTable_t JumpTables_DataTable LINKER_SECTION(".datajumptable") =
{
#ifdef GP_COMP_SCHED
    GPSCHED_DATAJUMPTABLE_INIT_LIST
#endif //GP_COMP_SCHED
#ifdef GP_COMP_MACCORE
    GPMACCORE_DATAJUMPTABLE_INIT_LIST
#endif //GP_COMP_MACCORE
#ifdef GP_COMP_RF4CE
    GPRF4CE_DATAJUMPTABLE_INIT_LIST
#endif //GP_COMP_RF4CE
#ifdef GP_COMP_BLEADDRESSRESOLVER
    // Constants
    GPBLEADDRESSRESOLVER_DATAJUMPTABLE_CONST_INIT_LIST
#endif //GP_COMP_BLEADDRESSRESOLVER
#ifdef GP_COMP_BLEADVERTISER
    GPBLEADVERTISER_DATAJUMPTABLE_INIT_LIST
#endif //GP_COMP_BLEADVERTISER
#ifdef GP_COMP_SILEXCRYPTOSOC
    SILEX_CRYPTOSOC_DATAJUMPTABLE_INIT_LIST
#endif
};

extern UInt8 flash_jump_gpJumpTables_GetRomVersion();
typedef UInt8 (*gpJumpTables_GetRomVersion_ptr_t)(void);
UInt8 gpJumpTables_GetRomVersionFromRom(void)
{
    // Use reference from Flash jump table, but take virtual window offset into account
    // Values in the table are expected to be triggered from the virtual window out
    gpJumpTables_GetRomVersion_ptr_t pGetRomVersionFunc;

    pGetRomVersionFunc = (gpJumpTables_GetRomVersion_ptr_t)((UIntPtr)flash_jump_gpJumpTables_GetRomVersion + (GP_MM_FLASH_VIRT_WINDOWS_START - GP_MM_FLASH_START));

    return pGetRomVersionFunc();
}
