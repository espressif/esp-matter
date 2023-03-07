/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_api.h"
#include "tfm_veneers.h"
#include "secure_utilities.h"
#include "tfm_secure_api.h"
#include "secure_fw/include/tfm_spm_services_api.h"
#include "spm_partition_defs.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "psa/service.h"
#include "psa_manifest/tfm_irq_test_service_1.h"
#include "tfm_plat_test.h"

static enum irq_test_scenario_t current_scenario = IRQ_TEST_SCENARIO_NONE;
static struct irq_test_execution_data_t *current_execution_data;

#ifdef TFM_PSA_API
static psa_handle_t execute_msg_handle = -1;
#endif

/**
 * \brief unrecoverable error during test execution.
 *
 * Called from places, where error code would be very difficult, or impossible
 * to return.
 */
static void halt_test_execution(void)
{
    while (1) {
        ; /* Test fail */
    }
}

/**
 * \brief Stop the timer, and disable and clear interrupts
 */
static void stop_timer(void)
{
    tfm_plat_test_secure_timer_stop();
}

int32_t spm_irq_test_1_prepare_test_scenario_internal(
                               enum irq_test_scenario_t irq_test_scenario,
                               struct irq_test_execution_data_t *execution_data)
{
    current_scenario = irq_test_scenario;
    current_execution_data = execution_data;

    current_execution_data->timer0_triggered = 0;

    switch (irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
        tfm_plat_test_secure_timer_start();
        break;
    case IRQ_TEST_SCENARIO_5:
        /* Do nothing */
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

int32_t spm_irq_test_1_execute_test_scenario(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    enum irq_test_scenario_t irq_test_scenario =
            (enum irq_test_scenario_t) *(uint32_t *)in_vec[0].base;
    psa_signal_t signals = 0;

    if (irq_test_scenario != current_scenario) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    switch (irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
        /* nothing to do*/
        break;
    case IRQ_TEST_SCENARIO_3:
        if (current_execution_data->timer0_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        while (!current_execution_data->timer0_triggered) {
            /* Wait for the timer to be triggered */
            ;
        }
        break;
    case IRQ_TEST_SCENARIO_4:
        if (current_execution_data->timer0_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        while ((signals & SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ) == 0) {
            signals = psa_wait(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ,
                               PSA_BLOCK);
        }
        if (!current_execution_data->timer0_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        psa_eoi(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
        break;
    case IRQ_TEST_SCENARIO_5:
        /* nothing to do*/
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

uint32_t spm_irq_test_1_prepare_test_scenario(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    enum irq_test_scenario_t irq_test_scenario;
    struct irq_test_execution_data_t *execution_data;

    if ((in_len != 2) || (out_len != 0)) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    if ((in_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[1].len != sizeof(struct irq_test_execution_data_t *))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    irq_test_scenario =
            (enum irq_test_scenario_t) *(uint32_t *)in_vec[0].base;

    execution_data =
            *(struct irq_test_execution_data_t **)in_vec[1].base;

    return spm_irq_test_1_prepare_test_scenario_internal(irq_test_scenario,
                                                         execution_data);
}

#ifndef TFM_PSA_API

void SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ_isr(void)
{
    stop_timer();

    if ((current_execution_data == NULL) ||
        (current_execution_data->timer0_triggered != 0)) {
            halt_test_execution();
    }

    current_execution_data->timer0_triggered = 1;

    switch (current_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        halt_test_execution();
        break;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
        psa_eoi(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
        break;
    case IRQ_TEST_SCENARIO_4:
        /* nothing to do*/
        break;
    case IRQ_TEST_SCENARIO_5:
        halt_test_execution();
        /* No secure interrups are used in this scenario */
        break;
    default:
        halt_test_execution();
        break;
    }

    __asm("DSB");
}

#else /* TFM_PSA_API */

typedef psa_status_t (*irq_test_1_func_t)(psa_msg_t *msg);

static void spm_irq_test_1_signal_handle(psa_signal_t signal,
                                         irq_test_1_func_t pfn)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    if (status) {
        return;
    }

    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = pfn(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        break;
    }
}

void TIMER_0_isr_ipc(void)
{
    current_execution_data->timer0_triggered = 1;

    tfm_plat_test_secure_timer_stop();

    switch (current_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        halt_test_execution();
        break;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
        psa_eoi(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
        break;
    case IRQ_TEST_SCENARIO_3:
        /* execute_msg_handle have to be valid at this point */
        if (execute_msg_handle <= 0) {
            halt_test_execution();
        }
        /* reply to the execute message, to unblock NS side */
        psa_reply(execute_msg_handle, CORE_TEST_ERRNO_SUCCESS);
        execute_msg_handle = -1;
        psa_eoi(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
        break;
    case IRQ_TEST_SCENARIO_4:
        /* This case should never be executed as in this scenario the 'execute
         * function does the psa wait, and calls psa_eoi immediately'. So when
         * execution gets to the psa_wait in the main loop, the IRQ signal is
         * unset.
         */
        halt_test_execution();
        break;
    case IRQ_TEST_SCENARIO_5:
        /* No secure interrups are used in this scenario */
        halt_test_execution();
        break;
    default:
        halt_test_execution();
        break;
    }
}

static psa_status_t spm_irq_test_1_wrap_prepare_test_scenario(psa_msg_t *msg)
{
    uint32_t irq_test_scenario;
    struct irq_test_execution_data_t *execution_data;
    size_t num;

    if ((msg->in_size[0] != sizeof(uint32_t)) ||
        (msg->in_size[1] != sizeof(struct irq_test_execution_data_t *))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }


    num = psa_read(msg->handle, 0, &irq_test_scenario, sizeof(uint32_t));
    if (num != msg->in_size[0]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 1, &execution_data,
                   sizeof(struct irq_test_execution_data_t *));
    if (num != msg->in_size[1]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return spm_irq_test_1_prepare_test_scenario_internal((enum irq_test_scenario_t)
                                                         irq_test_scenario,
                                                         execution_data);
}

static void spm_irq_test_1_execute_test_scenario_ipc_call(psa_msg_t *msg)
{
    size_t num;
    uint32_t irq_test_scenario;
    psa_signal_t signals = 0;

    num = psa_read(msg->handle, 0, &irq_test_scenario, sizeof(uint32_t));
    if ((num != msg->in_size[0]) ||
        (irq_test_scenario != current_scenario)) {
        psa_reply(msg->handle, CORE_TEST_ERRNO_INVALID_PARAMETER);
        return;
    }

    switch (irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        psa_reply(msg->handle, CORE_TEST_ERRNO_INVALID_PARAMETER);
        return;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_5:
        /* nothing to do, return success */
        psa_reply(msg->handle, CORE_TEST_ERRNO_SUCCESS);
        return;
    case IRQ_TEST_SCENARIO_3:
        if (current_execution_data->timer0_triggered) {
            psa_reply(msg->handle, CORE_TEST_ERRNO_TEST_FAULT);
            return;
        }
        /* We need the ISR to be able to run. So we do a wait to let
         * it run and set timer0_triggered. This message will be replied
         * from the ISR, so the NS side remains blocked for now. To be able
         * to reply, we also save the handle of the message.
         */
        if (execute_msg_handle > 0) {
            /* execute_msg_handle should be uninitialised at this point */
            psa_reply(msg->handle, CORE_TEST_ERRNO_TEST_FAULT);
            return;
        }
        execute_msg_handle = msg->handle;
        while (!(signals & SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ)) {
           signals = psa_wait(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ,
                              PSA_BLOCK);
        }
        return;
    case IRQ_TEST_SCENARIO_4:
        while (!(signals & SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ)) {
            signals = psa_wait(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ,
                              PSA_BLOCK);
        }
        /* There is no need to call the ISR in this scenario, so we can
         * clear the IRQ signal
         */
        stop_timer();
        psa_eoi(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
        psa_reply(msg->handle, CORE_TEST_ERRNO_SUCCESS);
        break;
    default:
        psa_reply(msg->handle, CORE_TEST_ERRNO_INVALID_PARAMETER);
        return;
    }
}

static void spm_irq_test_1_execute_test_scenario_ipc(psa_signal_t signal)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    if (status) {
        return;
    }

    if (signal != SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SIGNAL) {
        psa_reply(msg.handle, CORE_TEST_ERRNO_INVALID_PARAMETER);
    }

    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        spm_irq_test_1_execute_test_scenario_ipc_call(&msg);
    break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        break;
    }
}
#endif /* TFM_PSA_API */

int32_t tfm_irq_test_1_init(void)
{
    tfm_enable_irq(SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ);
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ) {
            TIMER_0_isr_ipc();
        } else if (signals & SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_SIGNAL) {
            spm_irq_test_1_signal_handle(
                               SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_SIGNAL,
                               spm_irq_test_1_wrap_prepare_test_scenario);
        } else if (signals & SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SIGNAL) {
            spm_irq_test_1_execute_test_scenario_ipc(
                              SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SIGNAL);
        } else {
            ; /* do nothing */
        }
    }
#else
    return TFM_SUCCESS;
#endif /* TFM_PSA_API */
}
