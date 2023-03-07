#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <easyflash.h>
#include <bl_timer.h>

#define EASYFLASH_BOOT_TIMES     "boot_times"

static void _cli_init()
{
    easyflash_cli_init();
}

static void __easyflash_first_boottimes()
{
    uint32_t timer_us;

    timer_us = bl_timer_now_us();
    ef_set_env(EASYFLASH_BOOT_TIMES, "0");
    ef_save_env();
    timer_us = bl_timer_now_us() - timer_us;
    printf("easyflash write boot_times us %ld\r\n", timer_us);
    printf("The system now boot times 0\r\n");
}

static void __easyflash_boottimes_dump()
{
    char *times = NULL;
    uint32_t times_num = 0;
    char env_set[12] = {0};

    uint32_t timer_us;

    timer_us = bl_timer_now_us();
    times = ef_get_env(EASYFLASH_BOOT_TIMES);
    timer_us = bl_timer_now_us() - timer_us;
    printf("easyflash read boot_times us %ld\r\n", timer_us);
    
    if (times == NULL) {
        __easyflash_first_boottimes();
        return;
    }
    times_num = atoi(times);
    sprintf(env_set, "%ld", ++times_num);
    
    timer_us = bl_timer_now_us();
    ef_set_env(EASYFLASH_BOOT_TIMES, env_set);
    ef_save_env();
    timer_us = bl_timer_now_us() - timer_us;
    printf("easyflash write boot_times us %ld\r\n", timer_us);
    
    printf("The system now boot times %ld\r\n", times_num);
}

static void proc_main_entry(void *pvParameters)
{
    uint32_t timer_us = bl_timer_now_us();
    easyflash_init();
    timer_us = bl_timer_now_us();
    printf("easyflash init time us %ld\r\n", timer_us);
    __easyflash_boottimes_dump();
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
