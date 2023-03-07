/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "tfm_ss_core_test_2.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "spm_partition_defs.h"
#include "psa/service.h"
#include "psa_manifest/tfm_test_core_2.h"
#include "tfm_peripherals_def.h"

#define INVALID_NS_CLIENT_ID  0x49abcdef
#define INVERT_BUFFER_SIZE    (16*4)

#ifndef TFM_PSA_API
/* Don't initialise caller_partition_id_zi and expect it to be linked in the
 * zero-initialised data area
 */
static int32_t caller_client_id_zi;

/* Initialise caller_partition_id_rw and expect it to be linked in the
 * read-write data area
 */
static int32_t caller_client_id_rw = INVALID_NS_CLIENT_ID;

static int32_t* invalid_addresses [] = {(int32_t*)0x0, (int32_t*)0xFFF12000};
#endif /* !defined(TFM_PSA_API) */

/* structures for secure IRQ testing */
static struct irq_test_execution_data_t *current_execution_data;

psa_status_t spm_core_test_2_slave_service(struct psa_invec *in_vec,
                                           size_t in_len,
                                           struct psa_outvec *out_vec,
                                           size_t out_len)
{
    /* This function doesn't do any sanity check on the input parameters, nor
     * makes any expectation of them, always returns successfully, with a
     * non-zero return value.
     * This is to test the parameter sanitization mechanisms implemented in SPM,
     * and the handling of non-zero success codes.
     */

    return CORE_TEST_ERRNO_SUCCESS_2;
}

#ifndef TFM_PSA_API
psa_status_t spm_core_test_2_check_caller_client_id(struct psa_invec *in_vec,
                                                    size_t in_len,
                                                    struct psa_outvec *out_vec,
                                                    size_t out_len)
{
    size_t i;
    int32_t caller_client_id_stack = INVALID_NS_CLIENT_ID;
    int32_t ret;

    caller_client_id_zi = INVALID_NS_CLIENT_ID;

    /* test with invalid output pointers */
    for (i = 0; i < sizeof(invalid_addresses)/sizeof(invalid_addresses[0]); ++i)
    {
        ret = tfm_core_get_caller_client_id(invalid_addresses[i]);
        if (ret != TFM_ERROR_INVALID_PARAMETER) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
    }

    /* test with valid output pointers */
    ret = tfm_core_get_caller_client_id(&caller_client_id_zi);
    if (ret != TFM_SUCCESS || caller_client_id_zi != TFM_SP_CORE_TEST) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_rw);
    if (ret != TFM_SUCCESS || caller_client_id_rw != TFM_SP_CORE_TEST) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_stack);
    if (ret != TFM_SUCCESS ||
            caller_client_id_stack != TFM_SP_CORE_TEST) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}
#endif /* !defined(TFM_PSA_API) */

psa_status_t spm_core_test_2_get_every_second_byte_internal(
                                                           const uint8_t *inbuf,
                                                           uint8_t *outbuf,
                                                           size_t in_size,
                                                           size_t *out_size)
{
    int j;

    if (in_size/2 > *out_size) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }
    for (j = 1; j < in_size; j += 2) {
        outbuf[j/2] = inbuf[j];
    }
    *out_size = in_size/2;

    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_get_every_second_byte(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    int i;
    psa_status_t res;

    if (in_len != out_len) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }
    for (i = 0; i < in_len; ++i) {
        res = spm_core_test_2_get_every_second_byte_internal(
                              in_vec[i].base, out_vec[i].base,
                              in_vec[i].len, &out_vec[i].len);
        if (res < 0) {
            return res;
        }
    }
    return CORE_TEST_ERRNO_SUCCESS;
}

/* Invert function */
#define SFN_INVERT_MAX_LEN 128
static psa_status_t spm_core_test_2_sfn_invert_internal(uint32_t *in_ptr,
                                                        uint32_t *out_ptr,
                                                        int32_t *res_ptr,
                                                        int32_t len)
{
    int32_t i;
    static uint32_t invert_buffer[SFN_INVERT_MAX_LEN];

    *res_ptr = -1;

    if (len > SFN_INVERT_MAX_LEN) {
        return CORE_TEST_ERRNO_INVALID_BUFFER;
    }

    for (i = 0; i < len; i++) {
        invert_buffer[i] = in_ptr[i];
    }
    for (i = 0; i < len; i++) {
        invert_buffer[i] = ~invert_buffer[i];
    }
    for (i = 0; i < len; i++) {
        out_ptr[i] = invert_buffer[i];
    }

    *res_ptr = 0;
    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_sfn_invert(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    int32_t len;
    uint32_t *in_ptr;
    uint32_t *out_ptr;
    int32_t *res_ptr;

    if ((in_len != 1) || (out_len != 2)) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    if ((out_vec[0].len < in_vec[0].len) || (in_vec[0].len%4 != 0) ||
        (out_vec[1].len < sizeof(int32_t))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    len = in_vec[0].len / 4;

    in_ptr = (uint32_t *)in_vec[0].base;
    out_ptr = (uint32_t *)out_vec[0].base;
    res_ptr = (int32_t *)out_vec[1].base;

    return spm_core_test_2_sfn_invert_internal(in_ptr, out_ptr, res_ptr, len);
}

static psa_status_t spm_core_test_2_prepare_test_scenario_internal(
                               enum irq_test_scenario_t irq_test_scenario,
                               struct irq_test_execution_data_t *execution_data)
{
    current_execution_data = execution_data;

    switch (irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
    case IRQ_TEST_SCENARIO_2:
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
    case IRQ_TEST_SCENARIO_5:
        /* No action is necessary*/
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_prepare_test_scenario(
                             struct psa_invec *in_vec, size_t in_len,
                             struct psa_outvec *out_vec, size_t out_size)
{
    if ((in_len != 2) ||
        (in_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[1].len != sizeof(struct irq_test_execution_data_t *))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    enum irq_test_scenario_t irq_test_scenario =
            (enum irq_test_scenario_t) *(uint32_t *)in_vec[0].base;

    struct irq_test_execution_data_t *execution_data =
            *(struct irq_test_execution_data_t **)in_vec[1].base;

    return spm_core_test_2_prepare_test_scenario_internal(irq_test_scenario,
                                                          execution_data);
}

static psa_status_t spm_core_test_2_execute_test_scenario_internal(
                                     enum irq_test_scenario_t irq_test_scenario)
{
    switch (irq_test_scenario) {
    case IRQ_TEST_SCENARIO_NONE:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    case IRQ_TEST_SCENARIO_1:
        /* No action is necessary*/
        break;
    case IRQ_TEST_SCENARIO_2:
        if (current_execution_data->timer0_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        while (!current_execution_data->timer0_triggered) {
            ;
        }
        break;
    case IRQ_TEST_SCENARIO_3:
    case IRQ_TEST_SCENARIO_4:
        /* No action is necessary*/
        break;
    case IRQ_TEST_SCENARIO_5:
        if (current_execution_data->timer1_triggered) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
        while (!current_execution_data->timer1_triggered) {
            ;
        }
        break;
    default:
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_execute_test_scenario(
                                    struct psa_invec *in_vec, size_t in_len,
                                    struct psa_outvec *out_vec, size_t out_size)
{
    enum irq_test_scenario_t irq_test_scenario =
            (enum irq_test_scenario_t) *(uint32_t *)in_vec[0].base;

    return spm_core_test_2_execute_test_scenario_internal(irq_test_scenario);
}


#ifdef TFM_PSA_API

typedef psa_status_t (*core_test_2_func_t)(psa_msg_t *msg);

static void core_test_2_signal_handle(psa_signal_t signal,
                                      core_test_2_func_t pfn)
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

psa_status_t spm_core_test_2_wrap_slave_service(psa_msg_t *msg)
{
    return spm_core_test_2_slave_service(NULL, 0, NULL, 0);
}

psa_status_t spm_core_test_2_wrap_check_caller_client_id(psa_msg_t *msg)
{
    return CORE_TEST_ERRNO_TEST_NOT_SUPPORTED;
}

psa_status_t spm_core_test_2_wrap_get_every_second_byte(psa_msg_t *msg)
{
    uint32_t inbuf[INVERT_BUFFER_SIZE/sizeof(uint32_t)] = {0};
    uint32_t outbuf[INVERT_BUFFER_SIZE/sizeof(uint32_t)] = {0};

    int i;
    size_t num;
    size_t out_len;
    psa_status_t res;

    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        if (msg->in_size[i] > INVERT_BUFFER_SIZE) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
        }

        if (msg->in_size[i] == 0) {
            continue;
        }

        num = psa_read(msg->handle, i, inbuf, msg->in_size[i]);
        if (num != msg->in_size[i]) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
        }

        out_len = msg->out_size[i];

        res = spm_core_test_2_get_every_second_byte_internal((uint8_t *)inbuf,
                (uint8_t *)outbuf, msg->in_size[i], &out_len);
        if (res < 0) {
            return res;
        }

        psa_write(msg->handle, i, outbuf, out_len);
    }
    return CORE_TEST_ERRNO_SUCCESS;
}

psa_status_t spm_core_test_2_wrap_sfn_invert(psa_msg_t *msg)
{
    uint32_t inbuf[INVERT_BUFFER_SIZE/sizeof(uint32_t)] = {0};
    uint32_t outbuf[INVERT_BUFFER_SIZE/sizeof(uint32_t)] = {0};
    size_t num;
    int32_t res_ptr;
    psa_status_t ret;

    if ((msg->out_size[0] < msg->in_size[0]) ||
        (msg->in_size[0] > INVERT_BUFFER_SIZE) ||
        (msg->in_size[0]%4 != 0) ||
        (msg->out_size[1] < sizeof(int32_t))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 0, inbuf, msg->in_size[0]);
    if (num != msg->in_size[0]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    ret = spm_core_test_2_sfn_invert_internal(inbuf, outbuf,
                                              &res_ptr, msg->in_size[0] / 4);
    if (ret < 0) {
        return ret;
    }

    psa_write(msg->handle, 0, outbuf, msg->in_size[0]);
    psa_write(msg->handle, 1, &res_ptr, sizeof(int32_t));

    return ret;
}

psa_status_t spm_core_test_2_wrap_prepare_test_scenario(psa_msg_t *msg)
{
    uint32_t irq_test_scenario;
    struct irq_test_execution_data_t *execution_data;
    size_t num;

    if ((msg->in_size[0] != sizeof(uint32_t)) ||
        (msg->in_size[1] != sizeof(struct irq_test_execution_data_t*)))  {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 0, &irq_test_scenario, sizeof(irq_test_scenario));
    if (num != msg->in_size[0]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 1, &execution_data, sizeof(
                                            struct irq_test_execution_data_t*));
    if (num != msg->in_size[1]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return spm_core_test_2_prepare_test_scenario_internal((enum irq_test_scenario_t)
                                                          irq_test_scenario,
                                                          execution_data);
}

psa_status_t spm_core_test_2_wrap_execute_test_scenario(psa_msg_t *msg)
{
    uint32_t irq_test_scenario;
    size_t num;

    if (msg->in_size[0] != sizeof(uint32_t))  {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 0, &irq_test_scenario, sizeof(irq_test_scenario));
    if (num != msg->in_size[0]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    return spm_core_test_2_execute_test_scenario_internal((enum irq_test_scenario_t)
                                                          irq_test_scenario);
}

#endif /* defined(TFM_PSA_API) */

/* FIXME: Add a testcase to test that a failed init makes the secure partition
 * closed, and none of its functions can be called.
 * A new test service for this purpose is to be added.
 */
psa_status_t core_test_2_init(void)
{
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & SPM_CORE_TEST_2_SLAVE_SERVICE_SIGNAL) {
            core_test_2_signal_handle(SPM_CORE_TEST_2_SLAVE_SERVICE_SIGNAL,
                                      spm_core_test_2_wrap_slave_service);
        } else if (signals & SPM_CORE_TEST_2_CHECK_CALLER_CLIENT_ID_SIGNAL) {
            core_test_2_signal_handle(
                                  SPM_CORE_TEST_2_CHECK_CALLER_CLIENT_ID_SIGNAL,
                                  spm_core_test_2_wrap_check_caller_client_id);
        } else if (signals & SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SIGNAL) {
            core_test_2_signal_handle(
                                   SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SIGNAL,
                                   spm_core_test_2_wrap_get_every_second_byte);
        } else if (signals & SPM_CORE_TEST_2_INVERT_SIGNAL) {
            core_test_2_signal_handle(SPM_CORE_TEST_2_INVERT_SIGNAL,
                                      spm_core_test_2_wrap_sfn_invert);
        } else if (signals & SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_SIGNAL) {
            core_test_2_signal_handle(
                                   SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_SIGNAL,
                                   spm_core_test_2_wrap_prepare_test_scenario);
        } else if (signals & SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_SIGNAL) {
            core_test_2_signal_handle(
                                   SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_SIGNAL,
                                   spm_core_test_2_wrap_execute_test_scenario);
        } else {
            ; /* do nothing */
        }
    }
#else
    return CORE_TEST_ERRNO_SUCCESS;
#endif /* defined(TFM_PSA_API) */
}
