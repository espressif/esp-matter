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

static void cmd_hosal_wdg_feed(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_wdg_feed(void);
    demo_hosal_wdg_feed();
}

static void cmd_hosal_wdg_not_feed(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_wdg_not_feed(void);
    demo_hosal_wdg_not_feed();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_wdg_feed", "demo_hosal_wdg_feed_every_2s", cmd_hosal_wdg_feed},
    {"demo_hosal_wdg_not_feed", "demo_hosal_wdg_not_feed", cmd_hosal_wdg_not_feed},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal wdg!\r\n");
    return 0;
}
