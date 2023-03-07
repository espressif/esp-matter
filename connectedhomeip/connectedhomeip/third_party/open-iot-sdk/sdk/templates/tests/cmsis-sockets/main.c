/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis_os2.h"
#include "iot_socket.h"
#include "iotsdk/ip_network_api.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool test_failed = false;

/* these should be replaced with green tea test defines */
#define TEST_ASSERT_MESSAGE(condition, message) \
    if ((condition)) { /*OK*/                   \
        ;                                       \
    } else {                                    \
        printf(message);                        \
        test_failed = true;                     \
    }
#define TEST_ASSERT(condition)                                    \
    if ((condition)) { /*OK*/                                     \
        ;                                                         \
    } else {                                                      \
        printf("FAIL (" #condition ") on line %d\r\n", __LINE__); \
        test_failed = true;                                       \
    }
#define TEST_ASSERT_TRUE(condition)                               \
    if ((condition)) { /*OK*/                                     \
        ;                                                         \
    } else {                                                      \
        printf("FAIL (" #condition ") on line %d\r\n", __LINE__); \
        test_failed = true;                                       \
    }
#define TEST_ASSERT_FALSE(condition)                              \
    if ((condition)) {                                            \
        printf("FAIL (" #condition ") on line %d\r\n", __LINE__); \
        test_failed = true;                                       \
    } else { /*OK*/                                               \
        ;                                                         \
    }
#define TEST_ASSERT_NULL(condition)                               \
    if ((condition)) {                                            \
        printf("FAIL (" #condition ") on line %d\r\n", __LINE__); \
        test_failed = true;                                       \
    } else { /*OK*/                                               \
        ;                                                         \
    }
#define TEST_ASSERT_NOT_NULL(condition)                           \
    if ((condition)) { /*OK*/                                     \
        ;                                                         \
    } else {                                                      \
        printf("FAIL (" #condition ") on line %d\r\n", __LINE__); \
        test_failed = true;                                       \
    }
#define TEST_FAIL_MESSAGE(message)        \
    printf("TEST FAIL: %s\r\n", message); \
    test_failed = true;
#define TEST_FAIL()                               \
    printf("TEST FAIL on line %d\r\n", __LINE__); \
    test_failed = true;

#define MAX_DATA_SENT 5

const uint32_t ADDRESS_ANY = 0;
const uint32_t INADDR_LOOPBACK = 0x100007FUL;
const uint16_t server_port = 4242;

typedef enum test_event_t {
    TEST_EVENT_NETWORK_UP,
    TEST_EVENT_NETWORK_DOWN,
    TEST_EVENT_DATA_RECEIVED,
    TEST_EVENT_READY,
    TEST_EVENT_NONE
} test_event_t;

typedef struct {
    test_event_t event;
    int32_t return_code;
} test_msg_t;

static osMessageQueueId_t test_msg_queue = NULL;
static osMessageQueueId_t test_runner_msg_queue = NULL;

static char receive_buffer[MAX_DATA_SENT] = {0};
static const char send_data[MAX_DATA_SENT] = "test";
static const int32_t send_data_size = sizeof(send_data);

/* helper functions */

static void network_event_callback(network_state_callback_event_t event)
{
    const test_msg_t msg = {.event = (event == NETWORK_UP) ? TEST_EVENT_NETWORK_UP : TEST_EVENT_NETWORK_DOWN};
    if (osMessageQueuePut(test_msg_queue, &msg, 0, 0) != osOK) {
        TEST_FAIL_MESSAGE("Failed to send message to test_msg_queue\r\n");
    }
}

static void get_msg(test_msg_t *msg)
{
    while (osMessageQueueGet(test_msg_queue, msg, NULL, 0) != osOK) {
        ;
    }

    if (msg->event == TEST_EVENT_NETWORK_DOWN) {
        TEST_FAIL_MESSAGE("Network went down\r\n");
    }
}

/** will not return until given event received */
static void wait_for_event(test_event_t event)
{
    test_msg_t msg;
    while (1) {
        get_msg(&msg);
        if (msg.event == event) {
            return;
        }
    }
}

static void send_event(test_event_t event)
{
    const test_msg_t msg = {.event = event, .return_code = 0};
    if (osMessageQueuePut(test_msg_queue, &msg, 0, 0) != osOK) {
        TEST_FAIL_MESSAGE("Failed to send message to test_msg_queue\r\n");
    }
}

static void test_end()
{
    const test_msg_t msg = {.event = TEST_EVENT_READY, .return_code = 0};
    if (osMessageQueuePut(test_runner_msg_queue, &msg, 0, 0) != osOK) {
        TEST_FAIL_MESSAGE("Failed to send message to test_runner_msg_queue\r\n");
    }

    osThreadTerminate(osThreadGetId());
}

/* tests */

static void open_close_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    int32_t test_socket_id = IOT_SOCKET_ESOCK;

    test_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
    TEST_ASSERT(test_socket_id >= 0);

    test_socket_id = iotSocketClose(test_socket_id);
    TEST_ASSERT(test_socket_id == 0);

    test_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    TEST_ASSERT(test_socket_id >= 0);

    osStatus_t ret = iotSocketClose(test_socket_id);
    TEST_ASSERT(ret == osOK);

    test_end();
}

static void bind_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    osStatus_t ret = 0;
    int32_t test_socket_id = IOT_SOCKET_ESOCK;
    const uint16_t port = 2022;

    test_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    TEST_ASSERT(test_socket_id >= 0);

    ret = iotSocketBind(test_socket_id, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), port);
    TEST_ASSERT(ret == osOK);

    /* binding again should fail */
    ret = iotSocketBind(test_socket_id, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), port);
    TEST_ASSERT(ret != osOK);

    ret = iotSocketClose(test_socket_id);
    TEST_ASSERT(ret == osOK);

    test_end();
}

static void invalid_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    osStatus_t ret = 0;
    int32_t test_socket_id = IOT_SOCKET_ESOCK;
    const uint16_t port = 2022;

    ret = iotSocketBind(test_socket_id, (uint8_t *)&ADDRESS_ANY, sizeof(ADDRESS_ANY), port);
    TEST_ASSERT(ret == IOT_SOCKET_ESOCK);

    ret = iotSocketClose(test_socket_id);
    TEST_ASSERT(ret == IOT_SOCKET_ESOCK);

    test_end();
}

static void options_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    int32_t test_socket_id = IOT_SOCKET_ESOCK;

    test_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
    TEST_ASSERT(test_socket_id >= 0);

    const uint32_t opt_val = 1;
    osStatus_t ret = iotSocketSetOpt(test_socket_id, IOT_SOCKET_SO_KEEPALIVE, &opt_val, sizeof(opt_val));
    TEST_ASSERT(ret == osOK);

    uint32_t get_opt_val = 0;
    uint32_t size_of_opt_val = sizeof(get_opt_val);
    ret = iotSocketGetOpt(test_socket_id, IOT_SOCKET_SO_KEEPALIVE, &get_opt_val, &size_of_opt_val);
    TEST_ASSERT(size_of_opt_val == sizeof(get_opt_val));
    TEST_ASSERT(ret == osOK);
    TEST_ASSERT(get_opt_val != 0);

    ret = iotSocketClose(test_socket_id);
    TEST_ASSERT(ret == osOK);

    test_end();
}

static void send_receive_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    int32_t send_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
    int32_t recv_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_DGRAM, IOT_SOCKET_IPPROTO_UDP);
    TEST_ASSERT(send_socket_id >= 0);
    TEST_ASSERT(recv_socket_id >= 0);

    const uint16_t send_port = 2022;
    osStatus_t ret = iotSocketBind(recv_socket_id, (uint8_t *)&ADDRESS_ANY, sizeof(ADDRESS_ANY), send_port);
    TEST_ASSERT(ret == osOK);

    /* send to */
    ret = iotSocketSendTo(
        send_socket_id, send_data, send_data_size, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), send_port);
    TEST_ASSERT(ret == send_data_size);

    /* recv from */
    uint32_t address = 0;
    uint16_t port = 0;
    uint32_t size_of_ip = sizeof(INADDR_LOOPBACK);
    ret = iotSocketRecvFrom(recv_socket_id, receive_buffer, MAX_DATA_SENT, (uint8_t *)&address, &size_of_ip, &port);
    TEST_ASSERT(ret == send_data_size);
    TEST_ASSERT(address == INADDR_LOOPBACK);
    TEST_ASSERT(port > 0);
    TEST_ASSERT(size_of_ip == sizeof(INADDR_LOOPBACK));
    TEST_ASSERT(memcmp(receive_buffer, send_data, MAX_DATA_SENT) == 0);

    /* recv without params */
    ret = iotSocketSendTo(
        send_socket_id, send_data, send_data_size, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), send_port);
    TEST_ASSERT(ret == send_data_size);
    ret = iotSocketRecv(recv_socket_id, receive_buffer, MAX_DATA_SENT);
    TEST_ASSERT(ret == send_data_size);

    ret = iotSocketClose(send_socket_id);
    TEST_ASSERT(ret == osOK);
    ret = iotSocketClose(recv_socket_id);
    TEST_ASSERT(ret == osOK);

    test_end();
}

static void get_host_name_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    uint32_t address = 0;
    uint32_t size_of_ip = sizeof(INADDR_LOOPBACK);
    const char name[] = "arm.com";
    uint32_t ret = iotSocketGetHostByName(name, IOT_SOCKET_AF_INET, (uint8_t *)&address, &size_of_ip);
    TEST_ASSERT(ret == osOK);
    TEST_ASSERT(address != 0);
    TEST_ASSERT(size_of_ip == sizeof(INADDR_LOOPBACK));

    test_end();
}

/** Returns connected socket. */
static int32_t wait_on_connection(int32_t recv_socket_id)
{
    int32_t ret = iotSocketBind(recv_socket_id, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), server_port);
    TEST_ASSERT(ret == osOK);

    send_event(TEST_EVENT_READY);

    ret = iotSocketListen(recv_socket_id, 1);
    TEST_ASSERT(ret == osOK);
    uint32_t address = 0;
    uint16_t port = 0;
    uint32_t size_of_ip = sizeof(INADDR_LOOPBACK);

    int32_t connected_socket = iotSocketAccept(recv_socket_id, (uint8_t *)&address, &size_of_ip, &port);
    TEST_ASSERT(ret == osOK);
    TEST_ASSERT(address == INADDR_LOOPBACK);
    TEST_ASSERT(port > 0);
    TEST_ASSERT(size_of_ip == sizeof(INADDR_LOOPBACK));

    return connected_socket;
}

static void connect_listen_accept_recv_task(void *arg)
{
    const int32_t recv_socket_id = *((int32_t *)arg);
    int32_t ret = 0;

    int32_t connected_socket = wait_on_connection(recv_socket_id);

    uint32_t address = 0;
    uint16_t port = 0;
    uint32_t size_of_ip = sizeof(INADDR_LOOPBACK);
    iotSocketGetSockName(recv_socket_id, (uint8_t *)&address, &size_of_ip, &port);
    TEST_ASSERT(ret == osOK);
    TEST_ASSERT(address == INADDR_LOOPBACK);
    TEST_ASSERT(port > 0);
    TEST_ASSERT(size_of_ip == sizeof(INADDR_LOOPBACK));

    uint16_t other_port = 0;
    iotSocketGetPeerName(connected_socket, (uint8_t *)&address, &size_of_ip, &other_port);
    TEST_ASSERT(ret == osOK);
    TEST_ASSERT(address == INADDR_LOOPBACK);
    TEST_ASSERT(other_port > 0);
    TEST_ASSERT(size_of_ip == sizeof(INADDR_LOOPBACK));

    int32_t received = 0;
    while (ret >= 0) {
        ret = iotSocketRecv(connected_socket, receive_buffer, MAX_DATA_SENT);
        TEST_ASSERT(ret >= 0);
        received += ret;

        if (received == send_data_size) {
            break;
        }
    }

    ret = iotSocketClose(connected_socket);
    TEST_ASSERT(ret == osOK);

    /* Signal to the sender thread the completion of the task */
    send_event(TEST_EVENT_READY);

    osThreadTerminate(osThreadGetId());
}

static void connect_listen_accept_test(void *arg)
{
    (void)arg;
    wait_for_event(TEST_EVENT_NETWORK_UP);

    int32_t send_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    int32_t recv_socket_id = iotSocketCreate(IOT_SOCKET_AF_INET, IOT_SOCKET_SOCK_STREAM, IOT_SOCKET_IPPROTO_TCP);
    TEST_ASSERT(send_socket_id >= 0);
    TEST_ASSERT(recv_socket_id >= 0);

    osThreadId_t recv_thread = osThreadNew(connect_listen_accept_recv_task, &recv_socket_id, NULL);
    TEST_ASSERT_NOT_NULL(recv_thread);

    wait_for_event(TEST_EVENT_READY);
    /* wait for listen */
    osDelay(100);

    int32_t ret = iotSocketConnect(send_socket_id, (uint8_t *)&INADDR_LOOPBACK, sizeof(INADDR_LOOPBACK), server_port);
    TEST_ASSERT(ret == osOK);

    ret = iotSocketSend(send_socket_id, send_data, send_data_size);
    TEST_ASSERT(ret == send_data_size);

    /* wait until receive task completes */
    wait_for_event(TEST_EVENT_READY);

    ret = iotSocketClose(send_socket_id);
    TEST_ASSERT(ret == osOK);
    ret = iotSocketClose(recv_socket_id);
    TEST_ASSERT(ret == osOK);

    /* Note: the receiver thread is detached by default. It isn't required to
       terminate it */

    test_end();
}

/* replace with greentea */
typedef struct Case {
    const char *name;
    void (*test)(void *);
} Case;

static const Case cases[] = {
    {"open and close socket", open_close_test},
    {"bind socket", bind_test},
    {"using invalid socket", invalid_test},
    {"socket options", options_test},
    {"send and receive", send_receive_test},
    {"get host name", get_host_name_test},
    {"connect, listen, accept", connect_listen_accept_test},
};

static void test_runner_task()
{
    const size_t num_of_tests = (sizeof(cases) / sizeof(Case));

    printf("Running %" PRIuPTR " tests\r\n\r\n", num_of_tests);

    for (size_t i = 0; i < num_of_tests; i++) {
        osDelay(100);
        printf("Running Test:    %s\r\n", cases[i].name);

        osThreadId_t test_thread = osThreadNew(cases[i].test, NULL, NULL);
        TEST_ASSERT_NOT_NULL(test_thread);

        test_msg_t msg;
        while (osMessageQueueGet(test_runner_msg_queue, &msg, NULL, 0) != osOK) {
            ;
        }

        TEST_ASSERT(msg.event == TEST_EVENT_READY);

        if (test_failed) {
            printf("Test:            %s    FAILED\r\n\r\n", cases[i].name);
            break;
        } else {
            printf("Test:            %s    PASSED\r\n\r\n", cases[i].name);
        }

        /* All tests are run in sequence, lwip cannot be deinited, real test runner will have to reset between runs. */
        /* To simulate a fresh start we need a new NETWORK_UP event for new test thread to start */
        send_event(TEST_EVENT_NETWORK_UP);
    }

    if (!test_failed) {
        printf("All tests PASSED\r\n");
    }
}

int main()
{
    osStatus_t ret = osKernelInitialize();
    TEST_ASSERT(ret == osOK);

    test_msg_queue = osMessageQueueNew(10, sizeof(test_msg_t), NULL);
    TEST_ASSERT_NOT_NULL(test_msg_queue);

    test_runner_msg_queue = osMessageQueueNew(2, sizeof(test_msg_t), NULL);
    TEST_ASSERT_NOT_NULL(test_runner_msg_queue);

    ret = start_network_task(network_event_callback, 0);
    TEST_ASSERT(ret == osOK);

    osThreadId_t test_runner_thread = osThreadNew(test_runner_task, NULL, NULL);
    TEST_ASSERT_NOT_NULL(test_runner_thread);

    osKernelStart();

    return 0;
}
