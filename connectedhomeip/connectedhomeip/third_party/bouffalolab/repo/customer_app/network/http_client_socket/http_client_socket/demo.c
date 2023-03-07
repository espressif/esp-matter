#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>
#include <http_client.h>
#include <cli.h>
#include "demo.h"

#define MAXBUF          128
#define BUFFER_SIZE     (12*1024)
#define PORT 80

static int client_demo(char *hostname)
{
    int sockfd;
    /* Get host address from the input name */
    struct hostent *hostinfo = gethostbyname(hostname);
    uint8_t *recv_buffer;

    if (!hostinfo) {
        printf("gethostbyname Failed\r\n");
        return -1;
    }

    struct sockaddr_in dest;

    char buffer[MAXBUF];
    /* Create a socket */
    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        return -1;
    }

    /*---Initialize server address/port struct---*/
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr = *((struct in_addr *) hostinfo->h_addr);
//    char ip[16];
    uint32_t address = dest.sin_addr.s_addr;
    char *ip = inet_ntoa(address);

    printf("Server ip Address : %s\r\n", ip);
    /*---Connect to server---*/
    if (connect(sockfd,
             (struct sockaddr *)&dest,
             sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        return -1;
    }
    /*---Get "Hello?"---*/
    memset(buffer, 0, MAXBUF);
    char wbuf[]
        = "GET /ddm/ContentResource/music/204.mp3 HTTP/1.1\r\nHost: nf.cr.dandanman.com\r\nUser-Agent: wmsdk\r\nAccept: */*\r\n\r\n";
    write(sockfd, wbuf, sizeof(wbuf) - 1);

    int ret = 0;
    int total = 0;
    int debug_counter = 0;
    uint32_t ticks_start, ticks_end, time_consumed;

    ticks_start = xTaskGetTickCount();
    recv_buffer = pvPortMalloc(BUFFER_SIZE);
    if (NULL == recv_buffer) {
        goto out;
    }
    while (1) {
        ret = read(sockfd, recv_buffer, BUFFER_SIZE);
        if (ret == 0) {
            printf("eof\n\r");
            break;
        } else if (ret < 0) {
            printf("ret = %d, err = %d\n\r", ret, errno);
            break;
        } else {
            total += ret;
            /*use less debug*/
            if (0 == ((debug_counter++) & 0xFF)) {
                printf("total = %d, ret = %d\n\r", total, ret);
            }
            //vTaskDelay(2);
            if (total > 820500) {
                ticks_end = xTaskGetTickCount();
                time_consumed = ((uint32_t)(((int32_t)ticks_end) - ((int32_t)ticks_start))) / 1000;
                printf("Download comlete, total time %u s, speed %u Kbps\r\n",
                        (unsigned int)time_consumed,
                        (unsigned int)(total / time_consumed * 8 / 1000)
                );
                break;
            }
        }
    }

    vPortFree(recv_buffer);
out:
    close(sockfd);
    return 0;
}

static void http_test_cmd(char *buf, int len, int argc, char **argv)
{
    // http://nf.cr.dandanman.com/ddm/ContentResource/music/204.mp3
    client_demo("nf.cr.dandanman.com");
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"http", "http client download test based on socket", http_test_cmd},
};

int http_client_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

