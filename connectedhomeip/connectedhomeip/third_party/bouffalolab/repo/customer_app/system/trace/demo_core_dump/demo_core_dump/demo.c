#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <cli.h>
#include <stdint.h>
#include "panic.h"
#include "utils_base64.h"
#include "demo.h"


void test_backtrace_api_child_func()
{
	uint32_t testp = 0;

	*(uint32_t *)testp = (uint32_t)0x11223344;

	return;
}

static void test_backtrace_api(char *buf, int len, int argc, char **argv)
{
    test_backtrace_api_child_func();
}

static void test_assert_0(char *buf, int len, int argc, char **argv)
{
    configASSERT(0);
}


// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "bt_test", "test backtrace api", test_backtrace_api },
        { "assert0", "test Assert(0)", test_assert_0 },
};

int test_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));

    return 0;
}

