// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/pk.h"

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/tlsio_mbedtls.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/threadapi.h"

static const char *const OPTION_UNDERLYING_IO_OPTIONS = "underlying_io_options";

#define HANDSHAKE_TIMEOUT_MS 5000
#define HANDSHAKE_WAIT_INTERVAL_MS 10

typedef enum TLSIO_STATE_ENUM_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPENING_UNDERLYING_IO,
    TLSIO_STATE_IN_HANDSHAKE,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_CLOSING,
    TLSIO_STATE_ERROR
} TLSIO_STATE_ENUM;

typedef struct SEND_COMPLETE_INFO_TAG
{
    bool is_fragmented_req;
    IO_SEND_RESULT last_fragmented_req_status;
    ON_SEND_COMPLETE on_send_complete;
    void *on_send_complete_callback_context;
} SEND_COMPLETE_INFO;

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE socket_io;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_IO_ERROR on_io_error;
    void *on_bytes_received_context;
    void *on_io_open_complete_context;
    void *on_io_close_complete_context;
    void *on_io_error_context;
    TLSIO_STATE_ENUM tlsio_state;
    unsigned char *socket_io_read_bytes;
    size_t socket_io_read_byte_count;
    SEND_COMPLETE_INFO send_complete_info;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config config;
    mbedtls_x509_crt trusted_certificates_parsed;
    mbedtls_ssl_session ssn;
    char *trusted_certificates;

    char *hostname;
    mbedtls_x509_crt owncert;
    mbedtls_pk_context pKey;

    char* x509_certificate;
    char* x509_private_key;

    int tls_status;
} TLS_IO_INSTANCE;

typedef enum TLS_STATE_TAG
{
    TLS_STATE_NOT_INITIALIZED,
    TLS_STATE_INITIALIZED,
    TLS_STATE_CLOSING,
} TLS_STATE;

static void indicate_error(TLS_IO_INSTANCE *tls_io_instance)
{
    if ((tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN) && (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR))
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        if (tls_io_instance->on_io_error != NULL)
        {
            tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
        }
    }
}

static void indicate_open_complete(TLS_IO_INSTANCE *tls_io_instance, IO_OPEN_RESULT open_result)
{
    if (tls_io_instance->on_io_open_complete != NULL)
    {
        tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, open_result);
    }
}

static int decode_ssl_received_bytes(TLS_IO_INSTANCE *tls_io_instance)
{
    int result = 0;
    unsigned char buffer[64];
    int rcv_bytes = 1;

    while (rcv_bytes > 0)
    {
        rcv_bytes = mbedtls_ssl_read(&tls_io_instance->ssl, buffer, sizeof(buffer));
        if (rcv_bytes > 0)
        {
            if (tls_io_instance->on_bytes_received != NULL)
            {
                tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, rcv_bytes);
            }
        }
    }

    return result;
}

static bool is_fragmented_send_request(TLS_IO_INSTANCE *tls_io_instance, size_t send_size)
{
#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    size_t max_len = mbedtls_ssl_get_max_frag_len(&tls_io_instance->ssl);
#else
    size_t max_len = MBEDTLS_SSL_MAX_CONTENT_LEN;
    (void)tls_io_instance;
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */
    bool result;

    if (send_size > max_len)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

static void on_underlying_io_open_complete(void *context, IO_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;
        int result = 0;

        if (open_result != IO_OPEN_OK)
        {
            xio_close(tls_io_instance->socket_io, NULL, NULL);
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
        }
        else
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_IN_HANDSHAKE;

            do
            {
                result = mbedtls_ssl_handshake(&tls_io_instance->ssl);
            } while (result == MBEDTLS_ERR_SSL_WANT_READ || result == MBEDTLS_ERR_SSL_WANT_WRITE);

            if (result == 0)
            {
                tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
                indicate_open_complete(tls_io_instance, IO_OPEN_OK);
            }
            else
            {
                if (result == MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED)
                {
                    LogError("Failure in ssl handshake has the server certificate been added?");
                }
                else
                {
                    LogError("Failure ssl handshake %d", result);
                }
                xio_close(tls_io_instance->socket_io, NULL, NULL);
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            }
        }
    }
}

static void on_underlying_io_bytes_received(void *context, const unsigned char *buffer, size_t size)
{
    if (context != NULL)
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        unsigned char *new_socket_io_read_bytes = (unsigned char *)realloc(tls_io_instance->socket_io_read_bytes, tls_io_instance->socket_io_read_byte_count + size);

        if (new_socket_io_read_bytes == NULL)
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
            indicate_error(tls_io_instance);
        }
        else
        {
            tls_io_instance->socket_io_read_bytes = new_socket_io_read_bytes;
            (void)memcpy(tls_io_instance->socket_io_read_bytes + tls_io_instance->socket_io_read_byte_count, buffer, size);
            tls_io_instance->socket_io_read_byte_count += size;
        }
    }
    else
    {
        LogError("NULL value passed in context");
    }
}

static void on_underlying_io_error(void *context)
{
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        switch (tls_io_instance->tlsio_state)
        {
        default:
        case TLSIO_STATE_NOT_OPEN:
        case TLSIO_STATE_ERROR:
            break;

        case TLSIO_STATE_OPENING_UNDERLYING_IO:
        case TLSIO_STATE_IN_HANDSHAKE:
            // Existing socket impls are all synchronous close, and this
            // adapter does not yet support async close.
            xio_close(tls_io_instance->socket_io, NULL, NULL);
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            break;

        case TLSIO_STATE_OPEN:
            indicate_error(tls_io_instance);
            break;
        }
    }
}

static void on_underlying_io_close_complete_during_close(void *context)
{
    if (context == NULL)
    {
        LogError("NULL value passed in context");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

        if (tls_io_instance->on_io_close_complete != NULL)
        {
            tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
        }
    }
}

static int on_io_recv(void *context, unsigned char *buf, size_t sz)
{
    int result;
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
        result = MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;
        unsigned char *new_socket_io_read_bytes;
        int pending = 0;

        while (tls_io_instance->socket_io_read_byte_count == 0)
        {
            xio_dowork(tls_io_instance->socket_io);

            if (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
            {
                break;
            }
            else if (tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN ||
                     tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING ||
                     tls_io_instance->tlsio_state == TLSIO_STATE_ERROR)
            {
                // Underlying io error, exit.
                return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
            }
            else
            {
                // Handkshake
                if (tls_io_instance->socket_io_read_byte_count == 0)
                {
                    if (pending++ >= HANDSHAKE_TIMEOUT_MS / HANDSHAKE_WAIT_INTERVAL_MS)
                    {
                        // The connection is close from server side and no response.
                        LogError("Tlsio_Failure: encountered unknow connection issue, the connection will be restarted.");
                        indicate_error(tls_io_instance);
                        return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
                    }
                    ThreadAPI_Sleep(HANDSHAKE_WAIT_INTERVAL_MS);
                }
            }
        }

        result = (int) tls_io_instance->socket_io_read_byte_count;
        if (result > (int)sz)
        {
            result = (int)sz;
        }

        if (result > 0)
        {
            (void)memcpy((void *)buf, tls_io_instance->socket_io_read_bytes, result);
            (void)memmove(tls_io_instance->socket_io_read_bytes, tls_io_instance->socket_io_read_bytes + result, tls_io_instance->socket_io_read_byte_count - result);
            tls_io_instance->socket_io_read_byte_count -= result;
            if (tls_io_instance->socket_io_read_byte_count > 0)
            {
                new_socket_io_read_bytes = (unsigned char *)realloc(tls_io_instance->socket_io_read_bytes, tls_io_instance->socket_io_read_byte_count);
                if (new_socket_io_read_bytes != NULL)
                {
                    tls_io_instance->socket_io_read_bytes = new_socket_io_read_bytes;
                }
            }
            else
            {
                free(tls_io_instance->socket_io_read_bytes);
                tls_io_instance->socket_io_read_bytes = NULL;
            }
        }

        if ((result == 0) && (tls_io_instance->tlsio_state == TLSIO_STATE_OPEN))
        {
            result = MBEDTLS_ERR_SSL_WANT_READ;
        }
    }
    return result;
}

static void on_send_complete(void* context, IO_SEND_RESULT send_result)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE *)context;
    if (tls_io_instance != NULL)
    {
        // update fragment status
        if (tls_io_instance->send_complete_info.is_fragmented_req)
        {
            tls_io_instance->send_complete_info.last_fragmented_req_status = send_result;
        }

        if (tls_io_instance->send_complete_info.on_send_complete != NULL &&
            tls_io_instance->tlsio_state != TLSIO_STATE_CLOSING)
        {
            // trigger callback always on failure, otherwise call it on last fragment completion
            if (send_result != IO_SEND_OK || !tls_io_instance->send_complete_info.is_fragmented_req)
            {
                void *ctx = tls_io_instance->send_complete_info.on_send_complete_callback_context;
                tls_io_instance->send_complete_info.on_send_complete(ctx, send_result);
            }
        }
    }
    else
    {
        LogError("Invalid context NULL value passed");
    }
}

static int on_io_send(void *context, const unsigned char *buf, size_t sz)
{
    int result;
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
        result = 0;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;
        ON_SEND_COMPLETE on_complete_callback = NULL;
        context = NULL;

        // Only allow Application data type message to send on_send_complete callback.
        if (tls_io_instance->ssl.out_msgtype == MBEDTLS_SSL_MSG_APPLICATION_DATA)
        {
            on_complete_callback = on_send_complete;
            context = tls_io_instance;
        }

        if (xio_send(tls_io_instance->socket_io, buf, sz, on_complete_callback, context) != 0)
        {
            indicate_error(tls_io_instance);
            result = 0;
        }
        else
        {
            result = (int)sz;
        }
    }
    return result;
}

static int tlsio_entropy_poll(void *v, unsigned char *output, size_t len, size_t *olen)
{
    (void)v;
    int result = 0;
    srand((unsigned int)time(NULL));
    for (uint16_t i = 0; i < len; i++)
    {
        output[i] = rand() % 256;
    }
    *olen = len;
    return result;
}

// Un-initialize mbedTLS
static void mbedtls_uninit(TLS_IO_INSTANCE *tls_io_instance)
{
    if (tls_io_instance->tls_status != TLS_STATE_NOT_INITIALIZED)
    {
        // mbedTLS cleanup...
        mbedtls_ssl_free(&tls_io_instance->ssl);
        mbedtls_ssl_session_free(&tls_io_instance->ssn);
        mbedtls_ssl_config_free(&tls_io_instance->config);
        mbedtls_x509_crt_free(&tls_io_instance->trusted_certificates_parsed);
        mbedtls_x509_crt_free(&tls_io_instance->owncert);
        mbedtls_pk_free(&tls_io_instance->pKey);
        mbedtls_ctr_drbg_free(&tls_io_instance->ctr_drbg);
        mbedtls_entropy_free(&tls_io_instance->entropy);

        tls_io_instance->tls_status = TLS_STATE_NOT_INITIALIZED;
    }
    else
    {
        LogError("Uninitialzing when not previously initialized");
    }
}

static void mbedtls_init(TLS_IO_INSTANCE *tls_io_instance)
{
    const char* pers = "azure_iot_client";
    if (tls_io_instance->tls_status != TLS_STATE_INITIALIZED)
    {
        if (tls_io_instance->tls_status == TLS_STATE_CLOSING)
        {
            // The underlying connection has been closed, so here un-initialize first
            mbedtls_uninit(tls_io_instance);
        }
        // mbedTLS initialize...
        mbedtls_x509_crt_init(&tls_io_instance->trusted_certificates_parsed);
        mbedtls_x509_crt_init(&tls_io_instance->owncert);
        mbedtls_pk_init(&tls_io_instance->pKey);

        mbedtls_entropy_init(&tls_io_instance->entropy);
        // Add a weak entropy source here,avoid some platform doesn't have strong / hardware entropy
        mbedtls_entropy_add_source(&tls_io_instance->entropy, tlsio_entropy_poll, NULL, MBEDTLS_ENTROPY_MAX_GATHER, MBEDTLS_ENTROPY_SOURCE_WEAK);

        mbedtls_ctr_drbg_init(&tls_io_instance->ctr_drbg);
        mbedtls_ctr_drbg_seed(&tls_io_instance->ctr_drbg, mbedtls_entropy_func, &tls_io_instance->entropy, (const unsigned char *)pers, strlen(pers));

        mbedtls_ssl_config_init(&tls_io_instance->config);
        mbedtls_ssl_config_defaults(&tls_io_instance->config, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
        mbedtls_ssl_conf_rng(&tls_io_instance->config, mbedtls_ctr_drbg_random, &tls_io_instance->ctr_drbg);
        mbedtls_ssl_conf_authmode(&tls_io_instance->config, MBEDTLS_SSL_VERIFY_REQUIRED);
        mbedtls_ssl_conf_min_version(&tls_io_instance->config, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3); // v1.2

        mbedtls_ssl_init(&tls_io_instance->ssl);
        mbedtls_ssl_set_bio(&tls_io_instance->ssl, tls_io_instance, on_io_send, on_io_recv, NULL);
        mbedtls_ssl_set_hostname(&tls_io_instance->ssl, tls_io_instance->hostname);

        mbedtls_ssl_session_init(&tls_io_instance->ssn);

        mbedtls_ssl_set_session(&tls_io_instance->ssl, &tls_io_instance->ssn);
        mbedtls_ssl_setup(&tls_io_instance->ssl, &tls_io_instance->config);

        tls_io_instance->tls_status = TLS_STATE_INITIALIZED;
    }
}

CONCRETE_IO_HANDLE tlsio_mbedtls_create(void *io_create_parameters)
{
    TLSIO_CONFIG *tls_io_config = (TLSIO_CONFIG *)io_create_parameters;
    TLS_IO_INSTANCE *result;

    if (tls_io_config == NULL)
    {
        LogError("NULL tls_io_config");
        result = NULL;
    }
    else
    {
        // Codes_SRS_TLSIO_MBED_OS5_TLS_99_006: [ The tlsio_mbedtls_create shall return NULL if allocating memory for TLS_IO_INSTANCE failed. ]
        result = calloc(1, sizeof(TLS_IO_INSTANCE));
        if (result != NULL)
        {
            SOCKETIO_CONFIG socketio_config;
            const IO_INTERFACE_DESCRIPTION *underlying_io_interface;
            void *io_interface_parameters;

            if (tls_io_config->underlying_io_interface != NULL)
            {
                underlying_io_interface = tls_io_config->underlying_io_interface;
                io_interface_parameters = tls_io_config->underlying_io_parameters;
            }
            else
            {
                socketio_config.hostname = tls_io_config->hostname;
                socketio_config.port = tls_io_config->port;
                socketio_config.accepted_socket = NULL;
                underlying_io_interface = socketio_get_interface_description();
                io_interface_parameters = &socketio_config;
            }

            if (underlying_io_interface == NULL)
            {
                free(result);
                result = NULL;
                LogError("Failed getting socket IO interface description.");
            }
            else
            {
                if (mallocAndStrcpy_s((char **)&result->hostname, tls_io_config->hostname) != 0)
                {
                    free(result);
                    result = NULL;
                    LogError("Failure allocating hostname.");
                }
                else if ((result->socket_io = xio_create(underlying_io_interface, io_interface_parameters)) == NULL)
                {
                    LogError("socket xio create failed");
                    free(result->hostname);
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->tls_status = TLS_STATE_NOT_INITIALIZED;
                    mbedtls_init((void*)result);
                    result->tlsio_state = TLSIO_STATE_NOT_OPEN;
                }
            }
        }
        else
        {
            LogError("Failure allocating TLS object");
        }
    }
    return result;
}

void tlsio_mbedtls_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        mbedtls_uninit(tls_io_instance);

        xio_close(tls_io_instance->socket_io, NULL, NULL);

        if (tls_io_instance->socket_io_read_bytes != NULL)
        {
            free(tls_io_instance->socket_io_read_bytes);
            tls_io_instance->socket_io_read_bytes = NULL;
        }
        xio_destroy(tls_io_instance->socket_io);
        if (tls_io_instance->hostname != NULL)
        {
            free(tls_io_instance->hostname);
            tls_io_instance->hostname = NULL;
        }
        if (tls_io_instance->trusted_certificates != NULL)
        {
            free(tls_io_instance->trusted_certificates);
            tls_io_instance->trusted_certificates = NULL;
        }
        if (tls_io_instance->x509_certificate != NULL)
        {
            free(tls_io_instance->x509_certificate);
            tls_io_instance->x509_certificate = NULL;
        }
        if (tls_io_instance->x509_private_key != NULL)
        {
            free(tls_io_instance->x509_private_key);
            tls_io_instance->x509_private_key = NULL;
        }
        free(tls_io);
    }
}

int tlsio_mbedtls_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void *on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void *on_bytes_received_context, ON_IO_ERROR on_io_error, void *on_io_error_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
        LogError("Invalid parameter specified tls_io: NULL");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            LogError("IO should not be open: %d", tls_io_instance->tlsio_state);
            result = MU_FAILURE;
        }
        else
        {

            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_open_complete = on_io_open_complete;
            tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

            mbedtls_ssl_session_reset(&tls_io_instance->ssl);

            if (xio_open(tls_io_instance->socket_io, on_underlying_io_open_complete, tls_io_instance, on_underlying_io_bytes_received, tls_io_instance, on_underlying_io_error, tls_io_instance) != 0)
            {

                LogError("Underlying IO open failed");
                tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                result = MU_FAILURE;
            }
        }
    }
    return result;
}

int tlsio_mbedtls_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
        LogError("Invalid parameter specified tls_io: NULL");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if ((tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN) ||
            (tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING))
        {
            LogError("IO should not be closed: %d", tls_io_instance->tlsio_state);
            result = MU_FAILURE;
        }
        else
        {
            int is_error = tls_io_instance->tlsio_state == TLSIO_STATE_ERROR;
            tls_io_instance->tlsio_state = TLSIO_STATE_CLOSING;

            if (tls_io_instance->tls_status == TLS_STATE_INITIALIZED)
            {
                if (is_error)
                {
                    // forced shutdown if tls is in ERROR state
                    mbedtls_ssl_session_reset(&tls_io_instance->ssl);
                }
                else
                {
                    // Tell the peer that you're going to close
                    mbedtls_ssl_close_notify(&tls_io_instance->ssl);
                }

                tls_io_instance->tls_status = TLS_STATE_CLOSING;
            }

            tls_io_instance->on_io_close_complete = on_io_close_complete;
            tls_io_instance->on_io_close_complete_context = callback_context;
            if (xio_close(tls_io_instance->socket_io, on_underlying_io_close_complete_during_close, tls_io_instance) != 0)
            {
                LogError("xio_close failed");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

int tlsio_mbedtls_send(CONCRETE_IO_HANDLE tls_io, const void *buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void *callback_context)
{
    int result = 0;

    if (tls_io == NULL || (buffer == NULL) || (size == 0))
    {
        LogError("Invalid parameter specified tls_io: %p, buffer: %p, size: %ul", tls_io, buffer, (unsigned int)size);
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            LogError("Invalid state specified %d", tls_io_instance->tlsio_state);
            result = MU_FAILURE;
        }
        else
        {
            tls_io_instance->send_complete_info.on_send_complete = on_send_complete;
            tls_io_instance->send_complete_info.on_send_complete_callback_context = callback_context;
            tls_io_instance->send_complete_info.last_fragmented_req_status = IO_SEND_OK;
            int out_left = (int)size;
            result = 0;

            do
            {
                tls_io_instance->send_complete_info.is_fragmented_req = is_fragmented_send_request(tls_io_instance, out_left);
                unsigned char *buf = (unsigned char *)buffer + size - out_left;
                int ret = mbedtls_ssl_write(&tls_io_instance->ssl, buf, out_left);

                if (ret < 0)
                {
                    LogError("Unexpected data size returned from  mbedtls_ssl_write %d/%d", ret, (int)size);
                    result = MU_FAILURE;
                    break;
                }
                else if (tls_io_instance->send_complete_info.last_fragmented_req_status != IO_SEND_OK)
                {
                    LogError("Failed to send last fragment with error:0x%0x, aborting whole send",
                             tls_io_instance->send_complete_info.last_fragmented_req_status);
                    result = MU_FAILURE;
                    break;
                }

                out_left -= ret;
            } while (out_left > 0);

            // remove on send complete info
            memset((void*)&tls_io_instance->send_complete_info, 0, sizeof(tls_io_instance->send_complete_info));
        }
    }

    return result;
}

void tlsio_mbedtls_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;
        if (tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE || tls_io_instance->tlsio_state == TLSIO_STATE_OPEN)
        {
            decode_ssl_received_bytes(tls_io_instance);
        }

        xio_dowork(tls_io_instance->socket_io);
    }
}

// this function will clone an option given by name and value
static void *tlsio_mbedtls_CloneOption(const char *name, const void *value)
{
    void *result = NULL;
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            result = (void *)value;
        }
        else if (strcmp(name, OPTION_TRUSTED_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char **)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s TrustedCerts value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, SU_OPTION_X509_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509certificate value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509privatekey value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccCertificate value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccKey value");
                result = NULL;
            }
            else
            {
                /*return as is*/
            }
        }
        else
        {
            LogError("not handled option : %s", name);
            result = NULL;
        }
    }
    return result;
}

// this function destroys an option previously created
static void tlsio_mbedtls_DestroyOption(const char *name, const void *value)
{
    // since all options for this layer are actually string copies., disposing of one is just calling free
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (
            (strcmp(name, OPTION_TRUSTED_CERT) == 0) ||
            (strcmp(name, SU_OPTION_X509_CERT) == 0) ||
            (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0) ||
            (strcmp(name, OPTION_X509_ECC_CERT) == 0) ||
            (strcmp(name, OPTION_X509_ECC_KEY) == 0)
            )
        {
            free((void*)value);
        }
        else if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            LogError("not handled option : %s", name);
        }
    }
}

int tlsio_mbedtls_setoption(CONCRETE_IO_HANDLE tls_io, const char *optionName, const void *value)
{
    int result = 0;

    if (tls_io == NULL || optionName == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            if (tls_io_instance->trusted_certificates != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->trusted_certificates);
                tls_io_instance->trusted_certificates = NULL;
            }
            if (mallocAndStrcpy_s(&tls_io_instance->trusted_certificates, (const char *)value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s");
                result = MU_FAILURE;
            }
            else
            {
                int parse_result = mbedtls_x509_crt_parse(&tls_io_instance->trusted_certificates_parsed, (const unsigned char *)value, (int)(strlen(value) + 1));
                if (parse_result != 0)
                {
                    LogInfo("Malformed pem certificate");
                    free(tls_io_instance->trusted_certificates);
                    tls_io_instance->trusted_certificates = NULL;
                    result = MU_FAILURE;
                }
                else
                {
                    mbedtls_ssl_conf_ca_chain(&tls_io_instance->config, &tls_io_instance->trusted_certificates_parsed, NULL);
                }
            }
        }
        else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
        {
            char* temp_cert;
            if (mallocAndStrcpy_s(&temp_cert, (const char *)value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s on certificate");
                result = MU_FAILURE;
            }
            else if (mbedtls_x509_crt_parse(&tls_io_instance->owncert, (const unsigned char *)temp_cert, (int)(strlen(temp_cert) + 1)) != 0)
            {
                LogError("failure parsing certificate");
                free(temp_cert);
                result = MU_FAILURE;
            }
            else if (mbedtls_pk_get_type(&tls_io_instance->pKey) != MBEDTLS_PK_NONE &&
                     mbedtls_ssl_conf_own_cert(&tls_io_instance->config, &tls_io_instance->owncert, &tls_io_instance->pKey) != 0)
            {
                LogError("failure calling mbedtls_ssl_conf_own_cert");
                free(temp_cert);
                result = MU_FAILURE;
            }
            else
            {
                if (tls_io_instance->x509_certificate != NULL)
                {
                    // Free the memory if it has been previously allocated
                    free(tls_io_instance->x509_certificate);
                }
                tls_io_instance->x509_certificate = temp_cert;
                result = 0;
            }
        }
        else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
        {
            char* temp_key;

            if (mallocAndStrcpy_s(&temp_key, (const char *)value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s on private key");
                result = MU_FAILURE;
            }
            else if (mbedtls_pk_parse_key(&tls_io_instance->pKey, (const unsigned char *)temp_key, (int)(strlen(temp_key) + 1), NULL, 0) != 0)
            {
                LogError("failure calling mbedtls_pk_parse_key");
                free(temp_key);
                result = MU_FAILURE;
            }
            else if (tls_io_instance->owncert.version > 0 && mbedtls_ssl_conf_own_cert(&tls_io_instance->config, &tls_io_instance->owncert, &tls_io_instance->pKey))
            {
                LogError("failure calling mbedtls_ssl_conf_own_cert");
                free(temp_key);
                result = MU_FAILURE;
            }
            else
            {
                if (tls_io_instance->x509_private_key != NULL)
                {
                    // Free the memory if it has been previously allocated
                    free(tls_io_instance->x509_private_key);
                }
                tls_io_instance->x509_private_key = temp_key;
                result = 0;
            }
        }
        else if (strcmp(optionName, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, (void*)tls_io_instance->socket_io) != OPTIONHANDLER_OK)
            {
                LogError("failed feeding options to underlying I/O instance");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else if (strcmp(optionName, OPTION_SET_TLS_RENEGOTIATION) == 0)
        {
            if (value == NULL)
            {
                LogError("Invalid value set for tls renegotiation");
                result = MU_FAILURE;
            }
            else
            {
                bool set_renegotiation = *((bool*)(value));
                mbedtls_ssl_conf_renegotiation(&tls_io_instance->config, set_renegotiation ? 1 : 0);
                result = 0;
            }
        }
        else
        {
            // tls_io_instance->socket_io is never NULL
            result = xio_setoption(tls_io_instance->socket_io, optionName, value);
        }
    }
    return result;
}

OPTIONHANDLER_HANDLE tlsio_mbedtls_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result = NULL;
    if (handle == NULL)
    {
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle);
        result = NULL;
    }
    else
    {
        result = OptionHandler_Create(tlsio_mbedtls_CloneOption, tlsio_mbedtls_DestroyOption, tlsio_mbedtls_setoption);
        if (result == NULL)
        {
            LogError("unable to OptionHandler_Create");
            // return as is
        }
        else
        {
            // this layer cares about the certificates
            TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)handle;
            OPTIONHANDLER_HANDLE underlying_io_options;

            if ((underlying_io_options = xio_retrieveoptions(tls_io_instance->socket_io)) == NULL ||
                OptionHandler_AddOption(result, OPTION_UNDERLYING_IO_OPTIONS, underlying_io_options) != OPTIONHANDLER_OK)
            {
                LogError("unable to save underlying_io options");
                OptionHandler_Destroy(underlying_io_options);
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->trusted_certificates != NULL &&
                     OptionHandler_AddOption(result, OPTION_TRUSTED_CERT, tls_io_instance->trusted_certificates) != OPTIONHANDLER_OK)
            {
                LogError("unable to save TrustedCerts option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_certificate != NULL &&
                     OptionHandler_AddOption(result, SU_OPTION_X509_CERT, tls_io_instance->x509_certificate) != OPTIONHANDLER_OK)
            {
                LogError("unable to save x509certificate option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_private_key != NULL &&
                     OptionHandler_AddOption(result, SU_OPTION_X509_PRIVATE_KEY, tls_io_instance->x509_private_key) != OPTIONHANDLER_OK)
            {
                LogError("unable to save x509privatekey option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                // all is fine, all interesting options have been saved
                // return as is
            }
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION tlsio_mbedtls_interface_description =
{
    tlsio_mbedtls_retrieveoptions,
    tlsio_mbedtls_create,
    tlsio_mbedtls_destroy,
    tlsio_mbedtls_open,
    tlsio_mbedtls_close,
    tlsio_mbedtls_send,
    tlsio_mbedtls_dowork,
    tlsio_mbedtls_setoption
};

const IO_INTERFACE_DESCRIPTION *tlsio_mbedtls_get_interface_description(void)
{
    return &tlsio_mbedtls_interface_description;
}
