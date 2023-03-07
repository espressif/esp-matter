#include <stdio.h>
#include <easyflash.h>
#include <string.h>
#include <bl_sys_time.h>
#include <bl_timer.h>

extern uint32_t bl_timer_now_us();
extern int bl_timer_tick_enable(void);

static void flash_test_write(const char *key, const char *value)
{
    ef_set_env(key, value);
    ef_save_env();

    return;
}

void flash_test_cmd(char *buf, int len, int argc, char **argv)
{
    char value_r[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    char key_w[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    char value_w[EF_STR_ENV_VALUE_MAX_SIZE + 1];
    int  i_count = 0;
    char *str_value_r = value_r;

    for (i_count = 0; i_count < 100; i_count++) {
        sprintf(key_w, "key_%d", i_count);
        sprintf(value_w, "value_%d", i_count);

        ef_set_env(key_w, value_w);
        ef_save_env();
        str_value_r = ef_get_env(key_w);
        if ( strcmp(str_value_r, (char *)value_w) == 0) {
            if (i_count == 0) {
                printf("test flash ok. \r\n");
            }
        } else {
            printf("times: %d ef_get_env value %s. \r\n", i_count, str_value_r);
        }
    }
}

void flash_test_speed_cmd(char *buf, int len, int argc, char **argv)
{
    uint32_t i_time_start = 0; 
    uint32_t i_time_end = 0;                     // 读写前后的时间获取
    int i_count = 0;                             // 用于读写次数计数
    float speed = 0.0;
    char value_r[EF_STR_ENV_VALUE_MAX_SIZE + 1]; // 保留读到的数据用于对比
    char key_w[EF_STR_ENV_VALUE_MAX_SIZE + 1];   // 保存key的值
    char value_w[EF_STR_ENV_VALUE_MAX_SIZE + 1]; // 要写的value
    char *str_value_r = value_r;
    
    if (argc > 2){
        printf("argc is %d\r\n", argc);
        return;
    }
       
    i_time_start = bl_timer_now_us();
    
    for ( i_count = 0; i_count < 100; i_count++) {
        sprintf(key_w, "key_%d", i_count);
        sprintf(value_w, "value_%d", i_count);
        ef_set_env(key_w, value_w);
        ef_save_env();
    }
    
    i_time_end = bl_timer_now_us();
    
    speed = (float)(100.0 * 1000.0 * (strlen(key_w) + strlen(value_w)) / (i_time_end - i_time_start ));
    printf("write speed is %f byte/s, datelen:%d .\r\n", speed, (strlen(key_w) + strlen(value_w)));
    
    i_time_start = bl_timer_now_us();
    
    for ( i_count = 0; i_count < 100; i_count++) {
        str_value_r = ef_get_env(key_w);
    }
    
    i_time_end = bl_timer_now_us();
    
    speed = (float)(100.0 * 1000.0 * (strlen(key_w) + strlen(value_r)) / (i_time_end - i_time_start ));
    printf("read speed  is %f byte/s. \r\n", speed);
    
    return;
}


