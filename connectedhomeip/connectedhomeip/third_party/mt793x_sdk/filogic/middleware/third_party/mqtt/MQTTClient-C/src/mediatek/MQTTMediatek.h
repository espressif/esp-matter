/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MQTT_MEDIATEK__
#define __MQTT_MEDIATEK__

#include "mbedtls/compat-1.3.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"

#include "syslog.h"

typedef struct Network Network;

/**
 * @addtogroup MQTT
 * @{
 * Open source MQTT client implementation is part of Paho project aimed at new, existing, and emerging applications for Machine?‘to?‘Machine (M2M) and Internet of Things (IoT). For more details, please refer to <a href="http://www.eclipse.org/paho"> Paho project </a>.
 * In this release, MediaTek has integrated Paho MQTT protocol with mbed TLS and lwIP. New APIs related to the network connectivity are also introduced in this section.
 * The MQTT add-on APIs follow the original Paho project coding style.
 * A reference example can be found under /project/mt7687_hdk/apps/mqtt_client folder.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b MQTT                       | MQ Telemetry Transport. For more details, please refer to <a href="https://en.wikipedia.org/wiki/MQTT">wikipedia MQTT</a>. |
 * |\b TLS                        | Transport Layer Security. |
 * |\b lwIP                       | Lightweight IP is a widely used open source TCP/IP stack designed for embedded systems. |
 * |\b Wi-Fi                      | Wi-Fi is a local area wireless computer networking technology. For more details, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi">wikipedia Wi-Fi</a>. |
 *
 */

/** @defgroup mqtt_struct Struct
  * @{
  */

/**
 * @brief The structure of MQTT network connection used in the MQTT library. The user has to allocate memory for this structure.
 */
struct Network {
    int my_socket;                                                /**< Connect the socket handle. */
    int (*mqttread)(Network *, unsigned char *, int, int);        /**< Read data from server function pointer. */
    int (*mqttwrite)(Network *, unsigned char *, int, int);       /**< Send data to server function pointer. */
    void (*disconnect)(Network *);    /**< Disconnect the network function pointer. */
    void (*on_disconnect_callback)(Network *n);     /**< When disconnected, the callback will be called. */
    mbedtls_ssl_context ssl;          /**< mbed TLS control context. */
    mbedtls_net_context fd;           /**< mbed TLS network context. */
    mbedtls_ssl_config conf;          /**< mbed TLS configuration context. */
    mbedtls_x509_crt cacertl;         /**< mbed TLS CA certification. */
    mbedtls_x509_crt clicert;         /**< mbed TLS Client certification. */
    mbedtls_pk_context pkey;          /**< mbed TLS Client key. */
};

/**
 * @}
 */

/**
 * @brief This function initializes the network structure.
 * @param[in] n is the network structure pointer. The user is responsible to allocate memory before calling this function. Call this function before #ConnectNetwork() and #TLSConnectNetwork().
 * @sa #ConnectNetwork();
 * @return None
 */
void NewNetwork(Network *n);


/**
 * @brief This function connects to the MQTT server without TLS, and returns a value that indicates whether the connection is created successfully or not. Call #NewNetwork() to initialize network structure before calling this function.
 * @param[in] n is a pointer to the network structure.
 * @param[in] addr is the server hostname or IP address.
 * @param[in] port is the server port.
 * @sa #NewNetwork ();
 * @return If the return value is 0, the connection is created successfully. If the return value is -1, then calling lwIP socket() has failed. If the return value is -2, then calling lwIP connect() has failed. Any other value indicates that calling lwIP getaddrinfo() has failed.
 */
int ConnectNetwork(Network *n, char *addr,  char *port);


/**
 * @brief This function connects to a MQTT server with TLS, and returns a value that indicates whether the connection is create successfully or not. Call #NewNetwork() to initialize network structure before calling this function.
 * @param[in] n is the the network structure pointer.
 * @param[in] addr is the Server Host name or IP address.
 * @param[in] port is the Server Port.
 * @param[in] ca_crt is the Server's CA certification.
 * @param[in] ca_crt_len is the length of Server's CA certification.
 * @param[in] client_crt is the client certification.
 * @param[in] client_crt_len is the length of client certification.
 * @param[in] client_key is the client key.
 * @param[in] client_key_len is the length of client key.
 * @param[in] client_pwd is the password of client key.
 * @param[in] client_pwd_len is the length of client key's password.
 * @sa #NewNetwork ();
 * @return If the return value is 0, the connection is created successfully. If the return value is -1, then calling lwIP socket() has failed. If the return value is -2, then calling lwIP connect() has failed. Any other value indicates that calling lwIP getaddrinfo() has failed.
 */
int TLSConnectNetwork(Network *n, const char *addr, const char *port,
                      const char *ca_crt, size_t ca_crt_len,
                      const char *client_crt,    size_t client_crt_len,
                      const char *client_key,    size_t client_key_len,
                      const char *client_pwd, size_t client_pwd_len);


/**
 * @}
 */


/* The following content is used inside the MQTT module. */
typedef struct Timer Timer;
/**
 * @brief The structure of MQTT timer, it is used in MQTT internal, customer doesn't need to use.
 */
struct Timer {
    unsigned int systick_period; /**< How many sytem ticks from the timer start to run to it timeout */
    unsigned int end_time; /**< The sytem time when the timer timeout. */
};

char expired(Timer *timer);
void countdown_ms(Timer *timer, unsigned int timeout);
void countdown(Timer *timer, unsigned int timeout);
unsigned int left_ms(Timer *timer);
void InitTimer(Timer *timer);

/** Define 0 to disable logging, define 1 to enable logging. */
#ifndef MTK_DEBUG_LEVEL_NONE
#define MTK_MQTT_DEBUG 1
#else
#define MTK_MQTT_DEBUG 0
#endif

#define MTK_MQTT_DEBUG 1

#if MTK_MQTT_DEBUG
#define MQTT_DBG(fmt,arg...)  LOG_I(MQTT_CLIENT, "[MQTT_CLIENT]: "fmt,##arg)
#define MQTT_WARN(fmt,arg...) LOG_W(MQTT_CLIENT, "[MQTT_CLIENT]: "fmt,##arg)
#define MQTT_ERR(fmt,arg...)  LOG_E(MQTT_CLIENT ,"[MQTT_CLIENT]: "fmt,##arg)
#else
#define MQTT_DBG(x, ...)
#define MQTT_WARN(x, ...)
#define MQTT_ERR(x, ...)
#endif

#endif /* __MQTT_MEDIATEK__ */
