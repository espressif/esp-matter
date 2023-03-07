/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 * Copyright (c) 2019, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* -------------------------------------- Includes ----------------------------------- */
#include "cmsis.h"
#include "cmsis_compiler.h"

#include "cy_device.h"
#include "cy_device_headers.h"
#include "cy_ipc_drv.h"
#include "cy_sysint.h"
#include "cy_ipc_sema.h"

#include "spe_ipc_config.h"
#include "tfm_spe_mailbox.h"
#include "platform_multicore.h"

/* -------------------------------------- HAL API ------------------------------------ */

int32_t tfm_mailbox_hal_notify_peer(void)
{
    cy_en_ipcdrv_status_t status;

    status = Cy_IPC_Drv_SendMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(IPC_TX_CHAN),
                                    IPC_TX_NOTIFY_MASK,
                                    PSA_CLIENT_CALL_REPLY_MAGIC);

    if (status == CY_IPC_DRV_SUCCESS) {
        return MAILBOX_SUCCESS;
    } else {
        return MAILBOX_CHAN_BUSY;
    }
}

static void mailbox_ipc_config(void)
{
    Cy_SysInt_SetIntSource(PSA_CLIENT_CALL_NVIC_IRQn, PSA_CLIENT_CALL_IPC_INTR);

    NVIC_SetPriority(PSA_CLIENT_CALL_NVIC_IRQn, PSA_CLIENT_CALL_IRQ_PRIORITY);

    NVIC_EnableIRQ(PSA_CLIENT_CALL_NVIC_IRQn);
}

static int32_t tfm_mailbox_sema_init(void)
{
#if defined(CY_IPC_DEFAULT_CFG_DISABLE)
    if (Cy_IPC_Sema_Init(PLATFORM_MAILBOX_IPC_CHAN_SEMA, 0,
                         NULL) != CY_IPC_SEMA_SUCCESS) {
        return PLATFORM_MAILBOX_INIT_ERROR;
    }
#endif

    if (MAILBOX_SEMAPHORE_NUM >= Cy_IPC_Sema_GetMaxSems()) {
        return PLATFORM_MAILBOX_INIT_ERROR;
    }

    /* TODO Check that the semaphore data is in NS memory */

    return PLATFORM_MAILBOX_SUCCESS;
}

int32_t tfm_mailbox_hal_init(struct secure_mailbox_queue_t *s_queue)
{
    struct ns_mailbox_queue_t *ns_queue = NULL;

    /* Init semaphores used for critical sections */
    if (tfm_mailbox_sema_init() != PLATFORM_MAILBOX_SUCCESS)
        return MAILBOX_INIT_ERROR;

    /* Inform NSPE that NSPE mailbox initialization can start */
    platform_mailbox_send_msg_data(NS_MAILBOX_INIT_ENABLE);

    platform_mailbox_wait_for_notify();

    /* Receive the address of NSPE mailbox queue */
    platform_mailbox_fetch_msg_ptr((void **)&ns_queue);

    /*
     * FIXME
     * Necessary sanity check of the address of NPSE mailbox queue should
     * be implemented there.
     */

    s_queue->ns_queue = ns_queue;

    mailbox_ipc_config();

    /* Inform NSPE that SPE mailbox service is ready */
    platform_mailbox_send_msg_data(S_MAILBOX_READY);

    return MAILBOX_SUCCESS;
}

void tfm_mailbox_hal_enter_critical(void)
{
    while (CY_IPC_SEMA_SUCCESS !=
        Cy_IPC_Sema_Set(MAILBOX_SEMAPHORE_NUM, false))
    {
    }
}

void tfm_mailbox_hal_exit_critical(void)
{
    while (CY_IPC_SEMA_SUCCESS !=
        Cy_IPC_Sema_Clear(MAILBOX_SEMAPHORE_NUM, false))
    {
    }
}
