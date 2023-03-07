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

static void cmd_hosal_uart_int(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_uart_int(int uart_id);

    blog_warn("UART0 cli will not work properly\r\n");
    vTaskDelay(50);
    demo_hosal_uart_int(0);
}

static void cmd_hosal_uart_dma(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_uart_dma(int uart_id);

    blog_warn("UART0 cli will not work properly\r\n");
    vTaskDelay(50);
    demo_hosal_uart_dma(0);
}

static void cmd_hosal_uart_echo(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_uart_echo(int uart_id);

    blog_warn("UART0 cli will not work properly\r\n");
    vTaskDelay(50);
    demo_hosal_uart_echo(0);
}

static void cmd_hosal_uart_abr(char *buf, int len, int argc, char **argv)
{
    extern void demo_hosal_uart_abr(int uart_id);

    blog_warn("UART0 cli will not work properly\r\n");
    vTaskDelay(50);
    demo_hosal_uart_abr(1);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "demo_hosal_uart_echo", "demo UART echo test", cmd_hosal_uart_echo},
    { "demo_hosal_uart_int", "demo UART int test", cmd_hosal_uart_int},
    { "demo_hosal_uart_dma", "demo UART DMA test", cmd_hosal_uart_dma},
    { "demo_hosal_uart_abr", "demo UART abr test", cmd_hosal_uart_abr},
};

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo hosal uart !\r\n");
    return 0;
}
