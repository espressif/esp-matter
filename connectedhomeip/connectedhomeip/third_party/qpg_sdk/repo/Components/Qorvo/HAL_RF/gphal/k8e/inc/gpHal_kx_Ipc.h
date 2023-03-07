/*
 * Copyright (c) 2017, Qorvo Inc
 *
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

#ifndef _GPHAL_KX_IPC_H_
#define _GPHAL_KX_IPC_H_

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#include "gpHal_kx_gpm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// Flags that need backup/restore between flash accessess
typedef struct {
    UInt8 interruptFlags;
} gpHal_IpcBackupRestoreFlags_t;


#define gpHal_RtSubSystem_BleMgr      0x00
#define gpHal_RtSubSystem_MacFilter   0x01
#define gpHal_RtSubSystem_CalMgr      0x02
typedef UInt8 gpHal_RtSubSystemId_t;

/*****************************************************************************
 *                    Internal Function Prototypes
 *****************************************************************************/

void gpHal_IpcInit(void);
gpHal_Result_t gpHal_IpcTriggerCommand(UInt8 commandId, UInt8 argsLength, UInt8* pArgs);

#define gpHal_IpcStop(f)     NOT_USED(f) //Stubbed to avoid call overhead
#define gpHal_IpcRestart(f)  NOT_USED(f) //Stubbed to avoid call overhead

/**
 * @brief Getter method for RT system version.
 * @param subsystem_id The id of the RT subsystem
 * @return The version of the RealTime system.
*/
UInt8 gpHal_GetRtSystemVersion(gpHal_RtSubSystemId_t subsystem_id);

void gpHal_RtInitVersionInfo(void);


#ifdef __cplusplus
}
#endif

#endif //_GPHAL_KX_IPC_H_

