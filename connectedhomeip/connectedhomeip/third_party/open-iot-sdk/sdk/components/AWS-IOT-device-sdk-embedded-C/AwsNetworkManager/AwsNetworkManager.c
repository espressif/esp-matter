/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AwsNetworkManager/AwsNetworkManager.h"

#include "backoff_algorithm.h"
#include "iot_socket.h"

#include <inttypes.h>

#define PW_LOG_MODULE_NAME "AWSN"
#include "pw_log/log.h"

static int AwsNetwork_configure_socket(NetworkContext_t *ctx);
static int AwsNetwork_configure_tls_contexts(NetworkContext_t *ctx);
static int AwsNetwork_connect_server(NetworkContext_t *ctx);
static int AwsNetwork_connect_tls(NetworkContext_t *ctx);
static int iot_socket_to_mbedlts_error(int32_t error_code);
static int mbedtls_to_mqtt_error(int32_t error_code);
static int iot_socket_send_bio(void *bio, const unsigned char *buf, size_t len);
static int iot_socket_recv_bio(void *bio, unsigned char *buf, size_t len);

void AwsNetwork_init(NetworkContext_t *ctx, const AwsNetworkContextConfig_t *config)
{
    ctx->config = config;
    ctx->socket_fd = IOT_SOCKET_ESOCK;
    mbedtls_entropy_init(&ctx->entropy);
    mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
    mbedtls_x509_crt_init(&ctx->ca_cert);
    mbedtls_x509_crt_init(&ctx->client_cert);
    mbedtls_pk_init(&ctx->pkey);
    mbedtls_ssl_init(&ctx->ssl);
    mbedtls_ssl_config_init(&ctx->ssl_conf);
}

void AwsNetwork_close(NetworkContext_t *ctx)
{
    // If the TLS connection is open, inform the server of its termination.
    if (ctx->socket_fd != IOT_SOCKET_ESOCK) {
        int err = mbedtls_ssl_close_notify(&ctx->ssl);
        if (err) {
            PW_LOG_ERROR("Failed to notify server of close: %d", err);
        }
    }

    // Free TLS data structures
    mbedtls_entropy_free(&ctx->entropy);
    mbedtls_ctr_drbg_free(&ctx->ctr_drbg);
    mbedtls_x509_crt_free(&ctx->ca_cert);
    mbedtls_x509_crt_free(&ctx->client_cert);
    mbedtls_pk_free(&ctx->pkey);
    mbedtls_ssl_free(&ctx->ssl);
    mbedtls_ssl_config_free(&ctx->ssl_conf);

    // Close the TCP socket
    if (ctx->socket_fd != IOT_SOCKET_ESOCK) {
        int err = iotSocketClose(ctx->socket_fd);
        if (err) {
            PW_LOG_ERROR("Failed to close TCP socket: %d", err);
        }
    }

    // Reset the data structure.
    memset(ctx, 0, sizeof(*ctx));
    ctx->socket_fd = IOT_SOCKET_ESOCK;
}

int AwsNetwork_connect(NetworkContext_t *ctx, AwsNetwork_connect_hook connect_cb)
{
    BackoffAlgorithmContext_t reconnect_config;
    BackoffAlgorithm_InitializeParams(
        &reconnect_config, ctx->config->backoff_base, ctx->config->backoff_max_delay, ctx->config->retry_max_attempts);
    const AwsNetworkContextConfig_t *config = ctx->config;
    BackoffAlgorithmStatus_t backoff_status = BackoffAlgorithmSuccess;

    // Configure the TLS context outside the connection loop
    int err = AwsNetwork_configure_tls_contexts(ctx);
    if (err) {
        PW_LOG_ERROR("Failed to initialize the TLS data structure, abort connection");
        return -1;
    }

    do {
        err = AwsNetwork_configure_socket(ctx);
        if (err) {
            goto handle_socket_error;
        }

        err = AwsNetwork_connect_server(ctx);
        if (err) {
            goto handle_socket_error;
        }

        err = AwsNetwork_connect_tls(ctx);
        if (err) {
            goto handle_tls_error;
        }

        // Finish with the hook
        if (connect_cb) {
            err = connect_cb(ctx);
            if (err) {
                goto handle_hook_error;
            }
        }

        // Everything went fine, return
        return 0;

    handle_hook_error:
        if (ctx->socket_fd != IOT_SOCKET_ESOCK) {
            err = mbedtls_ssl_close_notify(&ctx->ssl);
            if (err) {
                PW_LOG_WARN("Failed to notify server of close: %d", err);
                // This could happen if the server has closed the connection
            }
        }

    handle_tls_error:
        err = mbedtls_ssl_session_reset(&ctx->ssl);
        if (err) {
            PW_LOG_ERROR("Failed to reset TLS session: %d", err);
            // Reset the whole network and exit
            AwsNetwork_close(ctx);
            AwsNetwork_init(ctx, config);
            return -1;
        }

    handle_socket_error:
        // Close the TCP socket
        if (ctx->socket_fd != IOT_SOCKET_ESOCK) {
            err = iotSocketClose(ctx->socket_fd);
            if (err) {
                PW_LOG_ERROR("Failed to close TCP socket: %d", err);
            }
            ctx->socket_fd = IOT_SOCKET_ESOCK;
        }

        uint32_t random_number;
        err = mbedtls_ctr_drbg_random((void *)&ctx->ctr_drbg, (unsigned char *)&random_number, sizeof(random_number));
        if (err) {
            PW_LOG_ERROR("Failed to generate random number for backoff algorithm: %d", err);
            return -1;
        }

        uint16_t next_retry_backoff_ms = 0;
        backoff_status = BackoffAlgorithm_GetNextBackoff(&reconnect_config, random_number, &next_retry_backoff_ms);
        if (backoff_status == BackoffAlgorithmRetriesExhausted) {
            PW_LOG_ERROR("Connection to failed, all attempts exhausted.");
        } else if (backoff_status == BackoffAlgorithmSuccess) {
            PW_LOG_WARN("Connection failed. Retrying connection after %hu ms backoff.",
                        (unsigned short)next_retry_backoff_ms);

            // convert ms to tick
            uint32_t next_retry_backoff_tick = ((next_retry_backoff_ms * osKernelGetTickFreq()) / 1000);
            // round up
            if (((next_retry_backoff_ms * osKernelGetTickFreq()) % 1000)) {
                next_retry_backoff_tick += 1;
            }
            osDelay(next_retry_backoff_tick);
        }
    } while (backoff_status == BackoffAlgorithmSuccess);

    return -1;
}

static int AwsNetwork_configure_socket(NetworkContext_t *ctx)
{
    int res = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    if (res < 0) {
        PW_LOG_ERROR("iotSocketCreate failed: %d", res);
        return res;
    }

    ctx->socket_fd = res;

    res = iotSocketSetOpt(
        ctx->socket_fd, IOT_SOCKET_SO_RCVTIMEO, &ctx->config->receive_timeout, sizeof(ctx->config->receive_timeout));
    if (res) {
        PW_LOG_ERROR("Failed to set socket receive timeout: %d", res);
        goto release_socket;
    }

    iotSocketSetOpt(
        ctx->socket_fd, IOT_SOCKET_SO_SNDTIMEO, &ctx->config->send_timeout, sizeof(ctx->config->send_timeout));
    if (res) {
        PW_LOG_ERROR("Failed to set socket send timeout: %d", res);
        goto release_socket;
    }

    return 0;

release_socket:
    iotSocketClose(ctx->socket_fd);
    ctx->socket_fd = IOT_SOCKET_ESOCK;
    return res;
}

static int AwsNetwork_configure_tls_contexts(NetworkContext_t *ctx)
{
    int res = mbedtls_ctr_drbg_seed(
        &ctx->ctr_drbg, mbedtls_entropy_func, &ctx->entropy, ctx->config->drbg, ctx->config->drbg_len);
    if (res != 0) {
        PW_LOG_ERROR("mbedtls_ctr_drbg_seed() failed -0x%04X", -res);
        return res;
    }

    res = mbedtls_x509_crt_parse(&ctx->ca_cert,
                                 (const unsigned char *)ctx->config->server_certificate,
                                 strlen(ctx->config->server_certificate) + 1);
    if (res != 0) {
        PW_LOG_ERROR("mbedtls_x509_crt_parse() failed -0x%04X", -res);
        return res;
    }

    res = mbedtls_x509_crt_parse(&ctx->client_cert,
                                 (const unsigned char *)ctx->config->client_certificate,
                                 strlen(ctx->config->client_certificate) + 1);
    if (res != 0) {
        PW_LOG_ERROR("mbedtls_x509_crt_parse() failed -0x%04X", -res);
        return res;
    }

    res = mbedtls_ssl_config_defaults(
        &ctx->ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (res != 0) {
        PW_LOG_ERROR("mbedtls_ssl_config_defaults() failed -0x%04X", -res);
        return res;
    }

    res = mbedtls_pk_parse_key(&ctx->pkey,
                               (const unsigned char *)ctx->config->client_private_key,
                               strlen(ctx->config->client_private_key) + 1,
                               NULL,
                               0);
    if (res != 0) {
        PW_LOG_ERROR("mbedtls_pk_parse_key returned: %d", res);
        return res;
    }

    // link client certificate with private key
    if ((res = mbedtls_ssl_conf_own_cert(&ctx->ssl_conf, &ctx->client_cert, &ctx->pkey)) != 0) {
        PW_LOG_ERROR("mbedtls_ssl_conf_own_cert returned: %d", res);
        return res;
    }

    mbedtls_ssl_conf_authmode(&ctx->ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&ctx->ssl_conf, &ctx->ca_cert, NULL);
    mbedtls_ssl_conf_rng(&ctx->ssl_conf, mbedtls_ctr_drbg_random, &ctx->ctr_drbg);

    if ((res = mbedtls_ssl_setup(&ctx->ssl, &ctx->ssl_conf)) != 0) {
        PW_LOG_ERROR("mbedtls_ssl_setup() failed -0x%04X", -res);
        return res;
    }

    if ((res = mbedtls_ssl_set_hostname(&ctx->ssl, ctx->config->server_name)) != 0) {
        PW_LOG_ERROR("mbedtls_ssl_set_hostname() failed -0x%04X", -res);
        return res;
    }

    mbedtls_ssl_set_bio(&ctx->ssl, ctx, iot_socket_send_bio, iot_socket_recv_bio, NULL);

    return 0;
}

static int AwsNetwork_connect_server(NetworkContext_t *ctx)
{
    uint8_t ip[4] = {0};
    uint32_t ip_len = sizeof(ip);
    int res = iotSocketGetHostByName((const char *)ctx->config->server_name, IOT_SOCKET_AF_INET, ip, &ip_len);
    if (res < 0) {
        PW_LOG_ERROR("iotSocketGetHostByName returned %d", res);
        return res;
    }

    PW_LOG_INFO(
        "%s %d.%d.%d.%d:%d connecting...", ctx->config->server_name, ip[0], ip[1], ip[2], ip[3], ctx->config->port);

    res = iotSocketConnect(ctx->socket_fd, (const uint8_t *)&ip, ip_len, ctx->config->port);
    if (res) {
        PW_LOG_ERROR("iotSocketConnect failed: %d", res);
        return res;
    }

    PW_LOG_INFO("Successful connection to the server");

    return res;
}

int AwsNetwork_connect_tls(NetworkContext_t *ctx)
{
    int res = -1;

    PW_LOG_INFO("Performing the TLS handshake...");
    do {
        res = mbedtls_ssl_handshake(&ctx->ssl);
    } while (res != 0 && (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE));
    if (res < 0) {
        PW_LOG_ERROR("mbedtls_ssl_handshake() failed -0x%04X", -res);
        return res;
    }

    PW_LOG_INFO("Successfully completed the TLS handshake");
    PW_LOG_INFO("Verifying certificate...");

    uint32_t flags = mbedtls_ssl_get_verify_result(&ctx->ssl);
    if (flags != 0) {
        char gp_buf[1024];

        res = mbedtls_x509_crt_verify_info(gp_buf, sizeof(gp_buf), "\r  ! ", flags);
        if (res < 0) {
            PW_LOG_ERROR("mbedtls_x509_crt_verify_info() failed -0x%04X", -res);

            return res;
        } else {
            PW_LOG_ERROR("Certificate verification failed - flags: %" PRIu32, flags);
            PW_LOG_ERROR("%s", gp_buf);
            return flags;
        }
    } else {
        PW_LOG_INFO("Certificate verification passed");
    }

    PW_LOG_INFO("Established TLS connection");

    return 0;
}

int AwsNetwork_send(NetworkContext_t *ctx, const void *buf, size_t len)
{
    if (!ctx) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = mbedtls_ssl_write(&ctx->ssl, (const unsigned char *)buf, len);

    if (res < 0) {
        if (res != MBEDTLS_ERR_SSL_WANT_WRITE) {
            PW_LOG_ERROR("mbedtls_ssl_write returned : %d", res);
        }
        return mbedtls_to_mqtt_error(res);
    }

    return res;
}

int AwsNetwork_recv(NetworkContext_t *ctx, void *buf, size_t len)
{
    if (!ctx) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = mbedtls_ssl_read(&ctx->ssl, (unsigned char *)buf, len);
    if (res < 0) {
        if (res != MBEDTLS_ERR_SSL_WANT_READ) {
            PW_LOG_ERROR("AwsNetwork_recv returned : %d", res);
        }
        return mbedtls_to_mqtt_error(res);
    }

    return res;
}

static int iot_socket_to_mbedlts_error(int32_t error_code)
{
    switch (error_code) {
        case IOT_SOCKET_EAGAIN:
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        default:
            return MBEDTLS_ERR_SSL_INTERNAL_ERROR;
    }
}

static int mbedtls_to_mqtt_error(int32_t error_code)
{
    switch (error_code) {
        case MBEDTLS_ERR_SSL_WANT_WRITE:
        case MBEDTLS_ERR_SSL_WANT_READ:
            return 0;
        default:
            PW_LOG_ERROR("Received error code %d from mbed tls", error_code);
            return error_code;
    }
}

// Mbed TLS send BIO
static int iot_socket_send_bio(void *bio, const unsigned char *buf, size_t len)
{
    NetworkContext_t *ctx = (NetworkContext_t *)bio;

    if (!ctx || (!buf && len)) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = iotSocketSend(ctx->socket_fd, buf, len);
    if (res < 0) {
        if (res == IOT_SOCKET_EAGAIN) {
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        } else {
            PW_LOG_ERROR("iotSocketSend returned : %d", res);
            return iot_socket_to_mbedlts_error(res);
        }
    }

    return res;
}

// Mbed TLS receive BIO
static int iot_socket_recv_bio(void *bio, unsigned char *buf, size_t len)
{
    NetworkContext_t *ctx = (NetworkContext_t *)bio;

    if (!ctx) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = iotSocketRecv(ctx->socket_fd, buf, len);

    if (res < 0) {
        if (res == IOT_SOCKET_EAGAIN) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        } else {
            PW_LOG_ERROR("iotSocketRecv returned : %d", res);
            return iot_socket_to_mbedlts_error(res);
        }
    }

    return res;
}
