/*
 * Copyright (c) 2014-2015 Alibaba Group. All rights reserved.
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

#include "hal_gpio.h"
#include "hal_gpt.h"
#include "hal_eint.h"

#include "alink_export.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "alink_export_rawdata.h"
#include "platform_porting.h"
#include "aws_lib.h"

#include "nvdm.h"
#include "hal_sys.h"
#include "hal_eint.h"
#include "wifi_api.h"
#include "task_def.h"
#include "hal_wdt.h"

#define wsf_deb printf
#define wsf_err printf

xTaskHandle alink_sample_mutex;
static int device_status_change = 1;

//extern void alink_sleep(int);
int get_device_state()
{
    int ret=0;
    alink_pthread_mutex_lock(&alink_sample_mutex);
    ret = device_status_change;
    alink_pthread_mutex_unlock(&alink_sample_mutex);
    return ret;
}

int set_device_state(int state)
{
    alink_pthread_mutex_lock(&alink_sample_mutex);
    device_status_change = state;
    alink_pthread_mutex_unlock(&alink_sample_mutex);
    return state;
}
/*do your job here*/
struct virtual_dev {
	char power;
	char temp_value;
	char light_value;
	char time_delay;
	char work_mode;
} virtual_device = {
0x01, 0x30, 0x50, 0, 0x01};

char *device_attr[5] = { "OnOff_Power", "Color_Temperature", "Light_Brightness",
	"TimeDelay_PowerOff", "WorkMode_MasterLight"
};

const char *main_dev_params =
    "{\"OnOff_Power\": { \"value\": \"%d\" }, \"Color_Temperature\": { \"value\": \"%d\" }, \"Light_Brightness\": { \"value\": \"%d\" }, \"TimeDelay_PowerOff\": { \"value\": \"%d\"}, \"WorkMode_MasterLight\": { \"value\": \"%d\"}}";

#ifndef PASS_THROUGH
#define buffer_size 512
static int alink_device_post_data(alink_down_cmd_ptr down_cmd)
{
    wsf_deb("%s\n", __FUNCTION__);

	alink_up_cmd up_cmd;
	int ret = ALINK_ERR;
	char *buffer = NULL;

    set_device_state(0);
	buffer = (char *)malloc(buffer_size);
	if (buffer == NULL)
		return -1;
	memset(buffer, 0, buffer_size);
	sprintf(buffer, main_dev_params, virtual_device.power,
		virtual_device.temp_value, virtual_device.light_value,
		virtual_device.time_delay, virtual_device.work_mode);
	up_cmd.param = buffer;
	if (down_cmd != NULL) {
		up_cmd.target = down_cmd->account;
		up_cmd.resp_id = down_cmd->id;
	} else {
		up_cmd.target = NULL;
		up_cmd.resp_id = -1;
	}
	ret = alink_post_device_data(&up_cmd);
	if (ret == ALINK_ERR) {
		wsf_err("post failed!\n");
			alink_sleep(1000);
	} else {
		wsf_deb("dev post data success!\n");
		}
	if (buffer)
		free(buffer);
	return ret;
}

/* do your job end */
#endif

int sample_running = ALINK_TRUE;

int main_dev_set_device_status_callback(alink_down_cmd_ptr down_cmd)
{
    int attrLen = 0, valueLen = 0, value = 0, i = 0;
    char *valueStr=NULL, *attrStr=NULL;

    /* do your job here */
    wsf_deb("%s %d \n", __FUNCTION__, __LINE__);
    wsf_deb("%s %d\n%s\n", down_cmd->uuid, down_cmd->method,down_cmd->param);
    set_device_state(1);

    for (i = 0; i < 5; i++) {
        attrStr = alink_JsonGetValueByName(down_cmd->param, strlen(down_cmd->param), device_attr[i], &attrLen, 0);
        valueStr = alink_JsonGetValueByName(attrStr, attrLen, "value", &valueLen, 0);

        if (valueStr && valueLen>0) {
            char lastChar = *(valueStr+valueLen);
            *(valueStr+valueLen) = 0;
            value = atoi(valueStr);
            *(valueStr+valueLen) = lastChar;
            switch (i) {
                case 0:
                    virtual_device.power = value;
                    break;
                case 1:
                    virtual_device.temp_value = value;
                    break;
                case 2:
                    virtual_device.light_value = value;
                    break;
                case 3:
                    virtual_device.time_delay = value;
                    break;
                case 4:
                    virtual_device.work_mode = value;
                    break;
                default:
                	break;
            }
        }
    }

    return 0;
    /* do your job end! */
}

int main_dev_get_device_status_callback(alink_down_cmd_ptr down_cmd)
{
	/* do your job here */
	wsf_deb("%s %d \n", __FUNCTION__, __LINE__);
	wsf_deb("%s %d\n%s\n", down_cmd->uuid, down_cmd->method,down_cmd->param);
	set_device_state(1);

	return 0;
	/*do your job end */
}

int print_mem_callback(void *a, void *b)
{
    wsf_deb("%s\n", __FUNCTION__);

	/*wsf_deb("============ free memory=%u\n", _qcom_mem_heap_get_free_size());   */
	return 1;
}

//extern void alink_sleep(int);

#ifdef PASS_THROUGH
/* this sample save cmd value to virtual_device*/
static int execute_cmd(const char *rawdata, int len)
{
	int ret = 0, i = 0;
	if (len < 1)
		ret = -1;
	for (i = 0; i < len; i++) {
		wsf_deb("%2x ",(unsigned char) rawdata[i]);
		switch (i) {
		case 2:
			if (virtual_device.power != rawdata[i]) {
			virtual_device.power = rawdata[i];
			}
			break;
		case 3:
			if (virtual_device.work_mode != rawdata[i]) {
				virtual_device.work_mode = rawdata[i];
			}
			break;
		case 4:
			if (virtual_device.temp_value != rawdata[i]) {
			virtual_device.temp_value = rawdata[i];
			}
			break;
		case 5:
			if (virtual_device.light_value != rawdata[i]) {
			virtual_device.light_value = rawdata[i];
			}
			break;
		case 6:
			if (virtual_device.time_delay != rawdata[i]) {
			virtual_device.time_delay = rawdata[i];
			}
			break;
		default:
			break;
		}
	}
	return ret;
}

static int get_device_status(char *rawdata, int len)
{
	/* do your job here */
	int ret = 0;
	if (len > 7) {
		rawdata[0] = 0xaa;
		rawdata[1] = 0x07;
		rawdata[2] = virtual_device.power;
		rawdata[3] = virtual_device.work_mode;
		rawdata[4] = virtual_device.temp_value;
		rawdata[5] = virtual_device.light_value;
		rawdata[6] = virtual_device.time_delay;
		rawdata[7] = 0x55;
	} else {
		ret = -1;
	}
	/* do your job end */
	return ret;
}

int alink_device_post_raw_data(void)
{
    wsf_deb("%s\n", __FUNCTION__);

	/* do your job here */
	int len = 8, ret = 0;
	char rawdata[8] = { 0 };

	get_device_status(rawdata, len);
	ret = alink_post_device_rawdata(rawdata, len);
	set_device_state(0);

	if (ret) {
		wsf_err("post failed!\n");
	} else {
		wsf_deb("dev post raw data success!\n");
	}
	/* do your job end */
	return ret;
}


int rawdata_get_callback(const char *in_rawdata,int in_len, char *out_rawdata, int *out_len)
{
	int ret = 0;
	wsf_deb("%s %d \n", __FUNCTION__, __LINE__);
	set_device_state(1);
	/*do your job end! */
	return ret;
}

int rawdata_set_callback(char *rawdata, int len)
{
	/* TODO: */
	/*get cmd from server, do your job here! */
	int ret = 0;
	wsf_deb("%s %d \n", __FUNCTION__, __LINE__);
	ret = execute_cmd(rawdata, len);
	/* do your job end! */
	set_device_state(1);

    // TODO: workaroud. ali task priority is higher than sample.c task, sleep and sample.c task can be run
    alink_sleep(10); // 10ms

	return ret;
}

#endif /*PASS_THROUGH */

int alink_handler_systemstates_callback(void *dev_mac, void *sys_state)
{
    wsf_deb("%s\n", __FUNCTION__);

	char uuid[33] = { 0 };
	char *mac = (char *)dev_mac;
	enum ALINK_STATUS *state = (enum ALINK_STATUS *)sys_state;
	switch (*state) {
	case ALINK_STATUS_INITED:
		break;
	case ALINK_STATUS_REGISTERED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_REGISTERED, mac %s uuid %s \n", mac,
			uuid);
		break;
	case ALINK_STATUS_LOGGED:
		sprintf(uuid, "%s", alink_get_uuid(NULL));
		wsf_deb("ALINK_STATUS_LOGGED, mac %s uuid %s\n", mac, uuid);
        set_device_state(1);
		break;
	default:
		break;
	}
	return 0;
}

void alink_fill_deviceip(char *ip)
{
    strcpy(ip, alink_get_device_ip());

    // wsf_deb("%s : ip %s\n", __FUNCTION__, ip);

#if 0
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
#endif
}

void alink_fill_deviceinfo(struct device_info *deviceinfo)
{
    /*fill main device info here */
	strcpy(deviceinfo->name, DEV_NAME);
	strcpy(deviceinfo->sn, DEV_SN);
	strcpy(deviceinfo->key, ALINK_KEY);
	strcpy(deviceinfo->model, DEV_MODEL);
	strcpy(deviceinfo->secret, ALINK_SECRET);
	strcpy(deviceinfo->type, DEV_TYPE);
	strcpy(deviceinfo->version, DEV_VERSION);
	strcpy(deviceinfo->category, DEV_CATEGORY);
	strcpy(deviceinfo->manufacturer, DEV_MANUFACTURE);
	strcpy(deviceinfo->key_sandbox, ALINK_KEY_SANDBOX);
	strcpy(deviceinfo->secret_sandbox, ALINK_SECRET_SANDBOX);
	// strcpy(deviceinfo->mac, DEV_MAC);
	strcpy(deviceinfo->mac, alink_get_device_mac());
	strcpy(deviceinfo->cid, DEV_CHIPID);
	alink_fill_deviceip(deviceinfo->ip);
	wsf_deb("DEV_MODEL:%s DEV_VERSION:%s\n", DEV_MODEL, DEV_VERSION);
}


int  alink_get_dev_statistics(STATSTYPE type, char *dev_Stats)
{
    wsf_deb("%s\n", __FUNCTION__);

    switch (type) {
        case MEMUSED:
            break;
        case WIFISTRENGTH:
            break;
        default:
            dev_Stats[0] = '\0';
            break;
    }
    return 0;
}


extern int is_alink_network_up;

#ifdef MTK_MINISUPP_ENABLE
extern int g_supplicant_ready;
#endif
extern void aws_test_start(void);
void alink_demo_task( )
{
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

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
        // aws start
        uint8_t clear_flag[2] = "0";
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                                "SmartConfig",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                clear_flag,
                                sizeof(clear_flag));
        wsf_deb("SmartConfig write buff:%s\n", clear_flag);

        aws_test_start();
    }

    // wait netowrk ready
    alink_wait_network_up();

	// int count = 0;
	struct device_info *main_dev;
    main_dev = alink_malloc_func(sizeof(struct device_info));
    alink_pthread_mutex_init(&alink_sample_mutex, NULL);
	memset(main_dev, 0, sizeof(struct device_info));
	alink_fill_deviceinfo(main_dev);
#if 1
	alink_set_loglevel(ALINK_LL_DEBUG | ALINK_LL_INFO | ALINK_LL_WARN |
			   ALINK_LL_ERROR | ALINK_LL_DUMP);
#else
    alink_set_loglevel(ALINK_LL_ERROR);
#endif
	main_dev->sys_callback[ALINK_FUNC_SERVER_STATUS] = alink_handler_systemstates_callback;
	alink_set_callback(ALINK_FUNC_AVAILABLE_MEMORY, print_mem_callback);
	/* ALINK_CONFIG_LEN 2048 */
	alink_register_cb(ALINK_FUNC_READ_CONFIG, (void *)&alink_read_flash);
	alink_register_cb(ALINK_FUNC_WRITE_CONFIG, (void *)&alink_write_flash);
	alink_register_cb(ALINK_FUNC_GET_STATISTICS, alink_get_dev_statistics);
	alink_register_cb(ALINK_FUNC_GET_IP, alink_fill_deviceip);
//#ifdef ALINK_SECURITY // for debugging only
	// alink_enable_sandbox_mode();
//#endif

#ifdef ALINK_OTA
    // alink_register_cb(ALINK_FUNC_OTA_FIRMWARE_SAVE,ota_firmware_save);
	// alink_register_cb(ALINK_FUNC_OTA_UPGRADE, ota_upgrade);
    alink_register_cb(ALINK_FUNC_OTA_FIRMWARE_SAVE, alink_fw_download);
	alink_register_cb(ALINK_FUNC_OTA_UPGRADE, alink_fw_upgrade);
#endif

	/*start alink-sdk */
#ifdef PASS_THROUGH
	alink_start_rawdata(main_dev, rawdata_get_callback,rawdata_set_callback);
#else
	main_dev->dev_callback[ACB_GET_DEVICE_STATUS] = main_dev_get_device_status_callback;
	main_dev->dev_callback[ACB_SET_DEVICE_STATUS] = main_dev_set_device_status_callback;
	alink_start(main_dev);	/*register main device here */
#endif /*PASS_THROUGH */

    alink_free_func(main_dev);

	/*wait main device login, -1 means wait forever */
	alink_wait_connect(NULL, ALINK_WAIT_FOREVER);

    //if(buff[0] == '1') {
        // TODO: workaroud solution, setup connection cost too much time, APK maybe timeout. so sleep some time.
		alink_sleep(1000);
        // aws or softap success, notify app
        wsf_deb("notify app connection success\n");
        aws_notify_app();
    //}

	while (sample_running) {
	    if (get_device_state()) {
            alink_get_free_heap_size();
        #ifdef PASS_THROUGH
    		alink_device_post_raw_data();
        #else
    		alink_device_post_data(NULL);
        #endif /*PASS_THROUGH */
        }
		// alink_sleep(3000);
        alink_sleep(300);
	}
	alink_end();
	alink_pthread_mutex_destroy(&alink_sample_mutex);
}

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

        if(press_period < 1000) {
            // <1000ms reboot and aws start
            printf("Button period:%ud ----> reboot and aws start <----\n", (unsigned int)press_period);
            // write nvram flag
            uint8_t buff[2] = "1";
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA,
                                "SmartConfig",
                                NVDM_DATA_ITEM_TYPE_STRING,
                                buff,
                                sizeof(buff));
            wsf_deb("SmartConfig write buff:%s\n", buff);
            //hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
            hal_wdt_software_reset();
        } else {
            // >=1000ms only factory reset alink
            printf("Button period:%ud ----> factory reset <----\n", (unsigned int)press_period);
            int ret = alink_factory_reset();
            printf("alink_factory_reset ret:%d\n", ret);
        }
    }
}

void register_ali_btn_proc(void)
{
    int32_t irq_num = HAL_EINT_NUMBER_0;
    hal_eint_config_t eint_config;

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

/*************** ali button customize end *****************************/

void alink_demo()
{
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

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


