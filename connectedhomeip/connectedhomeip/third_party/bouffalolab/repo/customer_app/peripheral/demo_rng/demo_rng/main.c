/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <FreeRTOS.h>
#include <task.h>

#include <aos/kernel.h>
#include <aos/yloop.h>
#include <cli.h>
#include <blog.h>

static void cmd_hosal_rng(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_rng(void);
    demo_hosal_rng();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_rng", "demo_hosal_rng_test", cmd_hosal_rng},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal rng!\r\n");

    return 0;
}
