/*
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>

#include "driver_dap.h"

#include "cycfg.h"
#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_ipc_drv.h"
#include "cy_prot.h"
#include "cy_sysint.h"
#include "driver_dap.h"
#include "pc_config.h"

/* DAPControl SysCall opcode */
#define DAPCONTROL_SYSCALL_OPCODE       (0x3AUL << 24UL)

/* FlashBoot SysCall success return code */
#define CY_FB_SYSCALL_SUCCESS           (0xA0000000UL)

/* SysCall timeout value */
#define CY_DAP_SYSCALL_WAIT_MAX_TRIES   (15000UL)


int cy_access_port_control(enum cy_ap_name ap, enum cy_ap_control en)
{
    int rc = -1;
    uint32_t syscallCmd, result;
    uint32_t timeout = 0U;

    syscallCmd  = DAPCONTROL_SYSCALL_OPCODE;
    syscallCmd |= (uint8_t)en << 16;
    syscallCmd |= (uint8_t)ap << 8;
    syscallCmd |= 1;

    /* Get IPC base register address */
    IPC_STRUCT_Type * ipcStruct = Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL);

    while ((CY_IPC_DRV_SUCCESS != Cy_IPC_Drv_LockAcquire(ipcStruct)) &&
            (timeout < CY_DAP_SYSCALL_WAIT_MAX_TRIES)) {
        ++timeout;
    }

    if (timeout < CY_DAP_SYSCALL_WAIT_MAX_TRIES) {
        timeout = 0U;

        Cy_IPC_Drv_WriteDataValue(ipcStruct, syscallCmd);
        Cy_IPC_Drv_AcquireNotify(ipcStruct, (1<<CY_IPC_CHAN_SYSCALL));

        while ((Cy_IPC_Drv_IsLockAcquired(ipcStruct))&&
                (timeout < CY_DAP_SYSCALL_WAIT_MAX_TRIES)) {
            ++timeout;
        }

        if (timeout < CY_DAP_SYSCALL_WAIT_MAX_TRIES) {
            result = Cy_IPC_Drv_ReadDataValue(ipcStruct);
            if (result != CY_FB_SYSCALL_SUCCESS) {
                rc = result;
            }
            else {
                rc = 0;
            }
        }
    }
    return rc;
}
