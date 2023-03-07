// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_OPTIONS_H
#define IOTHUB_CLIENT_OPTIONS_H

#include "azure_c_shared_utility/const_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct IOTHUB_PROXY_OPTIONS_TAG
    {
        const char* host_address;
        const char* username;
        const char* password;
    } IOTHUB_PROXY_OPTIONS;

    static STATIC_VAR_UNUSED const char* OPTION_RETRY_INTERVAL_SEC = "retry_interval_sec";
    static STATIC_VAR_UNUSED const char* OPTION_RETRY_MAX_DELAY_SECS = "retry_max_delay_secs";

    static STATIC_VAR_UNUSED const char* OPTION_LOG_TRACE = "logtrace";
    static STATIC_VAR_UNUSED const char* OPTION_X509_CERT = "x509certificate";
    static STATIC_VAR_UNUSED const char* OPTION_X509_PRIVATE_KEY = "x509privatekey";
    static STATIC_VAR_UNUSED const char* OPTION_KEEP_ALIVE = "keepalive";
    static STATIC_VAR_UNUSED const char* OPTION_CONNECTION_TIMEOUT = "connect_timeout";

    /* None of the OPTION_PROXY_* options below are implemented.  Use OPTION_HTTP_PROXY 
    from shared_util_options.h in https://github.com/Azure/azure-c-shared-utility/ repo instead */
    static STATIC_VAR_UNUSED const char* OPTION_PROXY_HOST = "proxy_address";
    static STATIC_VAR_UNUSED const char* OPTION_PROXY_USERNAME = "proxy_username";
    static STATIC_VAR_UNUSED const char* OPTION_PROXY_PASSWORD = "proxy_password";

    static STATIC_VAR_UNUSED const char* OPTION_SAS_TOKEN_LIFETIME = "sas_token_lifetime";
    static STATIC_VAR_UNUSED const char* OPTION_SAS_TOKEN_REFRESH_TIME = "sas_token_refresh_time";
    static STATIC_VAR_UNUSED const char* OPTION_CBS_REQUEST_TIMEOUT = "cbs_request_timeout";

    static STATIC_VAR_UNUSED const char* OPTION_MIN_POLLING_TIME = "MinimumPollingTime";
    static STATIC_VAR_UNUSED const char* OPTION_BATCHING = "Batching";

    /* DEPRECATED:: OPTION_MESSAGE_TIMEOUT is DEPRECATED! Use OPTION_SERVICE_SIDE_KEEP_ALIVE_FREQ_SECS for AMQP; MQTT has no option available. OPTION_MESSAGE_TIMEOUT legacy variable will be kept for back-compat.  */
    static STATIC_VAR_UNUSED const char* OPTION_MESSAGE_TIMEOUT = "messageTimeout";
    static STATIC_VAR_UNUSED const char* OPTION_BLOB_UPLOAD_TIMEOUT_SECS = "blob_upload_timeout_secs";
    static STATIC_VAR_UNUSED const char* OPTION_PRODUCT_INFO = "product_info";

    /*
    * @brief    Specifies the Digital Twin Model Id of the connection. Only valid for use with MQTT Transport
    */
    static STATIC_VAR_UNUSED const char* OPTION_MODEL_ID = "model_id";

    /*
    * @brief    Turns on automatic URL encoding of message properties + system properties. Only valid for use with MQTT Transport
    */
    static STATIC_VAR_UNUSED const char* OPTION_AUTO_URL_ENCODE_DECODE = "auto_url_encode_decode";

    /*
    * @brief Informs the service of what is the maximum period the client will wait for a keep-alive message from the service.
    *        The service must send keep-alives before this timeout is reached, otherwise the client will trigger its re-connection logic.
    *        Setting this option to a low value results in more aggressive/responsive re-connection by the client.
    *        The default value for this option is 240 seconds, and the minimum allowed is usually 5 seconds.
    *        To virtually disable the keep-alives from the service (and consequently the keep-alive timeout control on the client-side), set this option to a high value (e.g., UINT_MAX).
    */
    static STATIC_VAR_UNUSED const char* OPTION_SERVICE_SIDE_KEEP_ALIVE_FREQ_SECS = "svc2cl_keep_alive_timeout_secs";

    /* DEPRECATED:: OPTION_C2D_KEEP_ALIVE_FREQ_SECS is DEPRECATED!  Use OPTION_SERVICE_SIDE_KEEP_ALIVE_FREQ_SECS, but OPTION_C2D_KEEP_ALIVE_FREQ_SECS legacy variable kept for back-compat.  */
    static STATIC_VAR_UNUSED const char* OPTION_C2D_KEEP_ALIVE_FREQ_SECS = "c2d_keep_alive_freq_secs";

    /*
    * @brief Ratio to be used for client side pings in AMQP protocol.
    *        The client must use this ratio to send keep-alives before service side remote idle timeout is reached, otherwise the service will disconnect the client.
    *        The default value for this option is 1/2 of the remote idle value sent by the service.
    *        For AMQP remote idle set to 4 minutes, default client ping will be 2 minutes. For AMQP remote idle set to 25 minutes configured via per Hub basis, the default ping will be 12.5 minutes.
    */
    static STATIC_VAR_UNUSED const char* OPTION_REMOTE_IDLE_TIMEOUT_RATIO = "cl2svc_keep_alive_send_ratio";

    /*
    * @brief This option should be used instead of OPTION_MESSAGE_TIMEOUT if using AMQP protocol.
    *        It defines the maximum ammount of time, in seconds, the client will wait for a telemetry message to complete sending before returning it with a IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT error.
    *        The default value 5 (five) minutes.
    *        This option is applicable only to AMQP protocol.
    */
    static STATIC_VAR_UNUSED const char* OPTION_EVENT_SEND_TIMEOUT_SECS = "event_send_timeout_secs";

    //diagnostic sampling percentage value, [0-100]
    static STATIC_VAR_UNUSED const char* OPTION_DIAGNOSTIC_SAMPLING_PERCENTAGE = "diag_sampling_percentage";

    static STATIC_VAR_UNUSED const char* OPTION_DO_WORK_FREQUENCY_IN_MS = "do_work_freq_ms";

// Minimum percentage (in the 0 to 1 range) of multiplexed registered devices that must be failing for a transport-wide reconnection to be triggered.
// A value of zero results in a single registered device to be able to cause a general transport reconnection 
// (thus causing all other multiplexed registered devices to be also reconnected, meaning an agressive reconnection strategy).
// Setting this parameter to one indicates that 100% of the multiplexed registered devices must be failing in parallel for a 
// transport-wide reconnection to be triggered (resulting in a very lenient reconnection strategy).  
#define DEVICE_MULTIPLEXING_FAULTY_DEVICE_RATIO_RECONNECTION_THRESHOLD 0

// Minimum number of consecutive failures an individual registered device must have to be considered a faulty device.
// This is used along with DEVICE_MULTIPLEXING_FAULTY_DEVICE_RATIO_RECONNECTION_THRESHOLD to trigger transport-wide reconnections.
// The device may fail to authenticate, timeout establishing the connection, get disconnected by the service for some reason or fail sending messages.
// In all these cases the failures are cummulatively counted; if the count is equal to or greater than DEVICE_FAILURE_COUNT_RECONNECTION_THRESHOLD
// the device is considered to be in a faulty state.    
#define DEVICE_FAILURE_COUNT_RECONNECTION_THRESHOLD 5

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_OPTIONS_H */
