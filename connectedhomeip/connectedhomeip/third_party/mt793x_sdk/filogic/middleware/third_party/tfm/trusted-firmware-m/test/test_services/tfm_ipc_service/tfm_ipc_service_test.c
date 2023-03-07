/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "psa/client.h"
#include "psa/service.h"
#include "secure_utilities.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "psa_manifest/tfm_ipc_service_partition.h"

#define IPC_SERVICE_BUFFER_LEN                          32

/* Define the whether the service is inuse flag. */
static uint32_t service_in_use = 0;

/* Define the global variable for the IPC_APP_ACCESS_PSA_MEM_SID service. */
uint8_t ipc_servic_data;
uint8_t *ipc_service_data_p = &ipc_servic_data;

/*
 * Fixme: Temporarily implement abort as infinite loop,
 * will replace it later.
 */
static void tfm_abort(void)
{
    while (1)
        ;
}

static void ipc_service_basic(void)
{
    psa_msg_t msg;
    psa_status_t r;
    int i;
    uint8_t rec_buf[IPC_SERVICE_BUFFER_LEN];
    uint8_t send_buf[IPC_SERVICE_BUFFER_LEN] = "It is just for IPC call test.";

    psa_get(IPC_SERVICE_TEST_BASIC_SIGNAL, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        if (service_in_use & IPC_SERVICE_TEST_BASIC_SIGNAL) {
            r = PSA_ERROR_CONNECTION_REFUSED;
        } else {
            service_in_use |= IPC_SERVICE_TEST_BASIC_SIGNAL;
            r = PSA_SUCCESS;
        }
        psa_reply(msg.handle, r);
        break;
    case PSA_IPC_CALL:
        for (i = 0; i < PSA_MAX_IOVEC; i++) {
            if (msg.in_size[i] != 0) {
                psa_read(msg.handle, i, rec_buf, IPC_SERVICE_BUFFER_LEN);
            }
            if (msg.out_size[i] != 0) {
                psa_write(msg.handle, i, send_buf, IPC_SERVICE_BUFFER_LEN);
            }
        }
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_DISCONNECT:
        assert((service_in_use & IPC_SERVICE_TEST_BASIC_SIGNAL) != 0);
        service_in_use &= ~IPC_SERVICE_TEST_BASIC_SIGNAL;
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* cannot get here? [broken SPM]. TODO*/
        tfm_abort();
        break;
    }
}

static void ipc_service_psa_access_app_mem(void)
{
    psa_msg_t msg;
    psa_status_t r;
    char rec_data;
    uint32_t rec_buf;

    psa_get(IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        if (service_in_use & IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL) {
            r = PSA_ERROR_CONNECTION_REFUSED;
        } else {
            service_in_use |= IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL;
            r = PSA_SUCCESS;
        }
        psa_reply(msg.handle, r);
        break;
    case PSA_IPC_CALL:
        /*
         * rec_buf is a pointer pointed to a char type memory in client stack.
         */
        if (msg.in_size[0] != 0) {
            psa_read(msg.handle, 0, &rec_buf, 4);
            rec_data = *(char *)rec_buf;

            /* rec_data is assigned to 'A' by the client side. */
            if (rec_data != 'A') {
                psa_reply(msg.handle, -1);
                break;
            }

            /* Change the char type client stack memory to 'B'. */
            *((char *)rec_buf) = 'B';
        }
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_DISCONNECT:
        assert((service_in_use & IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL)
               != 0);
        service_in_use &= ~IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL;
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* cannot get here? [broken SPM]. TODO*/
        tfm_abort();
        break;
    }
}

#ifdef TFM_IPC_ISOLATION_2_TEST_READ_ONLY_MEM
static void ipc_service_psa_access_app_readonly_mem(void)
{
    psa_msg_t msg;
    psa_status_t r;
    char rec_data;
    uint32_t rec_buf;

    psa_get(IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        if (service_in_use &
            IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL) {
            r = PSA_ERROR_CONNECTION_REFUSED;
        } else {
            service_in_use |=
                          IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL;
            r = PSA_SUCCESS;
        }
        psa_reply(msg.handle, r);
        break;
    case PSA_IPC_CALL:
         /*
          * rec_buf is a pointer pointed to a char type memory in client
          * code section. Write the memory will cause MemManage fault.
          */
        if (msg.in_size[0] != 0) {
            psa_read(msg.handle, 0, &rec_buf, 4);
            rec_data = *(char *)rec_buf;

            /* rec_data is assigned to 'A' by the client side. */
            if (rec_data != 'A') {
                psa_reply(msg.handle, -1);
                break;
            }

            /* Write the char type read only memory. */
            *((char *)rec_buf) = 'B';
        }

        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_DISCONNECT:
        assert((service_in_use &
                IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL) != 0);
        service_in_use &=
                         ~IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL;
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* cannot get here? [broken SPM]. TODO*/
        tfm_abort();
        break;
    }
}
#endif

#if defined TFM_IPC_ISOLATION_2_MEM_CHECK \
    || defined TFM_IPC_ISOLATION_2_APP_ACCESS_PSA_MEM
static void ipc_service_app_access_psa_mem(void)
{
    psa_msg_t msg;
    psa_status_t r;

    psa_get(IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        if (service_in_use & IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL) {
            r = PSA_ERROR_CONNECTION_REFUSED;
        } else {
            service_in_use |= IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL;
            r = PSA_SUCCESS;
        }

        psa_reply(msg.handle, r);
        break;
    case PSA_IPC_CALL:
        if (msg.out_size[0] != 0) {

            /*
             * Write a pointer to outvec. The pointer points to uint8_t
             * memory in ipc servive partition.
             */
            psa_write(msg.handle, 0, &ipc_service_data_p,
                      sizeof(ipc_service_data_p));
        }

        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_DISCONNECT:
        assert((service_in_use & IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL)
               != 0);
        service_in_use &= ~IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL;
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* cannot get here? [broken SPM]. TODO*/
        tfm_abort();
        break;
    }
}
#endif

static void ipc_service_programmer_error(void)
{
    psa_msg_t msg;
    psa_status_t r;

    psa_get(IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        if (service_in_use & IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL) {
            r = PSA_ERROR_CONNECTION_REFUSED;
        } else {
            service_in_use |= IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL;
            r = PSA_SUCCESS;
        }
        psa_reply(msg.handle, r);
        break;
    case PSA_IPC_CALL:
        psa_reply(msg.handle, PSA_ERROR_PROGRAMMER_ERROR);
        break;
    case PSA_IPC_DISCONNECT:
        assert((service_in_use
                & IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL) != 0);
        service_in_use &= ~IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL;
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* cannot get here? [broken SPM]. TODO*/
        tfm_abort();
        break;
    }
}

/* Test thread */
void ipc_service_test_main(void *param)
{
    uint32_t signals = 0;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & IPC_SERVICE_TEST_BASIC_SIGNAL) {
            ipc_service_basic();
        } else if (signals & IPC_SERVICE_TEST_PSA_ACCESS_APP_MEM_SIGNAL) {
            ipc_service_psa_access_app_mem();
#ifdef TFM_IPC_ISOLATION_2_TEST_READ_ONLY_MEM
        } else if (signals
                   & IPC_SERVICE_TEST_PSA_ACCESS_APP_READ_ONLY_MEM_SIGNAL) {
            ipc_service_psa_access_app_readonly_mem();
#endif
#if defined TFM_IPC_ISOLATION_2_MEM_CHECK \
    || defined TFM_IPC_ISOLATION_2_APP_ACCESS_PSA_MEM
        } else if (signals & IPC_SERVICE_TEST_APP_ACCESS_PSA_MEM_SIGNAL) {
            ipc_service_app_access_psa_mem();
#endif
        } else if (signals & IPC_SERVICE_TEST_CLIENT_PROGRAMMER_ERROR_SIGNAL) {
            ipc_service_programmer_error();
        } else {
            /* Should not come here */
            tfm_abort();
        }
    }
}
