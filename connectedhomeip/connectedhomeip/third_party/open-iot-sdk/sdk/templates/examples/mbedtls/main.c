/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/* This example uses mbedtls to GET a page from example.com and dumps the HTML to serial.
 */

#include "cmsis_os2.h"
#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Server settings
#define SERVER_NAME "os.mbed.com"
#define SERVER_PORT 443
#define SERVER_REQUEST                                        \
    "GET /media/uploads/mbed_official/hello.txt HTTP/1.1\r\n" \
    "Host: os.mbed.com\r\n\r\n"
#define SERVER_RESPONSE       "200 OK"
#define DRBG_PERSONALIZED_STR "Mbed TLS client"

#define TLS_PEM_CA                                                         \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n" \
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n" \
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n" \
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n" \
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n" \
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n" \
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n" \
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n" \
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n" \
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n" \
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n" \
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n" \
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n" \
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n" \
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n" \
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n" \
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n" \
    "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"                                     \
    "-----END CERTIFICATE-----\r\n"

#define BUFFER_LENGTH  1024
#define DEBUG_LEVEL    1
#define INVALID_SOCKET -1

typedef enum net_event_t { NET_EVENT_NETWORK_UP, NET_EVENT_NETWORK_DOWN, NET_EVENT_NONE } net_event_t;

typedef struct {
    net_event_t event;
    int32_t return_code;
} net_msg_t;

static osMessageQueueId_t net_msg_queue = NULL;

static const char mbedtls_cas_pem[] = TLS_PEM_CA;

static const size_t mbedtls_cas_pem_len = sizeof(mbedtls_cas_pem);

static void debug_print(void *ctx, int level, const char *file, int line, const char *str)
{
    (void)ctx;

    const char *p, *basename;

    /* Extract basename from file */
    for (p = basename = file; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\')
            basename = p + 1;
    }

    printf("%s:%d: |%d| %s\r\n", basename, line, level, str);
}

typedef struct {
    /**
     * TCP socket fd used to communicate with the server
     */
    int32_t socket_fd;

    /**
     * A generic buffer used to hold debug or HTTP request/response strings
     */
    char gp_buf[BUFFER_LENGTH];

    /**
     * Entropy context used to seed the DRBG to use in the TLS connection
     */
    mbedtls_entropy_context entropy;
    /**
     * The DRBG used throughout the TLS connection
     */
    mbedtls_ctr_drbg_context ctr_drbg;
    /**
     * The parsed chain of trusted CAs
     */
    mbedtls_x509_crt cacert;
    /**
     * THe TLS context
     */
    mbedtls_ssl_context ssl;
    /**
     * The TLS configuration in use
     */
    mbedtls_ssl_config ssl_conf;
} app_context_t;

static app_context_t appContext;

/** This callback is called by the ip network task. It translates from a network event code
 * to our app message queue code and sends the event to the main app task.
 *
 * @param event network up or down event.
 */
static void network_event_callback(network_state_callback_event_t event)
{
    const net_msg_t msg = {.event = (event == NETWORK_UP) ? NET_EVENT_NETWORK_UP : NET_EVENT_NETWORK_DOWN};
    if (osMessageQueuePut(net_msg_queue, &msg, 0, 0) != osOK) {
        printf("Failed to send message to net_msg_queue\r\n");
    }
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

static int mbedtls_iot_socket_send(app_context_t *ctx, const unsigned char *buf, size_t len)
{
    if (!ctx) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = iotSocketSend(ctx->socket_fd, buf, len);
    if (res < 0) {
        return iot_socket_to_mbedlts_error(res);
    }

    return res;
}

static int mbedtls_iot_socket_recv(app_context_t *ctx, unsigned char *buf, size_t len)
{
    if (!ctx) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    int32_t res = iotSocketRecv(ctx->socket_fd, buf, len);
    if (res < 0) {
        return iot_socket_to_mbedlts_error(res);
    }

    return res;
}

static int mbedtls_platform_example_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    if (buf == NULL) {
        return (-1);
    }
    memset(buf, 0xA5, buf_len);
    return 0;
}

static int mbedtls_platform_example_nv_seed_write(unsigned char *buf, size_t buf_len)
{
    return 0;
}

static void app_context_init(void)
{
    appContext.socket_fd = INVALID_SOCKET;

    mbedtls_threading_set_cmsis_rtos();

    mbedtls_entropy_init(&appContext.entropy);
    mbedtls_ctr_drbg_init(&appContext.ctr_drbg);
    mbedtls_x509_crt_init(&appContext.cacert);
    mbedtls_ssl_init(&appContext.ssl);
    mbedtls_ssl_config_init(&appContext.ssl_conf);
    mbedtls_platform_set_nv_seed(mbedtls_platform_example_nv_seed_read, mbedtls_platform_example_nv_seed_write);
}

static void app_context_deinit(void)
{
    mbedtls_entropy_free(&appContext.entropy);
    mbedtls_ctr_drbg_free(&appContext.ctr_drbg);
    mbedtls_x509_crt_free(&appContext.cacert);
    mbedtls_ssl_free(&appContext.ssl);
    mbedtls_ssl_config_free(&appContext.ssl_conf);

    if (appContext.socket_fd != INVALID_SOCKET) {
        if (iotSocketClose(appContext.socket_fd) == 0) {
            appContext.socket_fd = INVALID_SOCKET;
        }
    }

    mbedtls_threading_free_alt();
}

static int configure_socket(void)
{
    int res = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    if (res < 0) {
        printf("iotSocketCreate failed %d\r\n", res);
        return res;
    }

    appContext.socket_fd = res;

    return 0;
}

static int configure_tls_contexts(void)
{
    int res;

    res = mbedtls_ctr_drbg_seed(&appContext.ctr_drbg,
                                mbedtls_entropy_func,
                                &appContext.entropy,
                                (const unsigned char *)DRBG_PERSONALIZED_STR,
                                strlen(DRBG_PERSONALIZED_STR) + 1);
    if (res != 0) {
        printf("mbedtls_ctr_drbg_seed() failed -0x%04X\r\n", -res);
        return res;
    }

    res = mbedtls_x509_crt_parse(&appContext.cacert, (const unsigned char *)mbedtls_cas_pem, mbedtls_cas_pem_len);
    if (res != 0) {
        printf("mbedtls_x509_crt_parse() failed -0x%04X\r\n", -res);
        return res;
    }

    res = mbedtls_ssl_config_defaults(
        &appContext.ssl_conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (res != 0) {
        printf("mbedtls_ssl_config_defaults() failed -0x%04X\r\n", -res);
        return res;
    }

    mbedtls_ssl_conf_authmode(&appContext.ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&appContext.ssl_conf, &appContext.cacert, NULL);
    mbedtls_ssl_conf_rng(&appContext.ssl_conf, mbedtls_ctr_drbg_random, &appContext.ctr_drbg);

#if DEBUG_LEVEL > 0
    mbedtls_ssl_conf_dbg(&appContext.ssl_conf, debug_print, NULL);
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif /* DEBUG_LEVEL > 0 */

    if ((res = mbedtls_ssl_setup(&appContext.ssl, &appContext.ssl_conf)) != 0) {
        printf("mbedtls_ssl_setup() failed -0x%04X\r\n", -res);
        return res;
    }

    if ((res = mbedtls_ssl_set_hostname(&appContext.ssl, SERVER_NAME)) != 0) {
        printf("mbedtls_ssl_set_hostname() failed -0x%04X\r\n", -res);
        return res;
    }

    mbedtls_ssl_set_bio(&appContext.ssl, &appContext, mbedtls_iot_socket_send, mbedtls_iot_socket_recv, NULL);

    return 0;
}

static int connect_server(void)
{
    uint8_t ip[4];
    uint32_t ip_len = sizeof(ip);
    int res = iotSocketGetHostByName((const char *)SERVER_NAME, IOT_SOCKET_AF_INET, ip, &ip_len);
    if (res < 0) {
        printf("iotSocketGetHostByName failed %d\r\n", res);
        return res;
    }

    printf("Server %s %d.%d.%d.%d:%d connecting...\r\n", SERVER_NAME, ip[0], ip[1], ip[2], ip[3], SERVER_PORT);

    res = iotSocketConnect(appContext.socket_fd, (const uint8_t *)&ip, ip_len, SERVER_PORT);
    if (res) {
        printf("iotSocketConnect failed %d\r\n", res);
        return res;
    }

    printf("Successful connection to the server\r\n");

    return res;
}

static int connect_tls(void)
{
    int res;
    uint32_t flags;

    printf("Performing the TLS handshake...\r\n");
    do {
        res = mbedtls_ssl_handshake(&appContext.ssl);
    } while (res != 0 && (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE));
    if (res < 0) {
        printf("mbedtls_ssl_handshake() failed -0x%04X\r\n", -res);
        return res;
    }

    printf("Successfully completed the TLS handshake\r\n");
    printf("Verifying certificate...\r\n");

    flags = mbedtls_ssl_get_verify_result(&appContext.ssl);
    if (flags != 0) {
        res = mbedtls_x509_crt_verify_info(appContext.gp_buf, sizeof(appContext.gp_buf), "\r  ! ", flags);
        if (res < 0) {
            printf("mbedtls_x509_crt_verify_info() failed "
                   "-0x%04X\r\n",
                   -res);
            return res;
        } else {
            printf("Certificate verification failed (flags %u):"
                   "\n%s\r\n",
                   flags,
                   appContext.gp_buf);
            return flags;
        }
    } else {
        printf("Certificate verification passed\r\n");
    }

    printf("Established TLS connection\r\n");

    return 0;
}

static int send_server_request(void)
{
    int res = 0;
    size_t req_len, req_offset;

    printf("Sending server request\r\n");

    /* Fill the request buffer */
    res = snprintf(appContext.gp_buf, sizeof(appContext.gp_buf), "%s", SERVER_REQUEST);
    req_len = (size_t)res;
    if (res < 0 || req_len >= sizeof(appContext.gp_buf)) {
        printf("Failed to compose HTTP request\r\n");
        return res;
    }

    req_offset = 0;
    do {
        res = mbedtls_ssl_write(
            &appContext.ssl, (const unsigned char *)(appContext.gp_buf + req_offset), req_len - req_offset);
        if (res > 0)
            req_offset += (size_t)res;
    } while (req_offset < req_len
             && (res > 0 || res == MBEDTLS_ERR_SSL_WANT_WRITE || res == MBEDTLS_ERR_SSL_WANT_READ));
    if (res < 0) {
        printf("mbedtls_ssl_write() failed -0x%04X\r\n", -res);
        return res;
    }

    printf("Server request successfully sent\r\n");

    return 0;
}

static int get_server_response(void)
{
    int res;
    size_t resp_offset;
    bool expected_resp;

    printf("Reading server response...\r\n");

    memset(appContext.gp_buf, 0, sizeof(appContext.gp_buf));
    resp_offset = 0;
    expected_resp = false;
    do {
        res = mbedtls_ssl_read(&appContext.ssl,
                               (unsigned char *)(appContext.gp_buf + resp_offset),
                               sizeof(appContext.gp_buf) - resp_offset - 1);
        if (res > 0) {
            resp_offset += (size_t)res;
        }

        /* Ensure that the response string is null-terminated */
        appContext.gp_buf[resp_offset] = '\0';

        /* Check  if we received expected response */
        expected_resp = expected_resp || strstr(appContext.gp_buf, SERVER_RESPONSE) != NULL;
    } while (!expected_resp && (res > 0 || res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE));
    if (res < 0) {
        printf("mbedtls_ssl_read() failed -0x%04X\r\n", -res);
        return res;
    }

    printf("Server response %s\n", expected_resp ? "OK" : "FAIL");

    return 0;
}

static void app_task(void *arg)
{
    (void)arg;

    app_context_init();

    int res;

    while (1) {
        net_msg_t msg;
        if (osMessageQueueGet(net_msg_queue, &msg, NULL, 1000) != osOK) {
            msg.event = NET_EVENT_NONE;
        }

        if (msg.event == NET_EVENT_NETWORK_UP) {
            printf("Network is up\r\n");

            res = configure_socket();
            if (res) {
                printf("Configure socket failed: %d\r\n", res);
                break;
            }

            res = configure_tls_contexts();
            if (res) {
                printf("Configure TLS contexts failed: %d\r\n", res);
                break;
            }

            res = connect_server();
            if (res) {
                printf("Connect server failed: %d\r\n", res);
                break;
            }

            res = connect_tls();
            if (res) {
                printf("TLS connection failed: %d\r\n", res);
                break;
            }

            res = send_server_request();
            if (res) {
                printf("Send server TLS request failed: %d\r\n", res);
                break;
            }

            res = get_server_response();
            if (res) {
                printf("Received server TLS response failed: %d\r\n", res);
                break;
            }

            break;
        } else if (msg.event == NET_EVENT_NETWORK_DOWN) {
            printf("Network is not enable\r\n");
            break;
        }
    }

    app_context_deinit();
    printf("Demo stopped\r\n");
    while (1)
        ;
}

int main(void)
{
    printf("Initialising kernel\r\n");
    osStatus_t res = osKernelInitialize();
    if (res != osOK) {
        printf("osKernelInitialize failed: %d\r\n", res);
        return EXIT_FAILURE;
    }

    net_msg_queue = osMessageQueueNew(10, sizeof(net_msg_t), NULL);
    if (!net_msg_queue) {
        printf("Failed to create a net msg queue\r\n");
        return EXIT_FAILURE;
    }

    osThreadId_t demo_thread = osThreadNew(app_task, NULL, NULL);
    if (!demo_thread) {
        printf("Failed to create thread\r\n");
        return EXIT_FAILURE;
    }

    printf("Initialising network\r\n");
    res = start_network_task(network_event_callback, 0);
    if (res != osOK) {
        printf("Failed to start lwip\r\n");
        return EXIT_FAILURE;
    }

    osKernelState_t state = osKernelGetState();
    if (state != osKernelReady) {
        printf("Kernel not ready: %d\r\n", state);
        return EXIT_FAILURE;
    }

    printf("Starting kernel\r\n");
    res = osKernelStart();
    if (res != osOK) {
        printf("osKernelStart failed: %d\r\n", res);
        return EXIT_FAILURE;
    }

    return 0;
}
