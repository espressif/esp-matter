#include <stdio.h>
#ifdef CONFIG_CLI_CMD_ENABLE
#include <cli.h>
#endif
#include <easyflash.h>
#include <blog.h>

static void psm_set_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: psm_set [key] [value]\r\n");
        return;
    }
    ef_set_env(argv[1], argv[2]);
    ef_save_env();
}

static void psm_unset_cmd(char *buf, int len, int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: psm_unset [key]\r\n");
        return;
    }
    ef_del_env(argv[1]);
    ef_save_env();
}

static void psm_dump_cmd(char *buf, int len, int argc, char **argv)
{
    ef_print_env();
}

static void psm_erase_cmd(char *buf, int len, int argc, char **argv)
{
    ef_env_set_default();
}

void psm_test_cmd(char *buf, int len, int argc, char **argv)
{
    const char *def_name = "1234567890123456789012345678901234567890123456789012345678901234";
    uint8_t *data_src = NULL;
    uint8_t *data_buf = NULL;
    uint32_t data_len = 0;
    uint32_t tbuf[] = {1, 1024, 2048, 3978, 3979, 3980};
    uint32_t i, j;

    size_t read_len;
    uint32_t res1, res2;

    for (j = 0; j < sizeof(tbuf)/sizeof(tbuf[0]); j++) {
        data_len = tbuf[j];
        read_len = 0;

        data_src = pvPortMalloc(data_len + 1);
        data_buf = pvPortMalloc(data_len + 1);
        if ((NULL == data_src) || (NULL == data_buf)) {
            log_info("kvbin malloc %d byte error\r\n");
            goto kvbin_exit;
        }

        memset(data_src, 0, data_len + 1);  /* for get string */
        memset(data_buf, 0, data_len + 1);  /* for get string */
        for (i = 0; i < data_len; i++) {
            data_src[i] = 'v';
        }

        /* set */
        res1 = ef_set_env_blob(def_name, data_src, data_len);

        /* get */
        res2 = ef_get_env_blob(def_name, data_buf, data_len, &read_len);
        if ((res1 != 0) || (res2 != data_len)) {
            log_warn("kvbin set/get %ld byte error, res1 = %ld, res2 = %d.\r\n", data_len, res1, res2);
            goto kvbin_exit;
        }

        if (memcmp(data_buf, data_src, data_len) != 0) {
            log_error("kvbin set/get %ld byte , res1 = %ld, res2 = %d. memcmp error\r\n", data_len, res1, res2);
            goto kvbin_exit;
        }

        log_info("kvbin set %ld byte bin -> read %ld byte -> memcmp success.\r\n", data_len, data_len);
kvbin_exit:
        if (data_src) {
            vPortFree(data_src);
        }
        if (data_buf) {
            vPortFree(data_buf);
        }
    }

    ef_del_env(def_name);
}

#ifdef CONFIG_CLI_CMD_ENABLE
// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "psm_set", "psm set", psm_set_cmd },
        { "psm_unset", "psm unset", psm_unset_cmd },
        { "psm_dump", "psm dump", psm_dump_cmd },
        { "psm_erase", "psm dump", psm_erase_cmd },
        { "psm_test", "psm test", psm_test_cmd },
};
#endif

int easyflash_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
