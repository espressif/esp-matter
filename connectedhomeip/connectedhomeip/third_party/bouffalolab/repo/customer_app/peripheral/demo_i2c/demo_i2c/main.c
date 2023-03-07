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

static void cmd_hosal_i2c_master_mem(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_i2c_master_mem(void);
    demo_hosal_i2c_master_mem();
}

static void cmd_hosal_i2c_master(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_i2c_master(void);
    demo_hosal_i2c_master();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_i2c_mater_mem", "demo_hosal_i2c_mater_mem", cmd_hosal_i2c_master_mem},
    {"demo_hosal_i2c_master", "demo_hosal_i2c_mater", cmd_hosal_i2c_master},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal i2c!\r\n");
    return 0;
}
