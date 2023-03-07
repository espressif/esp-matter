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

static void cmd_hosal_spi_master(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_spi_master(void);
    demo_hosal_spi_master();
}

static void cmd_hosal_spi_slave(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_spi_slave(void);
    demo_hosal_spi_slave();
}

static void cmd_hosal_spi_dma_master(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_spi_dma_master(void);
    demo_hosal_spi_dma_master();
}

static void cmd_hosal_spi_dma_slave(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_spi_dma_slave(void);
    demo_hosal_spi_dma_slave();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"demo_hosal_spi_master", "demo_hosal_spi_master", cmd_hosal_spi_master},
    {"demo_hosal_spi_slave", "demo_hosal_spi_slave", cmd_hosal_spi_slave},
    {"demo_hosal_spi_dma_master", "demo_hosal_spi_dma_master", cmd_hosal_spi_dma_master},
    {"demo_hosal_spi_dma_slave", "demo_hosal_spi_dma_slave", cmd_hosal_spi_dma_slave},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal spi!\r\n");
    return 0;
}
