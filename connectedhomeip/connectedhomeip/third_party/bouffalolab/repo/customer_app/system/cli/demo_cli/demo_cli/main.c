#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <cli.h>
#include <bl_sys_time.h>
#include "demo.h"

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
    bl_sys_time_cli_init();
    test_cli_init();
}

static void proc_main_entry(void *pvParameters)
{
    _cli_init();

    vTaskDelete(NULL);
}

static void system_thread_init()
{
    /*nothing here*/
}

void main()
{
    system_thread_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
}
