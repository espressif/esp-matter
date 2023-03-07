#include <FreeRTOS.h>
#include <cli.h>
#include <task.h>
#include <bl_sys_time.h>
#include <bl_gpio_cli.h>
#include <bl_wdt_cli.h>
#include <fdt.h>
#include <looprt.h>

#include "tjpgd.h"

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"jpgd", "jpgd test", Jpeg_Dec},
};

static void _cli_init() {
    bl_sys_time_cli_init();
    blfdt_cli_init();
    bl_wdt_cli_init();
    bl_gpio_cli_init();
    looprt_test_cli_init();
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

void main() {
    system_thread_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
}
