#include <stdio.h>
#include <stdlib.h>
#include <wifi_bt_coex.h>
#include <cli.h>

static void cmd_coex_dump_all(char *buf, int len, int argc, char **argv)
{
    wifi_bt_coex_dump_all();
}

static void cmd_coex_wlan(char *buf, int len, int argc, char **argv)
{
    wifi_bt_coex_force_wlan();
}

static void cmd_coex_bt(char *buf, int len, int argc, char **argv)
{
    wifi_bt_coex_force_bt();
}

static void cmd_coex_mode(char *buf, int len, int argc, char **argv)
{
    int mode;
    if (2 != argc) {
        return;
    }
    mode = atoi(argv[1]);

    switch (mode) {
        case WIFI_BT_COEX_FORCE_MODE_PTA_FORCE:
        {
            printf("Use COEX Mode: WIFI_BT_COEX_FORCE_MODE_PTA_FORCE\r\n");
            wifi_bt_coex_force_mode(mode);
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        {
            printf("Use COEX Mode: WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE\r\n");
            wifi_bt_coex_force_mode(mode);
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        {
            printf("Use COEX Mode: WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE\r\n");
            wifi_bt_coex_force_mode(mode);
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        {
            printf("Use COEX Mode: WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2\r\n");
            wifi_bt_coex_force_mode(mode);
        }
        break;
        default:
        {
            /*Empty Here*/
            printf("unknown coex mode %d\r\n", mode);
        }
    }
}


// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "coex_dump_all", "COEX Dump All Status", cmd_coex_dump_all},
        { "coex_wlan", "coex force wlan", cmd_coex_wlan},
        { "coex_bt", "coex force bt", cmd_coex_bt},
        { "coex_mode", "coex mode", cmd_coex_mode},
};                                                                                   

int wifi_bt_coex_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
