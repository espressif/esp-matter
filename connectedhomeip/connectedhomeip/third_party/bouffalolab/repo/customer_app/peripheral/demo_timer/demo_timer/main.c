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


static void cmd_hosal_timer_start(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_timer_start(void);
    demo_hosal_timer_start();
}

static void cmd_hosal_timer_stop(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_timer_stop(void);
    demo_hosal_timer_stop();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_timer_start", "demo_pwm_hosal_start", cmd_hosal_timer_start},
    {"demo_hosal_timer_stop", "demo_pwm_hosal_stop", cmd_hosal_timer_stop},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal timer!\r\n");
    return 0;
}
