/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "cmsis.h"
#include "cmsis_compiler.h"

#include "cy_ipc_drv.h"
#include "spe_ipc_config.h"
#include "platform_multicore.h"

__STATIC_INLINE void tfm_trigger_pendsv(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void mailbox_clear_intr(void)
{
    uint32_t status;

    status = Cy_IPC_Drv_GetInterruptStatusMasked(
                            Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT));
    status >>= CY_IPC_NOTIFY_SHIFT;
    if ((status & IPC_RX_INT_MASK) == 0) {
        return;
    }

    Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(IPC_RX_INTR_STRUCT),
                              0, IPC_RX_INT_MASK);
}

void NvicMux7_IRQHandler(void)
{
    uint32_t magic;

    mailbox_clear_intr();

    platform_mailbox_fetch_msg_data(&magic);
    if (magic == PSA_CLIENT_CALL_REQ_MAGIC) {
        tfm_trigger_pendsv();
    }
}
