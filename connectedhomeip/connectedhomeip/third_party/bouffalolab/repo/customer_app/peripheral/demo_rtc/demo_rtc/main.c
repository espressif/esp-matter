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

static void cmd_hosal_rtc_DEC(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_rtc_DEC(void);
    demo_hosal_rtc_DEC();
}

static void cmd_hosal_rtc_BCD(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_rtc_BCD(void);
    demo_hosal_rtc_BCD();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_rtc_DEC", "demo_hosal_rtc_DEC_format", cmd_hosal_rtc_DEC},
    {"demo_hosal_rtc_BCD", "demo_hosal_rtc_BCD_format", cmd_hosal_rtc_BCD},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal rtc!\r\n");

    return 0;
}
