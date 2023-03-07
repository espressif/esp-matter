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

static void cmd_hosal_adc(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_adc(void);
    demo_hosal_adc();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_adc_signal", "demo_hosal_adc_signal", cmd_hosal_adc},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal dac!\r\n");
    return 0;
}
