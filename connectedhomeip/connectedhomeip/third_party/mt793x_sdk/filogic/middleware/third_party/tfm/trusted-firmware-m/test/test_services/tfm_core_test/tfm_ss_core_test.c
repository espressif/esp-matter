/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_ss_core_test.h"
#include "tfm_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "tfm_veneers.h"
#include "secure_utilities.h"
#include "tfm_secure_api.h"
#include "secure_fw/include/tfm_spm_services_api.h"
#include "spm_partition_defs.h"
#include "psa/service.h"
#include "tfm_plat_test.h"
#include "psa_manifest/tfm_test_core.h"
#ifdef TFM_PSA_API
#include "psa_manifest/sid.h"
#endif

static int32_t partition_init_done;

#define INVALID_NS_CLIENT_ID  0x49abcdef
#define EXPECTED_NS_CLIENT_ID (-1)

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

#else /* !defined(TFM_PSA_API) */

static psa_status_t psa_test_common(uint32_t sid, uint32_t version,
                                    const psa_invec *in_vecs, size_t in_len,
                                    psa_outvec *out_vecs, size_t out_len)
{
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(sid, version);
    if (handle <= 0) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vecs, in_len, out_vecs, out_len);
    if (status < 0) {
        status = CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    }

    psa_close(handle);
    return status;
}
#endif /* !defined(TFM_PSA_API) */

psa_status_t spm_core_test_sfn_init_success(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    if ((in_len != 0) || (out_len != 0)) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    if (partition_init_done) {
        return CORE_TEST_ERRNO_SUCCESS;
    } else {
        return CORE_TEST_ERRNO_SP_NOT_INITED;
    }
}

#ifndef TFM_PSA_API
psa_status_t spm_core_test_sfn_direct_recursion(
                                     struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    uint32_t depth;
    struct psa_invec new_vec = {NULL, sizeof(uint32_t)};

    if ((in_len != 1) || (out_len != 0) ||
        (in_vec[0].len != sizeof(uint32_t))) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    depth = *((uint32_t *)in_vec[0].base);

    if (depth != 0) {
        /* Protect against scenario where TF-M core fails to block recursion */
        return CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED;
    }
    /* Call to the same service again, should be rejected */
    depth += 1;
    new_vec.base = &depth;
    int32_t ret = tfm_spm_core_test_sfn_direct_recursion_veneer(&new_vec,
                                                                1, NULL, 0);

    if (ret == CORE_TEST_ERRNO_SUCCESS) {
        /* This is an unexpected return value */
        return CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR;
    } else if (ret == CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED) {
        /* This means that service was started in recursion */
        return CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED;
    } else {
        return CORE_TEST_ERRNO_SUCCESS;
    }
}
#endif /* !defined(TFM_PSA_API) */

static psa_status_t test_peripheral_access(void)
{
#ifdef TFM_ENABLE_PERIPH_ACCESS_TEST
    uint32_t leds;
    uint32_t invleds;
    uint32_t userled_mask;

    leds = tfm_plat_test_get_led_status();
    tfm_plat_test_set_led_status(~leds);
    invleds = tfm_plat_test_get_led_status();
    userled_mask = tfm_plat_test_get_userled_mask();

    if ((invleds & userled_mask) != (~leds & userled_mask)) {
        /* Code failed to invert value in peripheral reg */
        return CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED;
    }

    return CORE_TEST_ERRNO_SUCCESS;
#else
    return CORE_TEST_ERRNO_TEST_NOT_SUPPORTED;
#endif
}

#define SS_BUFFER_LEN 16

static psa_status_t test_ss_to_ss_buffer(uint32_t *in_ptr, uint32_t *out_ptr,
                                         int32_t len)
{
    int32_t i;
    /* Service internal buffer */
    uint32_t ss_buffer[SS_BUFFER_LEN] = {0};
    uint32_t slave_buffer [len];
    int32_t result;
    int32_t *result_ptr = &result;
    int32_t res;
    psa_invec in_vec[] = { {slave_buffer, len*sizeof(uint32_t)} };
    psa_outvec outvec[] = { {slave_buffer, len*sizeof(uint32_t)},
                            {result_ptr, sizeof(int32_t)} };

    if (len > SS_BUFFER_LEN) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    for (i = 0; i < len; i++) {
        ss_buffer[i] = in_ptr[i];
    }

    for (i = 0; i < len; i++) {
        slave_buffer[i] = ss_buffer[i];
    }

    /* Call internal service with buffer handling */

#ifdef TFM_PSA_API
    res = psa_test_common(SPM_CORE_TEST_2_INVERT_SID,
                          SPM_CORE_TEST_2_INVERT_VERSION,
                          in_vec, 1, outvec, 2);
#else /* defined(TFM_PSA_API) */
    res = tfm_spm_core_test_2_sfn_invert_veneer(in_vec, 1, outvec, 2);
#endif /* defined(TFM_PSA_API) */

    if (res != CORE_TEST_ERRNO_SUCCESS) {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }

    for (i = 0; i < len; i++) {
        if (slave_buffer[i] != ~ss_buffer[i]) {
            return CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE;
        }
        ss_buffer[i] = slave_buffer[i];
    }

    for (i = 0; i < len; i++) {
        out_ptr[i] = ss_buffer[i];
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

static psa_status_t test_outvec_write(void)
{
    int32_t err;
    int i;
    uint8_t data_buf [36]; /* (6 + 12) * 2 = 36 plus some alignment */
    uint8_t *data_buf_ptr = data_buf;
    psa_invec in_vec [2];
    psa_outvec out_vec [2];
    uint8_t *in_buf_0;
    uint8_t *in_buf_1;
    uint8_t *out_buf_0;
    uint8_t *out_buf_1;

    in_buf_0 = data_buf_ptr;
    for (i = 0; i < 5; ++i, ++data_buf_ptr)
    {
        *data_buf_ptr = i;
    }
    in_vec[0].base = in_buf_0;
    in_vec[0].len = data_buf_ptr - in_buf_0;

    in_buf_1 = data_buf_ptr;
    *(data_buf_ptr++) = 1;
    *(data_buf_ptr++) = 1;
    for (i = 2; i < 11; ++i, ++data_buf_ptr)
    {
        *data_buf_ptr = *(data_buf_ptr-1) + *(data_buf_ptr-2);
    }
    in_vec[1].base = in_buf_1;
    in_vec[1].len = data_buf_ptr - in_buf_1;

    out_buf_0 = data_buf_ptr;
    data_buf_ptr += in_vec[0].len;
    out_vec[0].base = out_buf_0;
    out_vec[0].len = data_buf_ptr - out_buf_0;

    out_buf_1 = data_buf_ptr;
    data_buf_ptr += in_vec[1].len;
    out_vec[1].base = out_buf_1;
    out_vec[1].len = data_buf_ptr - out_buf_1;

#ifdef TFM_PSA_API
    err = psa_test_common(SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SID,
                          SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_VERSION,
                          in_vec, 2, out_vec, 2);
#else /* defined(TFM_PSA_API) */
    err = tfm_spm_core_test_2_get_every_second_byte_veneer(in_vec, 2,
                                                           out_vec, 2);
#endif /* defined(TFM_PSA_API) */

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    if (out_vec[0].len != in_vec[0].len/2 ||
        out_vec[1].len != in_vec[1].len/2) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }
    for (i = 1; i < sizeof(in_buf_0); i += 2) {
        if (((uint8_t *)out_vec[0].base)[i/2] != in_buf_0[i]) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
    }
    for (i = 1; i < sizeof(in_buf_1); i += 2) {
        if (((uint8_t *)out_vec[1].base)[i/2] != in_buf_1[i]) {
            return CORE_TEST_ERRNO_TEST_FAULT;
        }
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

static psa_status_t test_ss_to_ss(void)
{
    int32_t ret;
    /* Call to a different service, should be successful */
#ifdef TFM_PSA_API
    ret = psa_test_common(SPM_CORE_TEST_2_SLAVE_SERVICE_SID,
                          SPM_CORE_TEST_2_SLAVE_SERVICE_VERSION,
                          NULL, 0, NULL, 0);
#else /* defined(TFM_PSA_API) */
    ret = tfm_spm_core_test_2_slave_service_veneer(NULL, 0, NULL, 0);
#endif /* defined(TFM_PSA_API) */
    if (ret == CORE_TEST_ERRNO_SUCCESS_2) {
        return CORE_TEST_ERRNO_SUCCESS;
    } else {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }
}

#ifndef TFM_PSA_API
static psa_status_t test_get_caller_client_id(void)
{
    /* Call to a special service that checks the caller service ID */
    size_t i;
    int32_t ret;
    int32_t caller_client_id_stack = INVALID_NS_CLIENT_ID;

    caller_client_id_zi = INVALID_NS_CLIENT_ID;

    ret = tfm_spm_core_test_2_check_caller_client_id_veneer(NULL, 0, NULL, 0);
    if (ret != CORE_TEST_ERRNO_SUCCESS) {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }

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
    if (ret != TFM_SUCCESS || caller_client_id_zi != EXPECTED_NS_CLIENT_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_rw);
    if (ret != TFM_SUCCESS || caller_client_id_rw != EXPECTED_NS_CLIENT_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    ret = tfm_core_get_caller_client_id(&caller_client_id_stack);
    if (ret != TFM_SUCCESS ||
            caller_client_id_stack != EXPECTED_NS_CLIENT_ID) {
        return CORE_TEST_ERRNO_TEST_FAULT;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}

static psa_status_t test_spm_request(void)
{
    /* Request a reset vote, should be successful */
    int32_t ret = tfm_spm_request_reset_vote();

    if (ret != TFM_SUCCESS) {
        return CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE;
    }

    return CORE_TEST_ERRNO_SUCCESS;
}
#endif /* !defined(TFM_PSA_API) */

#ifdef CORE_TEST_INTERACTIVE
static void wait_button_event(void)
{
    tfm_plat_test_wait_user_button_pressed();
    /*
     * The follow wait is required to skip multiple continues in one go due to
     * the fast execution of the code and time used by the user to
     * release button.
     */

    tfm_plat_test_wait_user_button_released();
}

psa_status_t test_wait_button(void)
{
    LOG_MSG("Inside the service, press button to continue...");
    wait_button_event();
    LOG_MSG("Leaving the service");
    return CORE_TEST_ERRNO_SUCCESS;
}
#endif /* defined(CORE_TEST_INTERACTIVE) */

static psa_status_t test_block(void)
{
#ifdef CORE_TEST_INTERACTIVE
    /* Only block if interactive test is turned on */
    return test_wait_button();
#else /* defined(CORE_TEST_INTERACTIVE) */
    /* This test should not be run if interactive tests are disabled */
    return CORE_TEST_ERRNO_TEST_FAULT;
#endif /* defined(CORE_TEST_INTERACTIVE) */
}

#ifndef TFM_PSA_API
psa_status_t spm_core_test_sfn(struct psa_invec *in_vec, size_t in_len,
                          struct psa_outvec *out_vec, size_t out_len)
{
    uint32_t tc;
    int32_t arg1;
    int32_t arg2;
    int32_t arg3;

    if ((in_len < 1) || (in_vec[0].len != sizeof(uint32_t))) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }
    tc = *((uint32_t *)in_vec[0].base);

    switch (tc) {
    case CORE_TEST_ID_SS_TO_SS:
        return test_ss_to_ss();
    case CORE_TEST_ID_SS_TO_SS_BUFFER:
        if ((in_len != 3) || (out_len != 1) ||
        (in_vec[2].len != sizeof(int32_t))) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
        }
        arg3 = *((int32_t *)in_vec[2].base);
        if ((in_vec[1].len < arg3*sizeof(int32_t)) ||
            (out_vec[0].len < arg3*sizeof(int32_t))) {
            return CORE_TEST_ERRNO_INVALID_PARAMETER;
        }
        arg1 = (int32_t)in_vec[1].base;
        arg2 = (int32_t)out_vec[0].base;
        return test_ss_to_ss_buffer((uint32_t *)arg1, (uint32_t *)arg2, arg3);
    case CORE_TEST_ID_OUTVEC_WRITE:
        return test_outvec_write();
    case CORE_TEST_ID_PERIPHERAL_ACCESS:
        return test_peripheral_access();
    case CORE_TEST_ID_GET_CALLER_CLIENT_ID:
        return test_get_caller_client_id();
    case CORE_TEST_ID_SPM_REQUEST:
        return test_spm_request();
    case CORE_TEST_ID_BLOCK:
        return test_block();
    case CORE_TEST_ID_NS_THREAD:
        /* dummy service call is enough */
        return CORE_TEST_ERRNO_SUCCESS;
    default:
        return CORE_TEST_ERRNO_INVALID_TEST_ID;
    }
}

#else /* !defined(TFM_PSA_API) */

#define SS_TO_SS_BUFFER_SIZE (16*4)

typedef psa_status_t (*core_test_func_t)(psa_msg_t *msg);

static psa_status_t tfm_core_test_sfn_wrap_init_success(psa_msg_t *msg)
{
    return spm_core_test_sfn_init_success(NULL, 0, NULL, 0);
}

static psa_status_t tfm_core_test_sfn_wrap_direct_recursion(psa_msg_t *msg)
{
    return CORE_TEST_ERRNO_TEST_FAULT;
}

static psa_status_t tfm_core_test_sfn_wrap_ss_to_ss(psa_msg_t *msg)
{
    return test_ss_to_ss();
}

static psa_status_t tfm_core_test_sfn_wrap_ss_to_ss_buffer(psa_msg_t *msg)
{
    size_t num;
    uint32_t inbuf[SS_TO_SS_BUFFER_SIZE/sizeof(uint32_t)] = {0};
    uint32_t outbuf[SS_TO_SS_BUFFER_SIZE/sizeof(uint32_t)] = {0};
    uint32_t len;
    psa_status_t res;

    if ((msg->in_size[0] > SS_TO_SS_BUFFER_SIZE) ||
        (msg->in_size[1] != sizeof(int32_t)) ||
        (msg->out_size[0] > SS_TO_SS_BUFFER_SIZE)) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 0, inbuf, msg->in_size[0]);
    if (num != msg->in_size[0]) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    num = psa_read(msg->handle, 1, &len, sizeof(int32_t));
    if (num != sizeof(int32_t)) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    if (len > SS_TO_SS_BUFFER_SIZE) {
        return CORE_TEST_ERRNO_INVALID_PARAMETER;
    }

    res = test_ss_to_ss_buffer(inbuf, outbuf, len);
    if (res < 0) {
        return res;
    }

    psa_write(msg->handle, 0, outbuf, len * sizeof(uint32_t));

    return res;
}

static psa_status_t tfm_core_test_sfn_wrap_outvec_write(psa_msg_t *msg)
{
    return test_outvec_write();
}

static psa_status_t tfm_core_test_sfn_wrap_peripheral_access(psa_msg_t *msg)
{
    return test_peripheral_access();
}

static psa_status_t tfm_core_test_sfn_wrap_get_caller_client_id(psa_msg_t *msg)
{
    return CORE_TEST_ERRNO_TEST_NOT_SUPPORTED;
}

static psa_status_t tfm_core_test_sfn_wrap_spm_request(psa_msg_t *msg)
{
    return CORE_TEST_ERRNO_TEST_NOT_SUPPORTED;
}

static psa_status_t tfm_core_test_sfn_wrap_block(psa_msg_t *msg)
{
    return test_block();
}

static psa_status_t tfm_core_test_sfn_wrap_ns_thread(psa_msg_t *msg)
{
    return CORE_TEST_ERRNO_SUCCESS;
}

static void core_test_signal_handle(psa_signal_t signal, core_test_func_t pfn)
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
#endif /* !defined(TFM_PSA_API) */

psa_status_t core_test_init(void)
{
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;
#endif /* defined(TFM_PSA_API) */

    partition_init_done = 1;

#ifdef TFM_PSA_API
    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & SPM_CORE_TEST_INIT_SUCCESS_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_INIT_SUCCESS_SIGNAL,
                                    tfm_core_test_sfn_wrap_init_success);
        } else if (signals & SPM_CORE_TEST_DIRECT_RECURSION_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_DIRECT_RECURSION_SIGNAL,
                                    tfm_core_test_sfn_wrap_direct_recursion);
        } else if (signals & SPM_CORE_TEST_SS_TO_SS_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_SS_TO_SS_SIGNAL,
                                    tfm_core_test_sfn_wrap_ss_to_ss);
        } else if (signals & SPM_CORE_TEST_SS_TO_SS_BUFFER_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_SS_TO_SS_BUFFER_SIGNAL,
                                    tfm_core_test_sfn_wrap_ss_to_ss_buffer);
        } else if (signals & SPM_CORE_TEST_OUTVEC_WRITE_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_OUTVEC_WRITE_SIGNAL,
                                    tfm_core_test_sfn_wrap_outvec_write);
        } else if (signals & SPM_CORE_TEST_PERIPHERAL_ACCESS_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_PERIPHERAL_ACCESS_SIGNAL,
                                    tfm_core_test_sfn_wrap_peripheral_access);
        } else if (signals & SPM_CORE_TEST_GET_CALLER_CLIENT_ID_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_GET_CALLER_CLIENT_ID_SIGNAL,
                                   tfm_core_test_sfn_wrap_get_caller_client_id);
        } else if (signals & SPM_CORE_TEST_SPM_REQUEST_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_SPM_REQUEST_SIGNAL,
                                    tfm_core_test_sfn_wrap_spm_request);
        } else if (signals & SPM_CORE_TEST_BLOCK_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_BLOCK_SIGNAL,
                                    tfm_core_test_sfn_wrap_block);
        } else if (signals & SPM_CORE_TEST_NS_THREAD_SIGNAL) {
            core_test_signal_handle(SPM_CORE_TEST_NS_THREAD_SIGNAL,
                                    tfm_core_test_sfn_wrap_ns_thread);
        } else {
            ; /* do nothing */
        }
    }
#else
    return CORE_TEST_ERRNO_SUCCESS;
#endif /* defined(TFM_PSA_API) */
}
