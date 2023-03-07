/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#define IP6ADDR_SIZE  16
#define MAX_DATA_SENT 5

typedef enum net_event_t {
    NET_EVENT_NETWORK_UP,
    NET_EVENT_NETWORK_DOWN,
    NET_EVENT_DATA_RECEIVED,
    NET_EVENT_NONE
} net_event_t;

typedef struct {
    net_event_t event;
    int32_t return_code;
} net_msg_t;

static osMessageQueueId_t net_msg_queue = NULL;

/* to simplify the code of the example the receive buffer is not guarded for concurrent access which can cause
 * corruption in normal use. We allocate one extra byte to guarantee null terminator in string */
static char receive_buffer[MAX_DATA_SENT + 1] = {0};

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

/** Task used to receive packets. Does not normally print and will send messages to app task instead.
 *
 * @param arg not used.
 */
static void socket_receive_task(void *arg)
{
    const int32_t recv_socket_id = *((int32_t *)arg);
    int32_t err = 0;

    while (err >= 0) {
        /* string is always safe to print as we never overwrite the last null character in receive_buffer */
        err = iotSocketRecv(recv_socket_id, receive_buffer, MAX_DATA_SENT);

        const net_msg_t msg = {.event = NET_EVENT_DATA_RECEIVED, .return_code = err};
        if (osMessageQueuePut(net_msg_queue, &msg, 0, 0) != osOK) {
            printf("Failed to send message to net_msg_queue\r\n");
        }
    }
}

/** Task responsible for creating sockets and sending data. Spawns a thread to receive data.
 *
 * @param arg not used.
 */
void app_task(void *arg)
{
    (void)arg;

    const uint8_t loopback_address[IP6ADDR_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    const uint16_t port = 2022;

    int32_t send_socket_id = IOT_SOCKET_ESOCK;
    int32_t recv_socket_id = IOT_SOCKET_ESOCK;
    osThreadId_t recv_thread = 0;
    int32_t err = 0;

    while (1) {
        net_msg_t msg;
        if (osMessageQueueGet(net_msg_queue, &msg, NULL, 1000) != osOK) {
            msg.event = NET_EVENT_NONE;
        }

        switch (msg.event) {
            case NET_EVENT_NETWORK_DOWN: {
                printf("Network is down\r\n");

                /* if sockets were created, close them */
                if (send_socket_id >= 0) {
                    send_socket_id = iotSocketClose(send_socket_id);
                    recv_socket_id = iotSocketClose(recv_socket_id);
                    if (send_socket_id || recv_socket_id) {
                        printf("Failed to close socket\r\n");
                        /* stack is in a broken state and this will not reattempt opening the socket
                         * hence we don't fix the socket ids as they are already less than zero */
                    } else {
                        printf("Sockets closed\r\n");
                    }
                    if (recv_thread) {
                        osThreadTerminate(recv_thread);
                        recv_thread = 0;
                    }
                }
            } break;

            case NET_EVENT_NETWORK_UP: {
                printf("Network is up\r\n");

                /* if sockets already exist nothing to do */
                if (send_socket_id != IOT_SOCKET_ESOCK) {
                    break;
                }

                /* create send and receive sockets */
                send_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET6, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
                recv_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET6, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);

                if (send_socket_id < 0 || recv_socket_id < 0) {
                    printf("Failed to create socket (errors: %" PRId32 " %" PRId32 ")\r\n",
                           send_socket_id,
                           recv_socket_id);
                    err = IOT_SOCKET_ERROR;
                } else {
                    printf("UDP sockets created: %" PRId32 " and %" PRId32 "\r\n", send_socket_id, recv_socket_id);
                }

                /* bind receive socket */
                if (!err) {
                    const uint8_t address_any[IP6ADDR_SIZE] = {0};
                    err = iotSocketBind(recv_socket_id, (uint8_t *)address_any, IP6ADDR_SIZE, port);

                    if (err) {
                        printf("Failed to bind socket (error: %" PRId32 ")\r\n", err);
                    } else {
                        printf("Socket %" PRId32 " bound\r\n", recv_socket_id);
                    }
                }

                /* create receive thread */
                if (!err) {
                    recv_thread = osThreadNew(socket_receive_task, &recv_socket_id, NULL);

                    if (!recv_thread) {
                        printf("Failed to create receive thread\r\n");
                        err = IOT_SOCKET_ERROR;
                    }
                }

                /* cleanup sockets if we failed */
                if (err) {
                    if (send_socket_id) {
                        send_socket_id = iotSocketClose(send_socket_id);
                        if (send_socket_id == 0) {
                            send_socket_id = IOT_SOCKET_ESOCK;
                        }
                    }
                    if (recv_socket_id) {
                        recv_socket_id = iotSocketClose(recv_socket_id);
                        if (recv_socket_id == 0) {
                            recv_socket_id = IOT_SOCKET_ESOCK;
                        }
                    }
                }
            } break;

            case NET_EVENT_DATA_RECEIVED: {
                if (msg.return_code < 0) {
                    printf("Failed to receive data on socket (error: %" PRId32 ")\r\n", msg.return_code);
                } else {
                    printf("Data received: %s\r\n", receive_buffer);
                }
            } break;

            default:
                /* nothing happened, maybe we dropped a packet, if the socket is up we will send again */
                break;
        }

        /* if sockets exist try sending some data */
        if (send_socket_id >= 0) {
            const char data[MAX_DATA_SENT] = "test";
            const int32_t data_size = sizeof(data);

            err = iotSocketSendTo(send_socket_id, data, data_size, (uint8_t *)loopback_address, IP6ADDR_SIZE, port);

            if (err < 0) {
                printf("Failed to send data to socket (error: %" PRId32 ")\r\n", err);
            } else {
                printf("%" PRId32 " bytes sent on socket %" PRId32 "\r\n", err, send_socket_id);

                if (err != data_size) {
                    printf("Expected to send %" PRId32 " bytes\r\n", data_size);
                }
            }
        }

        /* slow down the rate */
        osDelay(1000);
    }
}

int main()
{
    osStatus_t ret = osKernelInitialize();

    if (ret != osOK) {
        printf("osKernelInitialize failed: %d\r\n", ret);
        return -1;
    }

    net_msg_queue = osMessageQueueNew(10, sizeof(net_msg_t), NULL);

    if (!net_msg_queue) {
        printf("Cannot create a queue, exiting\r\n");
        return -1;
    }

    osThreadId_t app_thread = osThreadNew(app_task, NULL, NULL);

    if (!app_thread) {
        printf("Failed to create thread\r\n");
        return -1;
    }

    printf("Initialising network\r\n");

    ret = start_network_task(network_event_callback, 0);

    if (ret != osOK) {
        printf("Failed to start lwip\r\n");
        return -1;
    }

    osKernelState_t state = osKernelGetState();

    if (state != osKernelReady) {
        printf("Kernel not ready: %d\r\n", state);
        return -1;
    }

    printf("Starting kernel\r\n");

    ret = osKernelStart();

    if (ret != osOK) {
        printf("Failed to start kernel: %d\r\n", ret);
        return -1;
    }

    return 0;
}
