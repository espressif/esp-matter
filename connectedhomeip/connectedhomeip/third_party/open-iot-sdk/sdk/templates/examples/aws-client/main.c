/* Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/* This example uses mbedtls to GET a page from example.com and dumps the HTML to serial.
 */

#define PW_LOG_MODULE_NAME "main"
#include "AwsNetworkManager/AwsNetworkManager.h"
#include "aws_clientcredential.h"
#include "aws_clientcredential_keys.h"
#include "cmsis_os2.h"
#include "core_mqtt.h"
#include "hal/serial_api.h"
#include "iotsdk/ip_network_api.h"
#include "mbedtls/platform.h"
#include "pw_log/log.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define DEMO_PUBLISH_TOPIC "OpenIoTSDK/demo"
#define DEMO_PAYLOAD       "Hello World!"

const unsigned char drbg_str[] = "Open IoT SDK - AWS Client example";

static const AwsNetworkContextConfig_t networkConfig = {.server_name = clientcredentialMQTT_BROKER_ENDPOINT,
                                                        .server_certificate = keyCA1_ROOT_CERTIFICATE_PEM,
                                                        .client_certificate = keyCLIENT_CERTIFICATE_PEM,
                                                        .client_private_key = keyCLIENT_PRIVATE_KEY_PEM,
                                                        .client_public_key = keyCLIENT_PUBLIC_KEY_PEM,
                                                        .drbg = drbg_str,
                                                        .drbg_len = sizeof(drbg_str),
                                                        .send_timeout = 500,
                                                        .receive_timeout = 500,
                                                        .retry_max_attempts = 5,
                                                        .backoff_max_delay = 5000,
                                                        .backoff_base = 500,
                                                        .port = clientcredentialMQTT_BROKER_PORT};

static NetworkContext_t network_context;

static TransportInterface_t mqtt_transport = {
    .send = AwsNetwork_send, .recv = AwsNetwork_recv, .pNetworkContext = &network_context};

static uint8_t buffer[1024] = {0};
static MQTTFixedBuffer_t mqttBuffer = {
    .pBuffer = buffer,
    .size = sizeof(buffer),
};

static const MQTTConnectInfo_t mqtt_connect_info = {
    .cleanSession = true,
    .pClientIdentifier = clientcredentialIOT_THING_NAME,
    .clientIdentifierLength = (uint16_t)(sizeof(clientcredentialIOT_THING_NAME) - 1),
    .keepAliveSeconds = 60U,
};

static MQTTContext_t mqtt_context = {0};

typedef enum net_event_t { NET_EVENT_NETWORK_UP, NET_EVENT_NETWORK_DOWN, NET_EVENT_NONE } net_event_t;
typedef struct {
    net_event_t event;
    int32_t return_code;
} net_msg_t;

static osMessageQueueId_t net_msg_queue = NULL;

// store the packet id sent by the application
static uint16_t mqtt_packet_id;

mdh_serial_t *serial = NULL;
extern mdh_serial_t *get_example_serial();

// Events sent and received by the MQTT receiving thread.
#define EVENT_TERMINATE_MQTT_THREAD    (1 << 0)
#define EVENT_MQTT_THREAD_TERMINATING  (1 << 1)
#define EVENT_MQTT_THREAD_ACK_RECEIVED (1 << 2)

// Shared event flags for the MQTT reception thread
osEventFlagsId_t mqtt_recv_thread_events;

static void serial_setup();
static int mbedtls_platform_example_nv_seed_read(unsigned char *buf, size_t buf_len);
static int mbedtls_platform_example_nv_seed_write(unsigned char *buf, size_t buf_len);
static void network_event_callback(network_state_callback_event_t event);
static void app_task(void *arg);
static uint32_t get_time_ms(void);
static void
mqtt_event_cb(MQTTContext_t *mqtt_context, MQTTPacketInfo_t *packet_info, MQTTDeserializedInfo_t *deserialized_info);
static void mqtt_background_process(void *arg);
static int connect_mqtt(NetworkContext_t *ctx);

// Initialize the system then handover to app_task
int main(void)
{
    serial_setup();

    osStatus_t res = osKernelInitialize();
    if (res != osOK) {
        return EXIT_FAILURE;
    }

    // Initialization of PW log is done after the kernel initialization because
    // it requires a lock
    pw_log_mdh_init(serial);

    net_msg_queue = osMessageQueueNew(10, sizeof(net_msg_t), NULL);
    if (!net_msg_queue) {
        PW_LOG_ERROR("Failed to create a net msg queue");
        return EXIT_FAILURE;
    }

    const osThreadAttr_t thread1_attr = {
        .stack_size = 8192 // Create the thread stack with a size of 8192 bytes
    };
    osThreadId_t demo_thread = osThreadNew(app_task, NULL, &thread1_attr);
    if (!demo_thread) {
        PW_LOG_ERROR("Failed to create thread");
        return EXIT_FAILURE;
    }

    PW_LOG_INFO("Initialising network");
    res = start_network_task(network_event_callback, 0);
    if (res != osOK) {
        PW_LOG_ERROR("Failed to start lwip");
        return EXIT_FAILURE;
    }

    osKernelState_t state = osKernelGetState();
    if (state != osKernelReady) {
        PW_LOG_ERROR("Kernel not ready");
        return EXIT_FAILURE;
    }

    // Configure Mbed TLS
    mbedtls_threading_set_cmsis_rtos();
    int err =
        mbedtls_platform_set_nv_seed(mbedtls_platform_example_nv_seed_read, mbedtls_platform_example_nv_seed_write);
    if (err) {
        PW_LOG_ERROR("Failed to initialize NV seed functions");
        return EXIT_FAILURE;
    }

    PW_LOG_INFO("Starting kernel");
    res = osKernelStart();
    if (res != osOK) {
        PW_LOG_ERROR("osKernelStart failed: %d", res);
        return EXIT_FAILURE;
    }

    return 0;
}

static void app_task(void *arg)
{
    (void)arg;

    // Wait for the connection to be established
    PW_LOG_INFO("Awaiting network connection");
    while (1) {
        net_msg_t msg;
        if (osMessageQueueGet(net_msg_queue, &msg, NULL, 1000) != osOK) {
            msg.event = NET_EVENT_NONE;
        }

        if (msg.event == NET_EVENT_NETWORK_UP) {
            PW_LOG_INFO("Network connection enabled");
            break;
        } else if (msg.event == NET_EVENT_NETWORK_DOWN) {
            PW_LOG_DEBUG("Network is not enabled");
        }
    }

    // Initialize the MQTT
    PW_LOG_INFO("Initialising MQTT connection");

    MQTTStatus_t mqtt_status = MQTT_Init(&mqtt_context, &mqtt_transport, get_time_ms, mqtt_event_cb, &mqttBuffer);
    if (mqtt_status != MQTTSuccess) {
        PW_LOG_ERROR("MQTT Init failed - MQTT status = %d", mqtt_status);
        return;
    }

    PW_LOG_INFO("Establishing TLS connection");
    AwsNetwork_init(&network_context, &networkConfig);
    int res = AwsNetwork_connect(&network_context, connect_mqtt);
    if (res) {
        PW_LOG_ERROR("Connection to network failed: %d", res);
        AwsNetwork_close(&network_context);
        return;
    }

    // Start MQTT processing thread. This thread receives data from the server
    // and trigger the mqttEventCallBack.
    // To stop it send and EVENT_TERMINATE_MQTT_THREAD to it.
    PW_LOG_INFO("Starting MQTT receiving thread");
    const osThreadAttr_t mqtt_recv_thread_config = {
        .stack_size = 4096 // Create the thread stack with a size of 4096 bytes
    };
    mqtt_recv_thread_events = osEventFlagsNew(NULL);
    if (mqtt_recv_thread_events == NULL) {
        PW_LOG_ERROR("Failed to create MQTT receiving thead events");
        return;
    }

    osThreadId_t mqtt_recv_thread = osThreadNew(mqtt_background_process, NULL, &mqtt_recv_thread_config);
    if (mqtt_recv_thread == NULL) {
        PW_LOG_ERROR("Failed to create MQTT processing thread");
        return;
    }

    // Publish a packet to the demo topic
    PW_LOG_INFO("Publishing demo message to %s", DEMO_PUBLISH_TOPIC);
    mqtt_packet_id = MQTT_GetPacketId(&mqtt_context);
    MQTTPublishInfo_t publish_info = {.qos = MQTTQoS1,
                                      .pTopicName = DEMO_PUBLISH_TOPIC,
                                      .topicNameLength = strlen(DEMO_PUBLISH_TOPIC),
                                      .pPayload = DEMO_PAYLOAD,
                                      .payloadLength = strlen(DEMO_PAYLOAD)};
    mqtt_status = MQTT_Publish(&mqtt_context, &publish_info, mqtt_packet_id);
    if (mqtt_status != MQTTSuccess) {
        PW_LOG_ERROR("MQTT Publish failed - MQTT status = %d", mqtt_status);
        return;
    }

    // Waiting for packet ack
    PW_LOG_INFO("Waiting for packet ack");
    uint32_t previous_event =
        osEventFlagsWait(mqtt_recv_thread_events, EVENT_MQTT_THREAD_ACK_RECEIVED, osFlagsWaitAny, osWaitForever);
    if (previous_event & osFlagsError) {
        PW_LOG_ERROR("Faillure while waiting for ack");
        return;
    }

    // Terminating MQTT receiving thread
    PW_LOG_INFO("Terminating MQTT receiving thread");
    uint32_t events = osEventFlagsSet(mqtt_recv_thread_events, EVENT_TERMINATE_MQTT_THREAD);
    if (events & osFlagsError) {
        PW_LOG_ERROR("Failed to set flag to terminate receive thread");
        return;
    }
    previous_event =
        osEventFlagsWait(mqtt_recv_thread_events, EVENT_MQTT_THREAD_TERMINATING, osFlagsWaitAny, osWaitForever);
    if (previous_event & osFlagsError) {
        PW_LOG_ERROR("Faillure while waiting for receiving thread termination signal");
        return;
    }
    if (osThreadTerminate(mqtt_recv_thread)) {
        PW_LOG_ERROR("Failed to terminate MQTT receiving thread");
        return;
    }

    // Cleanup of the session and connection
    PW_LOG_INFO("Closing MQTT session and closing TLS connection");
    mqtt_status = MQTT_Disconnect(&mqtt_context);
    if (mqtt_status != MQTTSuccess) {
        PW_LOG_ERROR("MQTT Disconnect failed - MQTT status = %d", mqtt_status);
        return;
    }

    AwsNetwork_close(&network_context);

    PW_LOG_INFO("Demo finished !");
}

// Callback called by AwsNetwork_connect when the TLS connection is established.
static int connect_mqtt(NetworkContext_t *ctx)
{
    // Initialize and connect the MQTT instance
    PW_LOG_INFO("Setuping MQTT connection");

    bool session_present = false;
    MQTTStatus_t mqtt_status = MQTT_Connect(&mqtt_context, &mqtt_connect_info, NULL, 0, &session_present);
    if (mqtt_status != MQTTSuccess) {
        PW_LOG_ERROR("MQTT Connection failed - MQTT status = %d", mqtt_status);
        return -1;
    }
    return 0;
}

// Callback invoked whenever an MQTT packet is received.
static void
mqtt_event_cb(MQTTContext_t *mqtt_context, MQTTPacketInfo_t *packet_info, MQTTDeserializedInfo_t *deserialized_info)
{
    /* The MQTT context is not used for this demo. */
    (void)mqtt_context;

    switch (packet_info->type) {
        case MQTT_PACKET_TYPE_CONNECT:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_CONNECT");
            break;

        case MQTT_PACKET_TYPE_CONNACK:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_CONNACK");
            break;

        case MQTT_PACKET_TYPE_PUBLISH:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PUBLISH");
            break;

        case MQTT_PACKET_TYPE_PUBACK:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PUBACK");
            /* Make sure ACK packet identifier matches with Request packet identifier. */
            if (mqtt_packet_id != deserialized_info->packetIdentifier) {
                PW_LOG_WARN(
                    "Packet id received: %d, expected: %d", deserialized_info->packetIdentifier, mqtt_packet_id);
            } else {
                osEventFlagsSet(mqtt_recv_thread_events, EVENT_MQTT_THREAD_ACK_RECEIVED);
            }
            break;

        case MQTT_PACKET_TYPE_PUBREC:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PUBREC");
            break;

        case MQTT_PACKET_TYPE_PUBREL:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PUBREL");
            break;

        case MQTT_PACKET_TYPE_PUBCOMP:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PUBCOMP");
            break;

        case MQTT_PACKET_TYPE_SUBSCRIBE:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_SUBSCRIBE");
            break;

        case MQTT_PACKET_TYPE_SUBACK:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_SUBACK");
            break;

        case MQTT_PACKET_TYPE_UNSUBSCRIBE:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_UNSUBSCRIBE");
            break;

        case MQTT_PACKET_TYPE_UNSUBACK:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_UNSUBACK");
            break;

        case MQTT_PACKET_TYPE_PINGREQ:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PINGREQ");
            break;

        case MQTT_PACKET_TYPE_PINGRESP:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_PINGRESP");
            break;

        case MQTT_PACKET_TYPE_DISCONNECT:
            PW_LOG_DEBUG("Received MQTT packet of type: %s", "MQTT_PACKET_TYPE_DISCONNECT");
            break;

        default:
            PW_LOG_WARN("Received MQTT packet of unknown type: %d", packet_info->type);
            break;
    }
}

// Thread function that process incoming MQTT packets
static void mqtt_background_process(void *arg)
{
    // Continue processing until the thread is instructed to stop
    while (!(osEventFlagsGet(mqtt_recv_thread_events) & EVENT_TERMINATE_MQTT_THREAD)) {
        MQTTStatus_t status = MQTT_ProcessLoop(&mqtt_context, 100);
        if (status != MQTTSuccess) {
            PW_LOG_ERROR("MQTT_ProcessLoop: %d", status);
        } else {
            PW_LOG_DEBUG("MQTT_ProcessLoop done");
        }
    }

    // signal the thread is terminating and sleep
    osEventFlagsSet(mqtt_recv_thread_events, EVENT_MQTT_THREAD_TERMINATING);
    osDelay(osWaitForever);
}

static uint32_t get_time_ms(void)
{
    uint64_t time_ms = (osKernelGetTickCount() * 1000) / osKernelGetTickFreq();
    if ((osKernelGetTickCount() * 1000) % osKernelGetTickFreq()) {
        time_ms += 1;
    }
    return (uint32_t)time_ms;
}

static void serial_setup()
{
    serial = get_example_serial();
    mdh_serial_set_baud(serial, 115200);
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

/** This callback is called by the ip network task. It translates from a network event code
 * to our app message queue code and sends the event to the main app task.
 *
 * @param event network up or down event.
 */
static void network_event_callback(network_state_callback_event_t event)
{
    const net_msg_t msg = {.event = (event == NETWORK_UP) ? NET_EVENT_NETWORK_UP : NET_EVENT_NETWORK_DOWN};
    if (osMessageQueuePut(net_msg_queue, &msg, 0, 0) != osOK) {
        PW_LOG_WARN("Failed to send message to net_msg_queue");
    }
}
