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

#define MAXCLIENTNUM     (5)
#define SERVERPORT      (5150)
#define DATABUFFER      (10 * 1024)

void TCP_Server(void *pvParameters)
{
    int listening_socket, new_connection, i;
    uint32_t    client_addrlen;
    struct sockaddr_in server_addr, client_addr;
    char *data_buffer;
    int yes = 1;
    int connect_time[MAXCLIENTNUM];
    
    int time_cur, time_diff[MAXCLIENTNUM], time_last[MAXCLIENTNUM];
    fd_set fdsr;
    int maxsock;
    struct timeval tv;
    int client_fd[MAXCLIENTNUM];
    int con_amount = 0;     /*client connected number*/
    int ret;

    data_buffer = pvPortMalloc(DATABUFFER);
    memset(data_buffer, 0, DATABUFFER);
    memset(client_fd, 0, sizeof(client_fd));
    memset(time_diff, 0, sizeof(time_diff));
    memset(time_last, 0, sizeof(time_last));
    memset(connect_time, 0, sizeof(connect_time));
    

    if ((listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) < 0)) {
        printf("socket creat failed\r\n");
        vPortFree(data_buffer);
        vTaskDelete(NULL);
    } else {
        printf("socket:%d\r\n", listening_socket);
    }
    if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        printf("setsockopt failed\r\n");
        goto Failed;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVERPORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(listening_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)) {
        printf("bind failed\r\n");
        goto Failed;
    }

    if (listen(listening_socket, MAXCLIENTNUM) < 0) {
    	printf("listen failed\r\n");
        goto Failed;
    }

    printf("We are waiting a connection on port 5150.\r\n");
    printf("Listen(正在监听)...\r\n");

    maxsock = listening_socket;
    client_addrlen = sizeof(client_addr);
    time_cur = xTaskGetTickCount();

    for (i = 0; i < MAXCLIENTNUM; i++) {
        time_last[i] = time_cur;
        connect_time[i] = atoi((char *)pvParameters);
    }

    while(1) {
        FD_ZERO(&fdsr);
        FD_SET(listening_socket, &fdsr);

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        for (i = 0; i < MAXCLIENTNUM; i++) {
            if (client_fd[i] != 0) {
                FD_SET(client_fd[i], &fdsr);
                time_cur = xTaskGetTickCount();
                time_diff[i] = time_cur - time_last[i];
                if (time_diff[i] >= configTICK_RATE_HZ) {
                    time_last[i] = time_cur;
                    connect_time[i]--;
                    if (connect_time[i] <= 0) {
                        closesocket(client_fd[i]);
                        connect_time[i] = atoi((char *)pvParameters);
                        FD_CLR(client_fd[i], &fdsr);
                        client_fd[i] = 0;
                        con_amount--;
                        printf("disconnect becase time out\r\n");
                    }
                }
            }
        }
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0) {
            printf("select failed\r\n");
            goto Failed;
        } else if (ret == 0) {
            continue;
        } else {}
        /*Receive data from client*/
        for (i = 0; i < MAXCLIENTNUM; i++) {
            if (client_fd[i] != listening_socket) {
                if (FD_ISSET(client_fd[i], &fdsr)) {
                    time_cur = xTaskGetTickCount();
                    memset(data_buffer, 0, DATABUFFER);
                    if((ret = recv(client_fd[i], data_buffer, DATABUFFER, 0)) <= 0) {
                        printf("recv failed:%d\r\n", ret);
                        closesocket(client_fd[i]);
                        connect_time[i] = atoi((char *)pvParameters);
                        FD_CLR(client_fd[i], &fdsr);
                        client_fd[i] = 0;
                        con_amount--;
                    } else {
                        connect_time[i] = atoi((char *)pvParameters);   /*retime*/
                        time_last[i] = time_cur;
                        printf("We successfully received %d bytes from %d client\r\n", ret, i);
                        //data_buffer[ret] = '\0';
                        printf("%s\r\n", data_buffer);
                        if ((ret = send(client_fd[i], data_buffer, ret, 0)) <= 0 ) {
                            printf("send failed\r\n");
                            closesocket(client_fd[i]);
                            connect_time[i] = atoi((char *)pvParameters);
                            FD_CLR(client_fd[i], &fdsr);
                            client_fd[i] = 0;
                            con_amount--;
                        } else {
                            printf("We successfully send %d bytes to %d client\r\n", ret, i);
                            printf("\r\n");
                        }
                    }
                }
            }
        }
        /*client connect require*/ 
        if (FD_ISSET(listening_socket, &fdsr)) {
            if ((new_connection = accept(listening_socket, (struct sockaddr *)&client_addr, &client_addrlen)) <= 0) {
                printf("accept failed\r\n");
                continue;
            } else {
                printf("new_sock:%d\r\n", new_connection);
            }

            printf("We successfully got a connection from %s:%d\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));    
            printf("We are waiting to receive data\r\n");

            if (con_amount < MAXCLIENTNUM) {
                for (i = 0; i < MAXCLIENTNUM; i++) {
                    if (0 == client_fd[i]) {
                        time_cur = xTaskGetTickCount();
                        time_last[i] = time_cur;
                        connect_time[i] = atoi((char *)pvParameters);
                        client_fd[i] = new_connection;
                        if (new_connection > maxsock) {
                            maxsock = new_connection;
                        }
                        con_amount++;
                        printf("amount:%d\r\n", con_amount);
                        break;
                    }
                }
            } else { 
                printf("Max connections arrived!\r\n");
                send(new_connection, "Bye", 4, 0);
                closesocket(new_connection);
            }
        }
    }

Failed:
    vPortFree(data_buffer);
    closesocket(listening_socket);
    vTaskDelete(NULL);
}

static void cmd_tcp_server(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        printf("Please Input Parameter!\r\n");
        return;
    }
    xTaskCreate(TCP_Server, "TCP Server", 1024, argv[1], 20, NULL);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "tcps", "create a tcp server for in a new task", cmd_tcp_server},
};                                                                                   

int network_netutils_tcpserver_cli_register()
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
