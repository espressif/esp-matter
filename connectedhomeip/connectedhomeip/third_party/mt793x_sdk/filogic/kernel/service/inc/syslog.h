/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/** @file syslog.h
*
*  debug log interface file
*
*/

#ifndef __SYSLOG_H__
#define __SYSLOG_H__


#include <stdarg.h>
#include <stdint.h>
#include "hal_uart.h"


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/****************************************************************************
 *
 * Control Output Format
 *
 ****************************************************************************/
#define LOG_TIMESTAMP_OFFSET                   (0)
#define LOG_MODULE_OFFSET                      (1)
#define LOG_LEVEL_OFFSET                       (2)
#define LOG_FUNCTION_OFFSET                    (3)
#define LOG_LINE_OFFSET                        (4)
#define LOG_STM_OFFSET                         (5)
#define LOG_ID_OFFSET                          (6)
#define LOG_SNTP_OFFSET                        (7)

#define check_bit(num, offset)                 (num & (1 << offset))
/* Ren */
//T: time, M: module, C: level, F: function, L: line, S: stm, I: module id

#define LOG_CONTROL_BLOCK_SET_FORMAT(_module, format) \
    extern log_control_block_t log_control_block_##_module;\
    log_control_block_##_module.module_format = format;
/* Ren */
//T: time, M: module, C: level, F: function, L: line, S: stm, I: module id, N: SNTP



/* Ren */
//#define CRASH_SYSLOG_BASE           (0x18C38000-0x18000000)
//#define CRASH_SYSLOG_LENGTH         (0x10000)
#define NORMAL_SYSLOG_BASE          (LOG_BASE + 0x1000)
#define NORMAL_SYSLOG_LENGTH        (0x8000)

/****************************************************************************
 *
 * Config switch
 *
 ****************************************************************************/

#define DAYTIME_BUF_SIZE 19

/**
 * @brief print switch definition
 */
typedef enum {
    DEBUG_LOG_ON,
    DEBUG_LOG_OFF
} log_switch_t;


/**
 * @brief print level definition
 */
typedef enum {
    PRINT_LEVEL_DEBUG,
    PRINT_LEVEL_INFO,
    PRINT_LEVEL_WARNING,
    PRINT_LEVEL_ERROR
} print_level_t;


typedef void (*f_print_t)(void *handle, const char *func, int line, print_level_t level, const char *message, ...);
typedef void (*f_dump_buffer_t)(void *handle, const char *func, int line, print_level_t level, const void *data, int length, const char *message, ...);

typedef struct {
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    uint16_t msec;
} rtc_time_t;

/**
 * @brief log context definition
 */
typedef struct {
    const char *module_name;
    unsigned int module_id;
    uint8_t         module_format;
    log_switch_t    log_switch;
    print_level_t   print_level;
    f_print_t       print_handle;
    f_dump_buffer_t dump_handle;
} log_control_block_t;

typedef struct syslog_config_s syslog_config_t;

/**
 * syslog save config callback function prototype.
 *
 * Declare a function of this prototype and register it in
 * log_init so that it can be used when saving configuration.
 *
 * This decouples syslog from direct NVRAM module dependency.
 */
typedef void (*syslog_save_fn)(const syslog_config_t *config);

/**
 * syslog load config callback function prototype.
 *
 * Declare a function of this prototype and register it in
 * log_init so that it can be used when loading configuration.
 *
 * This decouples syslog from direct NVRAM module dependency.
 */
typedef uint32_t (*syslog_load_fn)(syslog_config_t *config);

/**
 * @brief syslog configuration
 */
struct syslog_config_s {
    syslog_save_fn          save_fn;
    log_control_block_t     **filters;
};

/**
 * Set filter of syslog modules by cli.
 */
int syslog_set_filter(char *module_name, char *log_switch_str, char *print_level_str, int save);

/**
 * Set filter of syslog modules by at command.
 */
int syslog_at_set_filter(char *module_name, int log_switch, int print_level, int save);

/**
 * Get a copy of config.
 */
void syslog_get_config(syslog_config_t *config);

/**
 * Convert the filter specifications from string to its internal notation.
 *
 * @param filters the array of filters.
 * @param buff    the filter specifications.
 */
int syslog_convert_filter_str2val(log_control_block_t **filters, char *buff);
int syslog_convert_filter_val2str(const log_control_block_t **filters, char *buff);

/**
 * Convert the the internal representation to readable format.
 */
const char *log_switch_to_str(log_switch_t log_switch);
const char *print_level_to_str(print_level_t print_level);

/**
 * Convert the readable format to internal representation.
 */
int str_to_log_switch(const char *log_switch_str);
int str_to_print_level(const char *print_level_str);

void print_module_log(void *handle, const char *func, int line, print_level_t level, const char *message, ...);
void vprint_module_log(void *handle, const char *func, int line, print_level_t level, const char *message, va_list list);

void dump_module_buffer(void *handle, const char *func, int line, print_level_t level, const void *data, int length, const char *message, ...);
void vdump_module_buffer(void *handle, const char *func, int line, print_level_t level, const void *data, int length, const char *message, va_list list);

int log_write(char *buf, int len);
int log_write_binary(char *buf, int len);
char *get_current_daytime(void);

#ifdef HAL_RTC_MODULE_ENABLED
/**
* Ren: syslog set SNTP time callback
*/
void log_set_sntp_time(hal_rtc_time_t *sntp_rt);
#endif /* #ifdef HAL_RTC_MODULE_ENABLED */

hal_uart_status_t log_uart_init(hal_uart_port_t port);
#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
int32_t log_dump_buffer_alloc(int32_t buffer_size);
#endif /* #if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) */
void log_putchar(char byte);

#if defined (MTK_DEBUG_LEVEL_NONE)

#define log_init(save, load, entries) do{}while(0)
#define log_send_dma(buf, len) do{}while(0)
#define log_receive_dma(buf, len) do{}while(0)
#define syslog_enter_deep_sleep(data) do{}while(0)
#define syslog_exit_deep_sleep(data) do{}while(0)
#define syslog_deep_sleep_switch(is_enable) do{}while(0)

/**
 * @brief create log context for a module
 */
#define log_create_module(_module, _level) \
log_control_block_t log_control_block_##_module = \
{ \
    #_module, \
    0, \
    0x1f, \
    (DEBUG_LOG_ON), \
    (_level), \
    print_module_log, \
    dump_module_buffer \
}

#else /* #if defined (MTK_DEBUG_LEVEL_NONE) */

/**
 * @brief log init
 */
void log_init(syslog_save_fn save, syslog_load_fn load, log_control_block_t  **entries);

/**
 * @brief log mode switching while deep sleep
 *        call by sleep manager only.
 */
void syslog_enter_deep_sleep(void *data);
void syslog_exit_deep_sleep(void *data);
void syslog_deep_sleep_switch(uint32_t is_enable);

/**
 * @brief log_send_dma (Deprecated)
 */
bool log_send_dma(char *buf, int len);

/**
 * @brief log_receive_dma (Deprecated)
 */
int log_receive_dma(char *buf, int len);

/**
 * @brief create log context for a module
 */
#define log_create_module(_module, _level) \
log_control_block_t log_control_block_##_module = \
{ \
    #_module, \
    0, \
    0x1f, \
    (DEBUG_LOG_ON), \
    (_level), \
    print_module_log, \
    dump_module_buffer \
}

#endif /* #if defined (MTK_DEBUG_LEVEL_NONE) */

#if defined(MTK_DEBUG_LEVEL_DEBUG)
/**
 * @brief print debug level log message
 */
#define LOG_D(_module, _message, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_handle(&log_control_block_##_module, \
                                             __FUNCTION__, \
                                             __LINE__, \
                                             PRINT_LEVEL_DEBUG, \
                                             (_message), \
                                             ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_DEBUG) */

#define LOG_D(_module, _message, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_DEBUG) */

#if defined(MTK_DEBUG_LEVEL_INFO)
/**
 * @brief print info level log message
 */
#define LOG_I(_module, _message, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_handle(&log_control_block_##_module, \
                                             __FUNCTION__, \
                                             __LINE__, \
                                             PRINT_LEVEL_INFO, \
                                             (_message), \
                                             ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_INFO) */

#define LOG_I(_module, _message, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_INFO) */

#if defined(MTK_DEBUG_LEVEL_WARNING)
/**
 * @brief print warning level log message
 */
#define LOG_W(_module, _message,...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_handle(&log_control_block_##_module, \
                                             __FUNCTION__, \
                                             __LINE__, \
                                             PRINT_LEVEL_WARNING, \
                                             (_message), \
                                             ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_WARNING) */

#define LOG_W(_module, _message, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_WARNING) */

#if  defined(MTK_DEBUG_LEVEL_ERROR)
/**
 * @brief print error level log message
 */
#define LOG_E(_module, _message,...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_handle(&log_control_block_##_module, \
                                             __FUNCTION__, \
                                             __LINE__, \
                                             PRINT_LEVEL_ERROR, \
                                             (_message), \
                                             ##__VA_ARGS__); \
} while (0)

#else /* #if  defined(MTK_DEBUG_LEVEL_ERROR) */

#define LOG_E(_module, _message, ...) do{}while(0)

#endif /* #if  defined(MTK_DEBUG_LEVEL_ERROR) */

#if defined(MTK_DEBUG_LEVEL_DEBUG)

/**
 * @brief dump buffer in hex format
 */
#define LOG_HEXDUMP_D(_module, _message, _data, _len, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.dump_handle(&log_control_block_##_module, \
                                            __FUNCTION__, \
                                            __LINE__, \
                                            PRINT_LEVEL_DEBUG, \
                                            (_data), \
                                            (_len), \
                                            (_message), \
                                            ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_DEBUG) */

#define LOG_HEXDUMP_D(_module, _message, _data, _len, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_DEBUG) */

#if defined(MTK_DEBUG_LEVEL_INFO)

/**
 * @brief dump buffer in hex format
 */
#define LOG_HEXDUMP_I(_module, _message, _data, _len, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.dump_handle(&log_control_block_##_module, \
                                            __FUNCTION__, \
                                            __LINE__, \
                                            PRINT_LEVEL_INFO, \
                                            (_data), \
                                            (_len), \
                                            (_message), \
                                            ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_INFO) */

#define LOG_HEXDUMP_I(_module, _message, _data, _len, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_INFO) */

#if defined(MTK_DEBUG_LEVEL_WARNING)
/**
* @brief dump buffer in hex format
*/
#define LOG_HEXDUMP_W(_module, _message, _data, _len, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.dump_handle(&log_control_block_##_module, \
                                            __FUNCTION__, \
                                            __LINE__, \
                                            PRINT_LEVEL_WARNING, \
                                            (_data), \
                                            (_len), \
                                            (_message), \
                                            ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_WARNING) */

#define LOG_HEXDUMP_W(_module, _message, _data, _len, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_WARNING) */

#if defined(MTK_DEBUG_LEVEL_ERROR)
/**
* @brief dump buffer in hex format
*/
#define LOG_HEXDUMP_E(_module, _message, _data, _len, ...) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.dump_handle(&log_control_block_##_module, \
                                            __FUNCTION__, \
                                            __LINE__, \
                                            PRINT_LEVEL_ERROR, \
                                            (_data), \
                                            (_len), \
                                            (_message), \
                                            ##__VA_ARGS__); \
} while (0)

#else /* #if defined(MTK_DEBUG_LEVEL_ERROR) */

#define LOG_HEXDUMP_E(_module, _message, _data, _len, ...) do{}while(0)

#endif /* #if defined(MTK_DEBUG_LEVEL_ERROR) */


#if defined (MTK_DEBUG_LEVEL_NONE)

#define log_config_print_switch(_module, _log_switch) do{}while(0)
#define log_config_print_level(_module, _level) do{}while(0)
#define log_config_print_func(_module, _print_func) do{}while(0)
#define log_config_dump_func(_module, _dump_func) do{}while(0)
#define LOG_CONTROL_BLOCK_DECLARE(_module)
#define LOG_CONTROL_BLOCK_SYMBOL(_module)

#else /* #if defined (MTK_DEBUG_LEVEL_NONE) */

/**
 * @brief turn on or off the print switch
 */
#define log_config_print_switch(_module, _log_switch) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.log_switch = (_log_switch); \
} while (0)


/**
 * @brief config the print level
 */
#define log_config_print_level(_module, _level) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_level = (print_level_t)(_level); \
} while (0)


/**
 * @brief customize the print func
 * @note the log message is printed to UART0 with a fixed format by
 *       default. it can be changed by providing a different print
 *       function with this API, if different behaviour is required
 */
#define log_config_print_func(_module, _print_func) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.print_handle = (_print_func); \
} while (0)

/**
 * @brief customize the dump func
 * @note the dump message is printed to UART0 with a fixed format by
 *       default. it can be changed by providing a different print
 *       function with this API, if different behaviour is required
 */
#define log_config_dump_func(_module, _dump_func) \
do { \
    extern log_control_block_t log_control_block_##_module; \
    log_control_block_##_module.dump_handle = (_dump_func); \
} while (0)

/* maximum length of syslog configuration */
#define SYSLOG_FILTER_LEN (1024)

/**
 * @brief macro to declare the log_control_block of a module
 */
#define LOG_CONTROL_BLOCK_DECLARE(_module) extern log_control_block_t log_control_block_##_module


/**
 * @brief macro to get the log_control_block of a module
 */
#define LOG_CONTROL_BLOCK_SYMBOL(_module)  log_control_block_##_module

#endif /* #if defined (MTK_DEBUG_LEVEL_NONE) */

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef __SYSLOG_H__ */

