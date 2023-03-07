/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AWS_NETWORK_MANAGER_H_
#define AWS_NETWORK_MANAGER_H_

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ssl.h"

#include <string.h>

/**
 * Configuration used by a NetworkContext to establish the connection to the
 * AWS service.
 */
typedef struct AwsNetworkContextConfig {
    /**
     * Points to the server to connect to.
     */
    const char *server_name;

    /**
     * Certificate of the client.
     */
    const char *client_certificate;

    /**
     * Certificate of the server.
     */
    const char *server_certificate;

    /**
     * Private key of the client.
     */
    const char *client_private_key;

    /**
     * Public key of the client.
     */
    const char *client_public_key;

    /**
     * Data used to the feed the random number generator
     */
    const unsigned char *drbg;

    /**
     * Size of the drbg buffer
     */
    size_t drbg_len;

    /**
     * Timeout used for socket send operations.
     */
    uint32_t send_timeout;

    /**
     * Timeout used for socket receive operations.
     */
    uint32_t receive_timeout;

    /**
     * Max connection attempts.
     */
    uint32_t retry_max_attempts;

    /**
     * Maximum delay in ms between two connection try.
     */
    uint32_t backoff_max_delay;

    /**
     * Base delay between two connection attempts.
     */
    uint32_t backoff_base;

    /**
     * Port to connect to.
     */
    uint16_t port;
} AwsNetworkContextConfig_t;

/**
 * Network context used by coreHTTP and coreMQTT.
 *
 * This data structure should be initialized with AwsNetwork_init. The connection
 * is made with AwsNetwork_connect and closed with AwsNetwork_close.
 *
 * Once the connection has been made, it is possible to use the functions AwsNetwork_send
 * and AwsNetwork_recv which are designed to be set in a TransportInterface_t used
 * by coreHTTP or coreMQTT.
 */
typedef struct NetworkContext {
    /**
     * Configuration of the NetworkContext.
     * This configuration persist is only reference and not copied.
     */
    const AwsNetworkContextConfig_t *config;

    /**
     * TCP socket fd used to communicate with the server
     */
    int32_t socket_fd;
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
    mbedtls_x509_crt ca_cert;

    /**
     * Certificate used by the client
     */
    mbedtls_x509_crt client_cert;
    /**
     * The TLS context
     */
    mbedtls_ssl_context ssl;
    /**
     * The TLS configuration in use
     */
    mbedtls_ssl_config ssl_conf;
    /**
     * The public key container
     */
    mbedtls_pk_context pkey;
} NetworkContext_t;

/**
 * Hook called by AwsNetwork_connect after the TLS connection has been established.
 *
 * This hook can be used to implement connection to the MQTT broker while benefitting
 * from the backoff algorithm used in AwsNetwork_Connect.
 *
 * It must return 0 in case of success and non zero in case of error.
 */
typedef int (*AwsNetwork_connect_hook)(NetworkContext_t *ctx);

/**
 * Initialize a new instance of an AwsNetwork.
 *
 * @param ctx The NetworkContext_t to initialize.
 * @param config The configuration used to establish the network connection. This
 * argument is not copied into the NetworkContext_t. It is only referenced.
 *
 * @note The initialization only setups the data structure.
 */
void AwsNetwork_init(NetworkContext_t *ctx, const AwsNetworkContextConfig_t *config);

/**
 * Establish a TLS connection using the parameters provided in the configuration
 * set during the initialization.
 *
 * The connection uses a backoff algorithm for retries. The parameters of the
 * algorithm are part of the config set at initialization.
 *
 * If the protocol used is MQTT, the logical next step is to establish the MQTT
 * connection to the broker. Since this should be part of the whole connection
 * and be considered by the retry strategy in case of error; the application can
 * extend the behaviour by providing a connect_cb function.
 *
 * This function is executed as part of the connection process after the TLS
 * connection has been successfully established. At this point, the functions
 * AwsNetwork_send and AwsNetwork_recv are operational.
 *
 * @param ctx The NetworkContext_t to use to establish the connection.
 * @param connect_cb The hook that can is called when the TLS connection has been
 * successfully established. This parameter can be NULL if no hook is required.
 *
 * @return 0 in case of success and -1 in case of failure.
 */
int AwsNetwork_connect(NetworkContext_t *ctx, AwsNetwork_connect_hook connect_cb);

/**
 * Send a buffer over the TLS connection.
 *
 * This function can be provided to a TransportInterface_t object which is used
 * by coreMQTT and coreHTTP.
 *
 * @param ctx The connection.
 * @param buff Pointer to the buffer to send.
 * @param len Size of the buffer to send.
 *
 * @return A positive integer represents the number of bytes sent. A zero, means
 * the system is busy a negative integer represents an internal error.
 */
int AwsNetwork_send(NetworkContext_t *ctx, const void *buf, size_t len);

/**
 * Receive a buffer over the TLS connection.
 *
 * This function can be provided to a TransportInterface_t object which is used
 * by coreMQTT and coreHTTP.
 *
 * @param ctx The connection.
 * @param buff Pointer to the reception buffer.
 * @param len Size of the reception buffer.
 *
 * @return A positive integer represents the number of bytes received. A zero, means
 * the system is busy a negative integer represents an internal error.
 */
int AwsNetwork_recv(NetworkContext_t *ctx, void *buf, size_t len);

/**
 * Close the connection.
 *
 * The ctx is not usable after this call. It is mandatory to call AwsNetwork_init
 * before reusing it.
 */
void AwsNetwork_close(NetworkContext_t *ctx);

#endif // AWS_NETWORK_MANAGER_H_
