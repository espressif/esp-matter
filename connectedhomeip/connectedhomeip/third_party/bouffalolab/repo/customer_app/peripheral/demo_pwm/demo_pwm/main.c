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

static void cmd_hosal_pwm_init(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_pwm_init(void);
    demo_hosal_pwm_init();
}

static void cmd_hosal_pwm_start(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_pwm_start(void);
    demo_hosal_pwm_start();
}

static void cmd_hosal_pwm_change_param(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_pwm_change_param(void);
    demo_hosal_pwm_change_param();
}

static void cmd_hosal_pwm_stop(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_pwm_stop(void);
    demo_hosal_pwm_stop();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_pwm_init", "demo_pwm_hosal_init", cmd_hosal_pwm_init},
    {"demo_hosal_pwm_start", "demo_pwm_hosal_start", cmd_hosal_pwm_start},
    {"demo_hosal_pwm_change_param", "demo_pwm_hosal_change_param", cmd_hosal_pwm_change_param},
    {"demo_hosal_pwm_stop", "demo_hosal_pwm_stop", cmd_hosal_pwm_stop},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal pwm!\r\n");
    return 0;
}
