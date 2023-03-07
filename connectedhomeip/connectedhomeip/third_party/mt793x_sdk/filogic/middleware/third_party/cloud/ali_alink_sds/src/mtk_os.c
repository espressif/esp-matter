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

#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#include "hal_sys.h"
#include "alink_export.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "semphr.h"
#include "fota.h"
#include "fota_config.h"

#include "lwip/sockets.h"
#include "syslog.h"
#include "sds_log.h"
#include "platform.h"

#define ALINK_TASK_PRIO                TASK_PRIORITY_NORMAL
#ifdef ALINK_SDS_PRINTF
#define alink_log  printf
#else
#define alink_log(fmt, args...)     LOG_I(alink_sds, "[alink] "fmt, ##args)
#define alink_loge(fmt, args...)    LOG_E(alink_sds, "[alink] "fmt, ##args)
#endif


typedef QueueHandle_t alink_port_mutex_t;
static alink_port_mutex_t g_alink_mutex = NULL;


log_create_module(sds, PRINT_LEVEL_WARNING);

#define handle_error_en(en, msg) \
        do { printf(msg); printf("%d\n", en); goto do_exit; } while (0)

void platform_printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
#ifdef MTK_DEBUG_LEVEL_NONE
#ifdef USE_SDS_LOG_TASK
        sds_log_print(fmt, args);
#else
        vprintf(fmt, args);
#endif
#else
    //vprint_module_log(&log_control_block_sds, __FUNCTION__, __LINE__, PRINT_LEVEL_INFO, fmt, args);
    vprintf(fmt, args);
#endif
    va_end(args);
}

/************************ memory manage ************************/


void *platform_malloc(uint32_t size)
{
	void *buf = NULL;
	buf =  pvPortCalloc(1, size);

	if(NULL == buf)
		alink_log("alink malloc buff error! size:%d",size);

	return buf;
}

void* platform_calloc_func(size_t size, size_t num)
{
    void *buf = NULL;
    buf = pvPortCalloc(num, size);

    if(NULL == buf)
        alink_loge("alink_calloc_func() Error! size:%d, num:%d", size, num);

    return buf;
}

void platform_free(void *ptr)
{
	if(ptr)
		vPortFree(ptr);
    ptr = NULL;
}



/************************ mutex manage ************************/

void *platform_mutex_init(void)
{
	BaseType_t ret;
    alink_port_mutex_t  alink_mutex = NULL;

	vSemaphoreCreateBinary(alink_mutex);
	//alink_mutex = xSemaphoreCreateMutex();
    alink_log("mutex create: 0x%lx", (uint32_t)alink_mutex);

    ///nvdm_port_log_info("nvdm_port_mutex_creat successfully");
	return (void *)alink_mutex;
}


void platform_mutex_lock(void *mutex)
{
	BaseType_t ret;

	if(mutex == NULL)
		alink_loge("mutex handle error");
    ret = xSemaphoreTake((SemaphoreHandle_t)mutex, portMAX_DELAY);
	if(pdTRUE != ret)
		alink_log("mutex lock error : mutexHandle=0x%lx", (uint32_t)mutex);

}


void platform_mutex_unlock(void *mutex)
{
	BaseType_t ret;

	if(mutex == NULL)
		alink_loge("mutex handle error");
    ret = xSemaphoreGive((SemaphoreHandle_t)mutex);
	if(pdTRUE != ret)
		alink_log("mutex unlock fail : mutexHandle=0x%lx", (uint32_t)mutex);
}


void platform_mutex_destroy(void *mutex)
{
	if(mutex != NULL)
	{
		vSemaphoreDelete((SemaphoreHandle_t)mutex);
	}
}


/************************ semaphore manage ************************/

void *platform_semaphore_init(void)
{
	SemaphoreHandle_t alink_sem = NULL;

	//vSemaphoreCreateBinary(alink_sem);
    alink_sem = xSemaphoreCreateBinary();
    alink_log("Semaphore create: 0x%lx", (uint32_t)alink_sem);

    if(alink_sem == NULL) {

	  alink_loge("creat alink semaphore fail");
    }

	return alink_sem;
}

int platform_semaphore_wait(void *sem, uint32_t timeout_ms)
{
	assert(sem);

	if(timeout_ms == PLATFORM_WAIT_INFINITE){
        timeout_ms = portMAX_DELAY;
    }
    alink_log("semaphore(0x%x) wait: %d", sem, timeout_ms);
    if(pdTRUE == xSemaphoreTake((SemaphoreHandle_t)sem, timeout_ms)) {
        alink_log("semaphore take success: semHandle=0x%lx", (uint32_t)sem);
        return ALINK_OK;
    }
    alink_log("semaphore take timeout: semHandle=0x%lx", (uint32_t)sem);
    return ALINK_ERR;
}


void platform_semaphore_post(void *sem)
{
	assert(sem);

    alink_log("semaphore(0x%x)", sem);
    if(pdTRUE != xSemaphoreGive((SemaphoreHandle_t)sem)) {
        alink_log("semaphore post fail : semHandle=0x%lx", (uint32_t)sem);
        //assert(0);
    }
}


void platform_semaphore_destroy(void *sem)
{
    assert(sem);
    vSemaphoreDelete((SemaphoreHandle_t)sem);
}

/****************************************************************************/


void platform_msleep(uint32_t ms)
{
	 vTaskDelay((TickType_t)ms / portTICK_RATE_MS);
}


uint32_t platform_get_time_ms(void)
{
    struct timeval tv = { 0 };
	uint32_t time_ms;

    gettimeofday(&tv, NULL);

    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return time_ms;
}


uint64_t platform_get_utc_time(uint64_t *p_utc)
{
    return (uint64_t)time((time_t *)p_utc);
}

//hal_rtc_time_t *platform_local_time_r(uint64_t *p_utc,
//        hal_rtc_time_t *p_result)
os_time_struct *platform_local_time_r(const  uint64_t *p_utc, os_time_struct *p_result)
{
	//return (hal_rtc_time_t *)localtime_r((time_t *)p_utc,(struct tm *)p_result);
	alink_loge("Not support");
	return NULL;
}

int platform_thread_get_stack_size(const char *thread_name)
{
    /* Note: not used by linux platform, rtos tune stack size here */
    if (0 == strcmp(thread_name, "wsf_receive_worker"))
    {
        alink_log("get wsf receive worker");
        return 10240;
    }
    else if (0 == strcmp(thread_name, "alcs_thread"))
    {
        alink_log("get alcs thread");
        return 10240;
    }
    else if (0 == strcmp(thread_name, "work queue"))
    {
        alink_log("get work queue thread");
        return 10240;
    }
		else if (0 == strcmp(thread_name, "ifttt_update_system_utc"))
    {
        alink_log("get ifttt_update_system_utc thread");
        return 10240;
    }
	else if (0 == strcmp(thread_name, "asr_websocket_thread"))
    {
        alink_log("get asr_websocket_thread thread");
        return 10240;
    }

    assert(0);
}

int platform_thread_create(
		void **thread,
		const char *name,
		void *(*start_routine) (void *),
		void *arg,
		void *stack,
		uint32_t stack_size,
		int *stack_used)
{
 	BaseType_t ret;
	UBaseType_t prio = ALINK_TASK_PRIO;
    *stack_used = 0;

	ret = xTaskCreate((pdTASK_CODE)start_routine,name,stack_size/sizeof(portSTACK_TYPE),arg,prio,(TaskHandle_t)(thread));
    if(pdTRUE != ret)
		alink_loge("creat thread fail");

	alink_log("threadname=%s, threadId=0x%lx, priority=%ld",name,(uint32_t)(*thread), prio);

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}

void platform_thread_exit(void *thread)
{
	//printf("thread exit threadId=0x%lx",(uint32_t)(*thread));
	alink_log("threadId=0x%lx", (uint32_t)xTaskGetCurrentTaskHandle());
	vTaskDelete(NULL);
}

uint8_t fota_status = 0;

void platform_flash_program_start(void)
{
    //fota_ret_t fota_ret;
    // hal_wdt_status_t wdt_ret;

    alink_log("fota start");
    if (fota_init(&fota_flash_default_config) != FOTA_STATUS_OK) {
        alink_log("[FOTA DL] fota init fail. \n");
        return;
    }
    fota_status = 1;
#if 0
    fota_ret = fota_trigger_update();
    if (FOTA_TRIGGER_SUCCESS == fota_ret) {
        // reboot device
        // hal_wdt_config_t wdt_init;
        // wdt_init.mode = HAL_WDT_MODE_RESET;
        // wdt_init.seconds = 4;

        // wdt_ret = hal_wdt_init(&wdt_init);
        // TODO:
        alink_log("reboot Bye Bye Bye!!!!");
        // wdt rest
        // wdt_ret = hal_wdt_software_reset();

        //reboot api
        hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
    } else {
        alink_loge("fail to trigger update, err = %d", fota_ret);
    }
#endif
}


int platform_flash_program_write_block(char *buffer, uint32_t length)
{
    fota_status_t ret = FOTA_STATUS_OK;
    // init flash layout info

    alink_log("buffer_len:%d", length);

#if 0
    if(!s_isInited)
    {
        if (FOTA_STATUS_OK == fota_init(&fota_flash_default_config))
        {
            s_isInited = 1;
        }
    }
#endif
    ret = fota_write(FOTA_PARITION_TMP, (const uint8_t *)buffer, (uint32_t)length);
    if (FOTA_STATUS_OK == ret) {
        return 0;
    }
    else {
        alink_loge("write flash fail, err = %d", ret);
        return -1;
    }
}


int platform_flash_program_stop(void)
{
    // firmware check will be performed in bootloader, maybe it's not necessary after downloading.
    fota_ret_t fota_ret;
    //hal_wdt_status_t wdt_ret;

    alink_log("firmware check");
    fota_status = 0;
    fota_ret = fota_trigger_update();
    if (FOTA_TRIGGER_SUCCESS == fota_ret) {
        // reboot device
        // hal_wdt_config_t wdt_init;
        // wdt_init.mode = HAL_WDT_MODE_RESET;
        // wdt_init.seconds = 4;

        // wdt_ret = hal_wdt_init(&wdt_init);
        // TODO:
        alink_log("reboot Bye Bye Bye!!!!");
        // wdt rest
        // wdt_ret = hal_wdt_software_reset();

        //reboot api
        //hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
    } else {
        alink_loge("fail to trigger update, err = %d", fota_ret);
        return -1;
    }
    return 0;
}
