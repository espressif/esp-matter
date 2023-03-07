#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <cli.h>
#include <blog.h>
#include "demo.h"
#include <hosal_uart.h>

HOSAL_UART_DEV_DECL(uart_stdio1, 1, 4, 3, 2000000);

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
    test_cli_init();
}

static void proc_main_entry(void *pvParameters)
{
    _cli_init();

    vTaskDelete(NULL);
}

void main()
{
    /*Init UART In the first place*/
    hosal_uart_init(&uart_stdio1);
    puts("Starting bl602 now....\r\n");

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);

}
