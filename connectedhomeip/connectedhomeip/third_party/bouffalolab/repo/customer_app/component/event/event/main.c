#include <FreeRTOS.h>
#include <task.h>
#include "demo.h"
#include <stdio.h>
#include <cli.h>
#include <bl_sys_time.h>
#include <aos/yloop.h>

#define EV_DEMO             0x0006
#define CODE_DEMO_TEST1     1
#define CODE_DEMO_TEST2     2

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
    bl_sys_time_cli_init();
    event_cli_init();
}

static void event_cb_demo_event(input_event_t *event, void *private_data)
{
    uint16_t code = event->code;
    switch (code) {
        case CODE_DEMO_TEST1:
        {
            printf("######This is event test1\r\n");
        }
        break;
        case CODE_DEMO_TEST2:
        {
            printf("######This is event test2\r\n"); 
        }
        break;
        default:
        {
            /*nothing*/
        }
    }
}

static void delay_action(void *arg) 
{
    printf("######This is event delay_action\r\n");
}

static void proc_main_entry(void *pvParameters)
{
    _cli_init();
    aos_register_event_filter(EV_DEMO, event_cb_demo_event, NULL);
    aos_post_event(EV_DEMO, CODE_DEMO_TEST1, 0);
    aos_post_event(EV_DEMO, CODE_DEMO_TEST2, 0);
    aos_post_delayed_action(5000, delay_action, NULL);
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
