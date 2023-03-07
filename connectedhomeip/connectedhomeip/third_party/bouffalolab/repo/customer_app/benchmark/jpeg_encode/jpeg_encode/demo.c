#include <stdio.h>
#include <cli.h>
#include <stdint.h>
#include <utils_log.h>
#include <sys/fcntl.h>
#include <vfs.h>
#include "jpec.h"
#include "bl_timer.h"

uint32_t start_us = 0;

static void jpec_test_cmd(char *buf, int len, int argc, char **argv)
{
    int w, h, length;
    uint8_t *data;
    int fd;

    if (argc == 4)
    {
        fd = aos_open(argv[1], 0);

        if(!fd)
        {
            return;
        }

        w = atoi(argv[2]);
        h = atoi(argv[3]);

        if ((w % 7 == 0) || (h % 7 == 0))
        {
            printf("The number of pixels cannot be an integer multiple of 7\r\n");
            return;
        }
        data = malloc(w * h);

        if (!data)
        {
            printf("Image size %d exceeds limit\r\n", (w * h));
            return;
        }

        length = aos_read(fd, data, w * h);

        if (length == w * h)
        {
            printf("Load %d byte data succ\r\n", length);

            start_us = bl_timer_now_us();
            jpec_enc_t *e = jpec_enc_new(data, w, h);
            const uint8_t *jpeg = jpec_enc_run(e, &length);

            printf("Encoder done (%d bytes), time: %d us\r\n", length, (bl_timer_now_us() - start_us));
            jpec_enc_del(e);
            free(data);
        }
        else
        {
            printf("Load %d byte data fail\r\n", length);
        }
    }
}


// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "jpec", "jpeg encode test", jpec_test_cmd },
};

int test_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

