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

static void cb_httpc_result(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    httpc_state_t **req = (httpc_state_t**)arg;

    printf("[HTTPC] Transfer finished. rx_content_len is %lu\r\n", rx_content_len);
    *req = NULL;
}

err_t cb_httpc_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    printf("[HTTPC] hdr_len is %u, content_len is %lu\r\n", hdr_len, content_len);
    return ERR_OK;
}

static err_t cb_altcp_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    //printf("[HTTPC] Received %u Bytes\r\n", p->tot_len);
    static int count = 0;

    puts(".");
    if (0 == ((count++) & 0x3F)) {
        puts("\r\n");
    }
    altcp_recved(conn, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

static void cmd_httpc_test(char *buf, int len, int argc, char **argv)
{
    static httpc_connection_t settings;
    static httpc_state_t *req;

    if (req) {
        printf("[CLI] req is on-going...\r\n");
        return;
    }
    memset(&settings, 0, sizeof(settings));
    settings.use_proxy = 0;
    settings.result_fn = cb_httpc_result;
    settings.headers_done_fn = cb_httpc_headers_done_fn;

    httpc_get_file_dns(
            "nf.cr.dandanman.com",
            80,
            "/ddm/ContentResource/music/205.mp3",
            &settings,
            cb_altcp_recv_fn,
            &req,
            &req
   );
}


// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"httpc", "http client download test based on RAW TCP", cmd_httpc_test},
};

int httpc_client_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

