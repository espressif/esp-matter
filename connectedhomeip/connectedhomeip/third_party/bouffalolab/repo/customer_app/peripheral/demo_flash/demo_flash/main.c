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

static void cmd_hosal_flash(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_flash_test();

    demo_hosal_flash_test();
}

static void cmd_hosal_flash_busaddr(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_flash_busaddr_test();

    demo_hosal_flash_busaddr_test();
}

static void cmd_hosal_flash_raw(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_flash_raw_test();

    demo_hosal_flash_raw_test();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "demo_hosal_flash", "demo flash test", cmd_hosal_flash},
    { "demo_hosal_flash_busaddr", "demo flash busaddr test", cmd_hosal_flash_busaddr},
    { "demo_hosal_flash_raw", "demo flash raw test", cmd_hosal_flash_raw},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal flash !\r\n");

    return 0;
}
