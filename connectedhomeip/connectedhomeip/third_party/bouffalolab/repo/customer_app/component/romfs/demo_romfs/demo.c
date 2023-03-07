#include <stdio.h>
#include <cli.h>
#include <bl_romfs.h>
#include "demo.h"

void log_step(const char *step[2])
{
    printf("%s   %s\r\n", step[0], step[1]);
}

void cmd_romfs(char *buf, int len, int argc, char **argv)
{
    test1_romfs();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"romfs", "romfs test", cmd_romfs}, 
};
int romfs_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    return 0;
}

