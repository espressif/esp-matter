/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <cli.h>
#include <aos/kernel.h>

#include <lwip/sockets.h>

#include <netutils/netutils.h>

#define TCPC_PORT_CONNECT  (7000)
#define DEBUG_HEADER "[TCPC] [THREAD] "
#define TCPC_BUFFER_SIZE (1024)

static void tcpc_entry(void* arg)
{
    static int counter_shared = 0;

    char *host = (char *)arg;
    int counter, counter_data;
    int sock, ret;
    uint8_t *send_buf;
    struct sockaddr_in addr;

    counter  = counter_shared++;
    printf(DEBUG_HEADER "[%d] host is %s, counter is %d\r\n", counter, host, counter);
    send_buf = (uint8_t *)pvPortMalloc(TCPC_BUFFER_SIZE);
    if (NULL == send_buf) {
        printf(DEBUG_HEADER "[%d] [%d] NO mem for holding data\r\n", counter, counter_shared);
        goto failed_nothing;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
        printf(DEBUG_HEADER "[%d] [%d] create socket failed, ret is sock %d!\r\n", counter, counter_shared, sock);
        goto failed_buffer;
    }
    addr.sin_family = PF_INET;
    addr.sin_port = htons(TCPC_PORT_CONNECT);
    addr.sin_addr.s_addr = inet_addr(host);

    ret = connect(sock, (const struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        printf(DEBUG_HEADER "[%d] [%d] Connect failed!\n", counter, counter_shared);
        closesocket(sock);
        goto failed_buffer;
    }
    printf(DEBUG_HEADER "[%d] [%d] Connect to iperf server successful!\n", counter, counter_shared);

    {
        int flag = 1;

        setsockopt(sock,
            IPPROTO_TCP,     /* set option at TCP level */
            TCP_NODELAY,     /* name of option */
            (void *)&flag,  /* the cast is historical cruft */
            sizeof(int));    /* length of option value */
    }

    memset(send_buf, 0, TCPC_BUFFER_SIZE);
    counter_data = 0;
    while (1) {
        snprintf((char*)send_buf, TCPC_BUFFER_SIZE, "Data Seq %d", counter_data++);
        ret = write(sock, send_buf, TCPC_BUFFER_SIZE);
        if (ret < 0) {
            printf(DEBUG_HEADER "[%d] [%d] send failed with ret %d\r\n", counter, counter_shared, ret);
            break;
        } else {
            printf(DEBUG_HEADER "[%d] [%d] send success with ret %d, %s\r\n", counter, counter_shared, ret, send_buf);
        }
        ret = read(sock, send_buf, TCPC_BUFFER_SIZE);
        if (ret < 0) {
            printf(DEBUG_HEADER "[%d] [%d] recv failed with ret %d\r\n", counter, counter_shared, ret);
            break;
        } else {
            printf(DEBUG_HEADER "[%d] [%d] recv success with ret %d, %s\r\n", counter, counter_shared, ret, send_buf);
        }
        vTaskDelay(1000);
    }
    closesocket(sock);


failed_buffer:
    vPortFree(send_buf);
failed_nothing:
    vPortFree(arg);
#if 0
    int i;
    int ret;

    int sentlen;
    uint32_t tick1, tick2;


    for (i = 0; i < IPERF_BUFSZ; i ++)
        send_buf[i] = i & 0xff;

    while (param.mode != IPERF_MODE_STOP) 
    {



        {
            int flag = 1;

            setsockopt(sock,
                IPPROTO_TCP,     /* set option at TCP level */
                TCP_NODELAY,     /* name of option */
                (void *) &flag,  /* the cast is historical cruft */
                sizeof(int));    /* length of option value */
        }

        sentlen = 0;

        tick1 = xTaskGetTickCount();
        while(param.mode != IPERF_MODE_STOP) 
        {
            tick2 = xTaskGetTickCount();
            if (tick2 - tick1 >= 1000 * 5)
            {
                float f;

                f = (float)(sentlen)  / 125 / (tick2 - tick1) * 1000;
                f /= 1000.0f;
                snprintf(speed, sizeof(speed), "%.4f Mbps!\r\n", f);
                printf("%s", speed);
                tick1 = tick2;
                sentlen = 0;
            }

            ret = send(sock, send_buf, IPERF_BUFSZ, 0);
            if (ret > 0) 
            {
                sentlen += ret;
            }

            if (ret < 0) break;
        }

        closesocket(sock);

        vTaskDelay(1000*2);
        printf("disconnected!\r\n");
    }
    printf("iper stop\r\n");
    vPortFree(send_buf);
#endif
}

void tcpclient_cmd(char *buf, int len, int argc, char **argv)
{
    static uint8_t counter = 0;
    int host_len;
    char names[32];
    char *host;

    if (2 != argc) {
        printf("[TCPC] host IP address is required. Ex. 192.168.1.1\r\n");
        return;
    }
    host_len = strlen(argv[1]) + 4;
    host = pvPortMalloc(host_len);//mem will be free in tcpc_entry
    if (NULL == host) {
        printf("[TCPC] run out of mem for host alloc\r\n");
    }
    strcpy(host, argv[1]);
    snprintf(names, sizeof(names), "tcp%u", counter++);
    aos_task_new(names, tcpc_entry, host, 2048);
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "tcpc", "create a tcpc for in a new task", tcpclient_cmd},
};                                                                                   

int network_netutils_tcpclinet_cli_register()
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
