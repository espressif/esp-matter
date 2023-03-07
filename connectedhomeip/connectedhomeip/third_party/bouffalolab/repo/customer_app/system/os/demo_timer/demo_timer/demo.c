#include <stdlib.h>
#include <stdio.h>
#include <cli.h>
#include "demo.h"
#include <hal_hwtimer.h>

#define MTIMER_TICKS_PER_US     (1)

hw_timer_t *handle1;
hw_timer_t *handle2;
hw_timer_t *handle3;
hw_timer_t *handle4;
hw_timer_t *handle5;
hw_timer_t *handle6;
hw_timer_t *handle7;

static uint32_t bl_timer_now_us(void)
{
    uint32_t tick_now; 
    tick_now = *(volatile uint32_t*)0x0200BFF8;
    return MTIMER_TICKS_PER_US * tick_now;
}

void testfunc01(void)
{
    static uint32_t tmp01 = 0;

    printf("testfunc01 20000  run time = %ld us \r\n", (bl_timer_now_us() - tmp01) / 10);
    tmp01 = bl_timer_now_us();
}

void testfunc02(void)
{
    static uint32_t tmp02 = 0;

    printf("testfunc02 15000 run time = %ld us \r\n", (bl_timer_now_us() - tmp02) / 10);
    tmp02 = bl_timer_now_us();
}

void testfunc03(void)
{
    static uint32_t tmp03 = 0;

    printf("testfunc03 1000 run time = %ld us \r\n", (bl_timer_now_us() - tmp03) / 10);
    tmp03 = bl_timer_now_us();
}

void testfunc04(void)
{
    static uint32_t tmp04 = 0;

    printf("testfunc04 3748 run time = %ld us \r\n", (bl_timer_now_us() - tmp04) / 10);
    tmp04 = bl_timer_now_us();
}

void testfunc05(void)
{
    static uint32_t tmp05 = 0;

    printf("testfunc05 2312 run time = %ld us \r\n", (bl_timer_now_us() - tmp05) / 10);
    tmp05 = bl_timer_now_us();
}

void testfunc06(void)
{
    static uint32_t tmp06 = 0;

    printf("testfunc06 3301ms run time = %ld us \r\n", (bl_timer_now_us() - tmp06) / 10);
    tmp06 = bl_timer_now_us();
}

void testfunc07(void)
{
    static uint32_t tmp07 = 0;

    printf("testfunc07 3301ms run time = %ld us \r\n", (bl_timer_now_us() - tmp07) / 10);
    tmp07 = bl_timer_now_us();
}

void testfunc08(void)
{
    static uint32_t tmp08 = 0;

    printf("testfunc08 1234ms run time = %ld us \r\n", (bl_timer_now_us() - tmp08) / 10);
    tmp08 = bl_timer_now_us();
}


void cmd_timer_func(char *buf, int len, int argc, char **argv)
{
    handle1 = hal_hwtimer_create(20000, testfunc01, 1);
    handle2 = hal_hwtimer_create(15000, testfunc02, 1);
    handle3 = hal_hwtimer_create(1000, testfunc03, 1);
    handle4 = hal_hwtimer_create(3748, testfunc04, 1);
    handle5 = hal_hwtimer_create(2312, testfunc05, 1);
    
    return;
}

void cmd_timer_add(char *buf, int len, int argc, char **argv)
{
    hal_hwtimer_create(3301, testfunc06, 0);
    hal_hwtimer_create(3301, testfunc07, 0);
    hal_hwtimer_create(1234, testfunc08, 0); 
    return;
}
   
void cmd_timer_delete(char *buf, int len, int argc, char **argv)
{
    int id;
    int ret = 0;
    id = atoi(argv[1]);

    switch (id) {
        case 1:
        {
            ret = hal_hwtimer_delete(handle1);
        }
        break;
        case 2:
        {
            ret = hal_hwtimer_delete(handle2);
        }
        break;
        case 3:
        {
            ret = hal_hwtimer_delete(handle3);
        }
        break;
        case 4:
        {
            ret =hal_hwtimer_delete(handle4);
        }
        break;
        case 5:
        {
           ret = hal_hwtimer_delete(handle5);
        }
        break;
        default:
        {
            printf("not right id \r\n");
        }
    }
    
    if (ret == 0) {
        printf("delete timer success \r\n");
    } else {
        printf("not find node\r\n", id);
    }
 
    return;
}

// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"test_timer", "test timer", cmd_timer_func},
    {"add_timer", "add timer", cmd_timer_add},
    {"delete_timer", "delete timer by id", cmd_timer_delete},
};                                                                                   

int timer_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
