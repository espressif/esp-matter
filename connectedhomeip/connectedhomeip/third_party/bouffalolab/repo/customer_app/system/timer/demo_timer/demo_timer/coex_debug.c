#include <stdio.h>
#include <cli.h>

void coex_dump_pta(void);
void coex_dump_wifi(void);

static void __attribute__((unused)) cmd_coex_dump(char *buf, int len, int argc, char **argv) 
{
    coex_dump_pta();
    coex_dump_wifi();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "coex_dump", "coex dump", cmd_coex_dump},
};                                                                                   

int codex_debug_cli_init()
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}

