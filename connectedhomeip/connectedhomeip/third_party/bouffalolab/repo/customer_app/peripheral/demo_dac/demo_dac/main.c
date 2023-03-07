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

static void cmd_hosal_dac(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_dac(void);
    demo_hosal_dac();
}

static void cmd_hosal_dac_dma(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_dac_dma(void);
    demo_hosal_dac_dma();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_dac", "demo_hosal_dac", cmd_hosal_dac},
    {"demo_hosal_dac_dma", "demo_hosal_dac_dma", cmd_hosal_dac_dma},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal dac!\r\n");
    return 0;
}
