/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include "alink_export.h"
#include "json_parser.h"
#include "platform.h"

#include "nvdm.h"
#include "hal_sys.h"
#include "hal_eint.h"
#include "wifi_api.h"
#include "task_def.h"
#include "hal_wdt.h"
#include "hal.h"
#include "syslog.h"
#include "sds_log.h"
#include "sockets.h"

log_create_module(alink_sds, PRINT_LEVEL_ERROR);

#ifdef ALINK_SDS_PRINTF
#define wsf_deb printf
#define wsf_err printf
#define alink_log  printf
#else
#define wsf_deb(fmt, args...)       LOG_I(alink_sds, "[wsfd] "fmt, ##args)
#define wsf_err(fmt, args...)       LOG_I(alink_sds, "[wsfe] "fmt, ##args)

#define alink_log(fmt, args...)     LOG_I(alink_sds, "[alink] "fmt, ##args)
#define alink_logw(fmt, args...)    LOG_W(alink_sds, "[alink] "fmt, ##args)
#define alink_loge(fmt, args...)    LOG_E(alink_sds, "[alink] "fmt, ##args)
#endif

/* raw data device means device post byte stream to cloud,
 * cloud translate byte stream to json value by lua script
 * for each product model, vendor need to sumbit a lua script
 * doing this job
 *
 * undefine RAW_DATA_DEVICE, sample will post json string to cloud
 */
//#define RAW_DATA_DEVICE

#define Method_PostData    "postDeviceData"
#define Method_PostRawData "postDeviceRawData"
#define Method_GetAlinkTime "getAlinkTime"
#define NVDM_ALINK_SDS_DEBUG_LEVEL_GROUP           "al_debug_g"
#define NVDM_ALINK_SDS_DEBUG_LEVEL_NAME            "al_debug_n"

#define PostDataFormat      "{\"ErrorCode\":{\"value\":\"%d\"},\"Hue\":{\"value\":\"%d\"},\"Luminance\":{\"value\":\"%d\"},\"Switch\":{\"value\":\"%d\"},\"WorkMode\":{\"value\":\"%d\"}}"
#define post_data_buffer_size    (512)


#ifndef RAW_DATA_DEVICE
static char post_data_buffer[post_data_buffer_size];
#else
static char raw_data_buffer[post_data_buffer_size];

/* rawdata byte order
 *
 * rawdata[0] = 0xaa;
 * rawdata[1] = 0x07;
 * rawdata[2] = device.power;
 * rawdata[3] = device.work_mode;
 * rawdata[4] = device.temp_value;
 * rawdata[5] = device.light_value;
 * rawdata[6] = device.time_delay;
 * rawdata[7] = 0x55;
 */
#define RAW_DATA_SIZE           (8)
uint8_t device_state_raw_data[RAW_DATA_SIZE] = {
    0xaa, 0x07, 1, 8, 10, 50, 10, 0x55
};
#endif

enum {
    ATTR_ERRORCODE_INDEX,
    ATTR_HUE_INDEX,
    ATTR_LUMINANCE_INDEX,
    ATTR_SWITCH_INDEX,
    ATTR_WORKMODE_INDEX,
    ATTR_MAX_NUMS
};
#define DEVICE_ATTRS_NUM   (ATTR_MAX_NUMS)

int device_state[] = {0, 10, 50, 1, 2};/* default value */
char *device_attr[] = {
    "ErrorCode",
    "Hue",
    "Luminance",
    "Switch",
    "WorkMode",
    NULL
};

SemaphoreHandle_t need_report = NULL; /* force to report when device login */
extern uint8_t fota_status;
uint8_t cloud_connected_status = 0;
uint8_t need_update_status = 1;

void cloud_connected(void) 
{ 
    LOG_I(sds, "alink cloud connected!");
    //fix device need to update status when connected after network disconnected 
    if(cloud_connected_status == 1) {
        platform_semaphore_post(need_report);
    }
    cloud_connected_status = 1;
}
void cloud_disconnected(void)
{
    alink_loge("alink cloud disconnected!");
    if(fota_status == 1) {
        platform_sys_reboot();
    }
}

#ifndef RAW_DATA_DEVICE
void cloud_get_device_status(char *json_buffer) { platform_semaphore_post(need_report);}

int cloud_set_device_status(char *json_buffer)
{
    int attr_len = 0, value_len = 0, value = 0, i;
    char *value_str = NULL, *attr_str = NULL;

    //need_report = 1;
    alink_log("enter:T=%d", platform_get_time_ms());
    for (i = 0; device_attr[i]; i++) {
        attr_str = json_get_value_by_name(json_buffer, strlen(json_buffer),
                device_attr[i], &attr_len, NULL);
        value_str = json_get_value_by_name(attr_str, attr_len,
                "value", &value_len, NULL);

        if (value_str && value_len > 0) {
            char last_char = *(value_str+value_len);
            *(value_str + value_len) = 0;
            value = atoi(value_str);
            *(value_str + value_len) = last_char;
            device_state[i] = value;
        }
    }

    platform_semaphore_post(need_report);
    alink_log("exit:T=%d", platform_get_time_ms());
    return 0;
}

#else
static char a2x(char ch)
{
    switch (ch) {
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
        default:
            break;;
    }

    return 0;
}

int uart_send(const uint8_t *raw_data, int raw_data_len)
{
    //TODO: implement uart send here
    int i;
    for (i = 0; i < raw_data_len; i++)
        printf("%02x ", raw_data[i]);

    printf("\n");
    return 0;
}

int raw_data_unserialize(char *json_buffer, uint8_t *raw_data, int *raw_data_len)
{
    int attr_len = 0, i = 0;
    char *attr_str = NULL;

    assert(json_buffer && raw_data && raw_data_len);

    attr_str = json_get_value_by_name(json_buffer, strlen(json_buffer),
            "rawData", &attr_len, NULL);

    if (!attr_str || !attr_len || attr_len > *raw_data_len * 2)
        return -1;

    for (i = 0; i < attr_len; i += 2) {
        raw_data[i / 2] = a2x(attr_str[i]) << 4;
        raw_data[i / 2] += a2x(attr_str[i + 1]);
    }

    raw_data[i / 2] = '\0';
    *raw_data_len = i / 2;

    return 0;
}

int cloud_get_device_raw_data(char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    need_report = 1;

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret)
        return uart_send(raw_data, raw_data_len);
    else
        return -1;
}

int cloud_set_device_raw_data(char *json_buffer)
{
    int ret = 0, raw_data_len = RAW_DATA_SIZE;
    uint8_t raw_data[RAW_DATA_SIZE] = { 0 };

    need_report = 1;

    ret = raw_data_unserialize(json_buffer, raw_data, &raw_data_len);
    if (!ret) {
        /* update device state */
        memcpy(device_state_raw_data, raw_data, raw_data_len);
        return uart_send(raw_data, raw_data_len);
    } else
        return -1;
}

int alink_post_raw_data(uint8_t *byte_stream, int len)
{
        int i, size;
#define RawDataHeader   "{\"rawData\":\""
#define RawDataTail     "\", \"length\":\"%d\"}"

        size = strlen(RawDataHeader);
        strncpy(raw_data_buffer, RawDataHeader, post_data_buffer_size);
        for (i = 0; i < len; i++) {
           size += snprintf(raw_data_buffer + size,
                   post_data_buffer_size - size, "%02X", byte_stream[i]);
        }

        size += snprintf(raw_data_buffer + size,
                post_data_buffer_size - size, RawDataTail, len * 2);

        return alink_report(Method_PostRawData, raw_data_buffer);
}
#endif

static uint32_t work_time = 1; //default work time 1s
extern int is_alink_network_up;

#ifdef MTK_MINISUPP_ENABLE
extern int g_supplicant_ready;
#endif

void main_loop(void)
{
    uint32_t time_start, time_end;
    struct timeval tv = { 0 };

    gettimeofday(&tv, NULL);
    time_start = tv.tv_sec;
    do {
        if (platform_semaphore_wait(need_report, portMAX_DELAY) == ALINK_OK) {
#ifdef RAW_DATA_DEVICE
            /*
             * Note: post data to cloud,
             * use sample alink_post_raw_data()
             * or alink_post()
             */
            /* sample for raw data device */
            alink_post_raw_data(device_state_raw_data, RAW_DATA_SIZE);

#else
            /* sample for json data device */
            snprintf(post_data_buffer, post_data_buffer_size, PostDataFormat,
                    device_state[ATTR_ERRORCODE_INDEX],
                    device_state[ATTR_HUE_INDEX],
                    device_state[ATTR_LUMINANCE_INDEX],
                    device_state[ATTR_SWITCH_INDEX],
                    device_state[ATTR_WORKMODE_INDEX]);
            alink_report(Method_PostData, post_data_buffer);
#endif
        }
        gettimeofday(&tv, NULL);
        time_end = tv.tv_sec;
    } while ((time_start + work_time) > time_end);
}

int alink_get_time(unsigned int *utc_time)
{
#define TIME_STR_LEN    (32)
    char buf[TIME_STR_LEN] = { 0 }, *attr_str;
    int size = TIME_STR_LEN, attr_len = 0;
    int ret;

    ret = alink_query(Method_GetAlinkTime, "{}", buf, &size);
    if (!ret) {
        attr_str = json_get_value_by_name(buf, size, "time", &attr_len, NULL);
        if (attr_str && utc_time) {
            sscanf(attr_str, "%u", utc_time);
        }
    }

    return ret;
}

void helper_api_test(void)
{
    unsigned int time;
    int ret = alink_get_time(&time);
    //assert(!ret);

    alink_log("get alink utc time: %d", time);
}

void awss_demo(void)
{

    char ssid[PLATFORM_MAX_SSID_LEN] = { 0 };
    char passwd[PLATFORM_MAX_PASSWD_LEN] = { 0 };
#define WLAN_CONNECTION_TIMEOUT     (30 * 1000) //30 seconds

    /* after system booting, read ssid & passwd from flash */
    //vendor_read_ssid_passwd_from_flash(ssid, passwd);
    uint8_t buff[2] = "0";
    uint32_t buf_size = sizeof(buff);
    nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA,
                        "SmartConfig",
                        buff,
                        &buf_size);
    wsf_deb("SmartConfig read buff[size:%ud]:%s\n", (unsigned int)buf_size, buff);

#ifdef MTK_MINISUPP_ENABLE
    while(!g_supplicant_ready){
        vTaskDelay(50);
    }
#endif

    if(buff[0] == '1'){
        int ret = -2;
        uint8_t clear_flag[2] = "0";
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                                "SmartConfig",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                clear_flag,
                                sizeof(clear_flag));
        wsf_deb("SmartConfig write buff:%s\n", clear_flag);
        need_update_status = 0;

        ret = awss_start(); /* Note: awss_start() will block until success */
        alink_log("====>awss done:ret=%d<====", ret);
        msc_save_info();
    }

    /* call alink_start() after system got IP address */

    /* activate device after alink connnected, see activate_button_pressed */
}

/* activate sample */
char active_data_tx_buffer[128];
#define ActivateDataFormat    "{\"ErrorCode\": { \"value\": \"%d\" }}"
int activate_button_pressed(void)
{
    if(need_report == NULL) {
        alink_log("need_report = NULL");
        return 0;
    }
    sprintf(active_data_tx_buffer, ActivateDataFormat, 1);
    alink_log("send:%s", active_data_tx_buffer);
    alink_report(Method_PostData, (char *)active_data_tx_buffer);

    sprintf(active_data_tx_buffer, ActivateDataFormat, 0);
    alink_log("send:%s", active_data_tx_buffer);
    alink_report(Method_PostData, (char *)active_data_tx_buffer);
    platform_semaphore_post(need_report);

    return 0;
}


enum SERVER_ENV {
    DAILY = 0,
    SANDBOX,
    ONLINE,
    DEFAULT
};
const char *env_str[] = {"daily", "sandbox", "online", "default"};

void usage(void)
{
    printf("\nalink_sample -e enviroment -t work_time -l log_level\n");
    printf("\t -e alink server environment, 'daily', 'sandbox' or 'online'(default)\n");
    printf("\t -t work time, unit is s\n");
    printf("\t -l log level, trace/debug/info/warn/error/fatal/none\n");
    printf("\t -h show help text\n");
}

static int env = DEFAULT;
static char log_level = ALINK_LL_INFO;
extern char *optarg;

void parse_opt(int argc, char *argv[])
{
    char ch;

    while ((ch = getopt(argc, argv, "e:t:l:h")) != -1) {
        switch (ch) {
        case 'e':
            if (!strcmp(optarg, "daily"))
                env = DAILY;
            else if (!strcmp(optarg, "sandbox"))
                env = SANDBOX;
            else if (!strcmp(optarg, "online"))
                env = ONLINE;
            else {
                env = ONLINE;
                printf("unknow opt %s, use default env\n", optarg);
            }
            break;
        case 't':
            work_time = atoi(optarg);
            break;
        case 'l':
            if (!strcmp(optarg, "trace"))
                log_level = ALINK_LL_TRACE;
            else if (!strcmp(optarg, "debug"))
                log_level = ALINK_LL_DEBUG;
            else if (!strcmp(optarg, "info"))
                log_level = ALINK_LL_INFO;
            else if (!strcmp(optarg, "warn"))
                log_level = ALINK_LL_WARN;
            else if (!strcmp(optarg, "error"))
                log_level = ALINK_LL_ERROR;
            else if (!strcmp(optarg, "fatal"))
                log_level = ALINK_LL_FATAL;
            else if (!strcmp(optarg, "none"))
                log_level = ALINK_LL_NONE;
            else
                log_level = ALINK_LL_INFO;
            break;
        case 'h':
        default:
            usage();
            exit(0);
        }
    }

    printf("alink server: %s, work_time: %ds, log level: %d\n",
            env_str[env], work_time, log_level);
}


int alink_demo_task()
{
    //parse_opt(argc, argv);
    uint8_t link_status = 0;
    int ret;
    uint8_t buff[2] = "0";
    uint32_t buf_size = sizeof(buff);

    nvdm_read_data_item(NVDM_ALINK_SDS_DEBUG_LEVEL_GROUP,
                        NVDM_ALINK_SDS_DEBUG_LEVEL_NAME,
                        buff,
                        &buf_size);
    if(buff[0] != '0') {
        log_level = (buff[0] - '0') - 1;
    }
    alink_set_loglevel(log_level);

    awss_demo();

    if (env == SANDBOX)
        alink_enable_sandbox_mode();
    else if (env == DAILY)
        alink_enable_daily_mode(NULL, 0);

    alink_register_callback(ALINK_CLOUD_CONNECTED, &cloud_connected);
    alink_register_callback(ALINK_CLOUD_DISCONNECTED, &cloud_disconnected);
    /*
     * NOTE: register ALINK_GET/SET_DEVICE_STATUS or ALINK_GET/SET_DEVICE_RAWDATA
     */
#ifdef RAW_DATA_DEVICE
    /*
     * TODO: before using callback ALINK_GET/SET_DEVICE_RAWDATA,
     * submit product_model_xxx.lua script to ALINK cloud.
     * ALINKTEST_LIVING_LIGHT_SMARTLED_LUA is done with it.
     */
    alink_register_callback(ALINK_GET_DEVICE_RAWDATA, &cloud_get_device_raw_data);
    alink_register_callback(ALINK_SET_DEVICE_RAWDATA, &cloud_set_device_raw_data);
#else
    alink_register_callback(ALINK_GET_DEVICE_STATUS, &cloud_get_device_status);
    alink_register_callback(ALINK_SET_DEVICE_STATUS, &cloud_set_device_status);
#endif

    ret = alink_start();
    ret = alink_wait_connect(ALINK_WAIT_FOREVER);

    need_report = platform_semaphore_init();
    //fix device need to update status when connected after reboot don't need activated
    if(need_update_status == 1) {
        platform_semaphore_post(need_report);
    }
    helper_api_test();

    while(1) {
        main_loop();
    }

    //ÔÝÆÁ±Î¸´Î»´úÂë£¬±ÜÃâ²âÊÔÖÐÎóÖØÆô
    //alink_factory_reset();

    alink_end();

    return 0;
}


/*************** ali button customize end *****************************/

/*************** ali button customize  start *****************************/
QueueSetHandle_t ali_queue = NULL;

static void eint_irq_handle(void *user_data)
{
    static uint32_t btn_down_count = 0;
    static uint32_t btn_up_count = 0;
    static uint32_t press_step = 0;
#if (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
#ifdef HAL_EINT_FEATURE_MASK
    /* Mask EINT first to prevent the interrupt misfiring */
    hal_eint_mask(HAL_EINT_NUMBER_0);
#endif
#endif
    press_step++;
    if(press_step%2 != 0) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &btn_down_count);
    }
    else {
        portBASE_TYPE time_monitor_wake = pdFALSE;
        uint32_t press_period;

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &btn_up_count);
        if(btn_up_count > btn_down_count) {
            press_period = (uint32_t)(((uint64_t)(btn_up_count - btn_down_count)*1000) / 32768);
        }
        else {
            press_period = (uint32_t)((((uint64_t)(btn_up_count + (0xFFFFFFFF - btn_down_count))) * 1000) / 32768);
        }

#ifdef MTK_MINISUPP_ENABLE
        if(!g_supplicant_ready)
            return;
#endif
        xQueueSendToBackFromISR(ali_queue, &press_period, &time_monitor_wake);
    }
#if (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
#ifdef HAL_EINT_FEATURE_MASK
    /* Unmask EINT  */
    hal_eint_unmask(HAL_EINT_NUMBER_0);
#endif
#endif

}

static void press_time_monitor(void *args)
{
    for(;;) {
        uint32_t press_period;
        xQueueReceive(ali_queue, (void *)&press_period, portMAX_DELAY);
        if(press_period < 5000) {
            // <1000ms reboot and aws start
            alink_log("Button period:%ud ----> reboot and aws start <----", (unsigned int)press_period);
            // write nvram flag
            uint8_t buff[2] = "1";
            int32_t ret = 0;
            ret = nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                                "SmartConfig",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                buff,
                                sizeof(buff));
            wsf_deb("SmartConfig write buff:%s, ret=%d\n", buff, ret);
            hal_wdt_software_reset();
        } else {
            // >=5000ms only factory reset alink
            alink_log("Button period:%ud ----> factory reset <----", (unsigned int)press_period);
            int ret = alink_factory_reset();
            alink_log("alink_factory_reset ret:%d", ret);
        }
    }
}

void register_ali_btn_proc(void)
{
    int32_t irq_num = HAL_EINT_NUMBER_0;
    hal_eint_config_t eint_config;

    alink_log("register aws button");

#if (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
    hal_gpio_init(HAL_GPIO_0);

    /* Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_EINT0);

    /* Set direction as input and disable pull of corresponding GPIO */
    hal_gpio_set_direction(HAL_GPIO_0, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_disable_pull(HAL_GPIO_0);

#ifdef HAL_EINT_FEATURE_MASK
    /* Mask EINT first to prevent the interrupt misfiring */
    hal_eint_mask(irq_num);
#endif
#endif

    eint_config.debounce_time = 5;
    eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;

    hal_eint_init(irq_num, &eint_config);
    hal_eint_register_callback(irq_num, eint_irq_handle, NULL);

#if (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
#ifdef HAL_EINT_FEATURE_MASK
    /* Unmask EINT  */
    hal_eint_unmask(irq_num);
#endif
#endif

    ali_queue = xQueueCreate(ALINK_BUTTON_MONITOR_QUEUE_LENGTH, sizeof(uint32_t));
    if(ali_queue == NULL)
        return;

    if(xTaskCreate(
            press_time_monitor,
            ALINK_BUTTON_MONITOR_TASK_NAME,
            ALINK_BUTTON_MONITOR_TASK_STACKSIZE/sizeof(portSTACK_TYPE),
            NULL,
            ALINK_BUTTON_MONITOR_TASK_PRIO,
            NULL) != pdPASS){
        vQueueDelete(ali_queue);
        ali_queue = NULL;
        return;
    }
}

void alink_demo()
{
    LOG_I(sds, "heap free size:%d", xPortGetFreeHeapSize());
#ifdef USE_SDS_LOG_TASK
    sds_log_init();
#endif
/*
    uint8_t buff[2] = "0";
    uint32_t buf_size = sizeof(buff);
    nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA,
                        "SmartConfig",
                        buff,
                        &buf_size);
    wsf_deb("SmartConfig read buff[size:%d]:%s\n", buf_size, buff);
    if(buff[0] == '1') {
        // wifi_config_disable_scan();
    }
 */
    xTaskCreate(alink_demo_task, ALINK_TASK_NAME, ALINK_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, ALINK_TASK_PRIO, NULL);

    /* ali button customize */
    register_ali_btn_proc();
}

int reset_awss_config(void)
{
    uint8_t buff[2] = "1";
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                        "SmartConfig",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        buff,
                        sizeof(buff));
    wsf_deb("SmartConfig write buff:%s\n", buff);
    hal_wdt_software_reset();

    return 0;
}



uint8_t alink_cli_handler(uint8_t len, char *param[])
{

     if (len > 0) {
        uint32_t num = 0;

        num = atoi(param[0]);
        switch(num) {
            case 1: {
                activate_button_pressed();
                break;
            }
            case 2: {
                alink_log("reset config");
                reset_awss_config();
                break;
            }
            case 3: {
                uint32_t log_temp = 0;

                if(param[1] != NULL) {
                    log_temp = atoi(param[1]);
                }

                if((log_temp > ALINK_LL_NONE) && (log_temp < (ALINK_LL_TRACE + 2))) {
                    uint8_t buff[2] = "1";
                    buff[0] = log_temp + '0';

                    nvdm_write_data_item(NVDM_ALINK_SDS_DEBUG_LEVEL_GROUP,
                        NVDM_ALINK_SDS_DEBUG_LEVEL_NAME,
                        NVDM_DATA_ITEM_TYPE_STRING,
                        buff,
                        sizeof(buff));
                }
                printf("cli:log level %d\n", log_temp);
                break;
            }
            case 4: {
                  printf("Button period:%ud ----> factory reset <----");
                    uint8_t buff[2] = "1";
                    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                        "SmartConfig",
                        NVDM_DATA_ITEM_TYPE_STRING,
                        buff,
                        sizeof(buff));
                  int ret = alink_factory_reset();
                  printf("alink_factory_reset ret:%d", ret);

                break;
            }          
            default:
                break;
        }

    } else {
         activate_button_pressed();
    }
    return 0;
}
