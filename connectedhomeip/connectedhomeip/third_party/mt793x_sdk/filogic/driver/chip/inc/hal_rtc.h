/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __HAL_RTC_H__
#define __HAL_RTC_H__
#include "hal_platform.h"

#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 * This section introduces the Real-Time Clock (RTC) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_RTC_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b RTC                        | Real-Time Clock. A real-time clock (RTC) is a computer clock, mostly in the form of an integrated circuit, that keeps track of the current time. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Real-time_clock"> an introduction to the RTC in Wikipedia </a>.|
 *
 * @section HAL_RTC_Features_Chapter Supported features
 *
 * @}
 * @}
 */
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - \b Support \b date \b and \b time \b information. \n
*   Call #hal_rtc_set_time() to set the RTC current time and #hal_rtc_get_time() to retrieve the RTC current time.
* @}
* @}
*/
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - \b Support \b alarm \b notification. \n
*   Call #hal_rtc_set_alarm() to set the RTC alarm time and #hal_rtc_get_alarm() to get the RTC alarm time.
*   To receive alarm notification, please call #hal_rtc_set_alarm_callback() to register a callback function and call
*   #hal_rtc_enable_alarm() to enable the RTC alarm notification. When the RTC current time reaches the RTC alarm
*   time, the callback function you registered will be executed. The alarm notification can be disabled by using #hal_rtc_disable_alarm()
*   function, if it's no longer required.
* - \b Support \b store \b data \b during \b power \b off. \n
*   There are several spare registers in the RTC to store data that are not cleared during a power off, except when the battery is removed.
*
* @section HAL_RTC_Architechture_Chapter Software architecture of RTC
*
* @}
* @}
*/
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* The software architecture of the RTC driver is shown in the diagram below.
* - The user needs to call #hal_rtc_init() to initialize the RTC hardware before using the RTC service. The user can set and get the current
*   time or RTC alarm time at any time. RTC just supports the alarm notification. The user can use #hal_rtc_set_alarm_callback(), to register
*   callbacks for the RTC notification.
* @image html hal_rtc_archi.png
* @}
* @}
*/
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
*
* @section HAL_RTC_Driver_Usage_Chapter How to use this driver
*
* - Set the RTC current time. \n
*  - Step 1. Call #hal_rtc_init() to initialize the RTC hardware, if it hasn't been called after the power went on.
*  - Step 2. Call #hal_rtc_set_time() to set the RTC current time.
*  - sample code:
* Note: the base year of RTC should be 2000, there will be leap year problem if a value other than 2000 is used.
*    @code
*       #define BASE_YEAR 2000
*       set_current_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
*           hal_rtc_time_t time;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           time.rtc_year = year - BASE_YEAR;
*           time.rtc_mon = mon;
*           time.rtc_day = day;
*           time.rtc_hour = hour;
*           time.rtc_min = min;
*           time.rtc_sec = sec;
*
*           // Set the RTC current time.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_time(&time)) {
*               // error handle
*           }
*       }
*
*    @endcode
*
* - Get the RTC current time. \n
*  - Step 1. Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2. Call #hal_rtc_get_time() to get RTC current time.
*  - sample code:
*    @code
*       #define BASE_YEAR 2000
*       get_current_time(uint16_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec) {
*           hal_rtc_time_t time;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_get_time(&time)) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           *year = time.rtc_year + BASE_YEAR;
*           *mon = time.rtc_mon;
*           *day = time.rtc_day;
*           *hour = time.rtc_hour;
*           *min = time.rtc_min;
*           *sec = time.rtc_sec;
*       }
*
*    @endcode
*
* @}
* @}
*/
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - Set the RTC alarm time. \n
*  - Step 1. Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2. Call #hal_rtc_set_time() to set the RTC alarm time.
*  - sample code:
*    @code
*       #define BASE_YEAR 2000
*       set_alarm_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
*           hal_rtc_time_t alarm;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           alarm.rtc_year = year - BASE_YEAR;
*           alarm.rtc_mon = mon;
*           alarm.rtc_day = day;
*           alarm.rtc_hour = hour;
*           alarm.rtc_min = min;
*           alarm.rtc_sec = sec;
*
*           // Set the RTC alarm time.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_alarm(&alarm)) {
*               // Error handler
*           }
*           if(HAL_RTC_STATUS_OK != hal_rtc_enable_alarm()) {
*               // Error handler
*           }
*       }
*
*    @endcode
*
* - Get the RTC alarm time. \n
*  - Step 1. Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2. Call #hal_rtc_get_alarm() to get the RTC alarm time.
*  - sample code:
*    @code
*       get_alarm_time(uint16_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec) {
*           hal_rtc_time_t alarm;
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_get_alarm(&alarm)) {
*               // Error handler
*           }
*
*           // The user has to define the base year and the RTC year is defined
*           // as an offset. For example, define the base year as 2000 and assign 15 to the RTC year to represent the year of 2015.
*           *year = alarm.rtc_year + BASE_YEAR;
*           *mon = alarm.rtc_mon;
*           *day = alarm.rtc_day;
*           *hour = alarm.rtc_hour;
*           *min = alarm.rtc_min;
*           *sec = alarm.rtc_sec;
*       }
*
*    @endcode
*
* - Register a user callback function to handle the RTC alarm. \n
*  - Step 1. Call #hal_rtc_init() to initialize the RTC hardware if it has not been called after the power went on.
*  - Step 2. Call #hal_rtc_set_alarm_callback() to register a user callback function to handle the RTC alarm.
*  - Step 3. Call #hal_rtc_enable_alarm() to enable an alarm notification. A callback function is triggered in an
*           interrupt service routine if an alarm notification is received.
*  - sample code:
*    @code
*       uint32_t g_user_data;
*
*       set_alarm_handle_cb(hal_rtc_alarm_callback_t alarm_handle_cb) {
*
*           if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
*               // Error handler
*           }
*
*           // Register a callback function to handle the RTC alarm.
*           if(HAL_RTC_STATUS_OK != hal_rtc_set_alarm_callback(alarm_handle_cb, &g_user_data)) {
*               // Error handler
*           }
*
*           // Enable an alarm notification.
*           if(HAL_RTC_STATUS_OK != hal_rtc_enable_alarm()) {
*               // Error handler
*           }
*       }
*    @endcode
*    @code
*       void alarm_handle_cb(void *user_data)
*       {
*           // In this case, *user_data is g_user_data.
*           // Apply the RTC functionality, for example, get the RTC current time and update it on the user interface.
*           // Please note, that this callback runs in an interrupt service routine.
*       }
*
*    @endcode
*
* @}
* @}
*/
#ifdef HAL_RTC_FEATURE_SLEEP
/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* - Put the system into sleep if the magic number input is as expected. \n
*  - Call #hal_rtc_sleep() to set the system to sleep mode.
*  - sample code:
*    @code
*       uint32_t magic = 0xBABEBABE; // 0xBABEBABE is the expected magic number.
*       hal_rtc_sleep(magic); // Set the system to sleep mode.
*    @endcode
*
* @}
* @}
*/
#endif /* #ifdef HAL_RTC_FEATURE_SLEEP */

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
* @addtogroup HAL
* @{
* @addtogroup RTC
* @{
* @defgroup hal_rtc_define Define
* @{
*/

#ifdef HAL_RTC_FEATURE_SLEEP
/** @brief  This macro defines a magic number used in #hal_rtc_sleep().
  * Please always use this macro as a parameter when you call #hal_rtc_sleep().
  */
#define HAL_RTC_SLEEP_MAGIC      (0xBABEBABE)
#endif /* #ifdef HAL_RTC_FEATURE_SLEEP */

/**
  * @}
  */

/** @defgroup hal_rtc_enum Enum
  * @{
  */

/** @brief RTC status */
typedef enum {
    HAL_RTC_STATUS_ERROR = -2,                              /**< An error occurred. */
    HAL_RTC_STATUS_INVALID_PARAM = -1,                      /**< Invalid parameter is given. */
    HAL_RTC_STATUS_OK = 0                                   /**< The operation completed successfully. */
} hal_rtc_status_t;

/**
  * @}
  */

/** @defgroup hal_rtc_typedef Typedef
  * @{
  */

/** @brief Callback function definition to handle the RTC current time change. \n
 *  @param[in] user_data is a pointer to the data assigned by #hal_rtc_set_time_callback(). \n
 */
typedef void (*hal_rtc_time_callback_t)(void *user_data);

/** @brief Callback function definition to handle the RTC alarm. \n
 *  @param[in] user_data is a pointer to the data assigned by #hal_rtc_set_alarm_callback(). \n
 */
typedef void (*hal_rtc_alarm_callback_t)(void *user_data);

/**
  * @}
  */

/**
 * @brief This function initializes the RTC module. This function must be called once the power is on and before using the RTC service.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_init(void);

/**
 * @brief This function deinitializes the RTC module.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_deinit(void);

/**
 * @brief This function sets the RTC current time.
 * @param[in] time is a pointer to the #hal_rtc_time_t structure that contains the date and time settings for the RTC current time.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid time parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_time(const hal_rtc_time_t *time);

/**
 * @brief This function gets the RTC current time.
 * @param[out] time is a pointer to the #hal_rtc_time_t structure to store the date and time settings received from the RTC current time.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Get RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_get_time(hal_rtc_time_t *time);

/**
 * @brief This function sets the RTC alarm time.
 * @param[in] time is a pointer to the hal_rtc_time_t structure that contains the date and time settings to configure the RTC alarm time.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid alarm time parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_alarm(const hal_rtc_time_t *time);

/**
 * @brief This function gets the RTC alarm time.
 * @param[out] time is a pointer to the hal_rtc_time_t structure to store the date and time settings received from the RTC alarm time.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Get RTC alarm time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_get_alarm(hal_rtc_time_t *time);

/**
 * @brief This function enables an alarm.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Register a user callback function for handling alarm" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_enable_alarm(void);

/**
 * @brief This function disables an alarm. Call this function if the alarm notification is no longer required.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_disable_alarm(void);

/**
 * @brief This function sets the RTC time notification callback, and the callback execution period is set by #hal_rtc_set_time_notification_period().
 * @param[in] callback_function is a user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_time_notification_period()
 * @par Example
 * Please refer to "Set RTC current time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_time_callback(hal_rtc_time_callback_t callback_function, void *user_data);

#ifdef HAL_RTC_FEATURE_SLEEP
/**
 * @brief This function sets the system into sleep mode. This function should not be called from ISR and the caller is
 *        responsible to ensure the ISR masks won't block the expecting ISRs, which will then
 *        set the system into sleep mode with no option to wake up.
 * @param[in] magic is the magic number of this function. Please always use #HAL_RTC_SLEEP_MAGIC.
 * @return #HAL_RTC_STATUS_ERROR, enter RTC mode failed, due to wakeup signal is active before the chip power off.
 *         #HAL_RTC_STATUS_INVALID_PARAM, an invalid magic parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_sleep(uint32_t magic);
#endif /* #ifdef HAL_RTC_FEATURE_SLEEP */

/**
 * @brief This function sets the alarm callback. This callback is executed if the RTC current and alarm times are the same.
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_alarm(), #hal_rtc_get_alarm(), #hal_rtc_enable_alarm() and #hal_rtc_disable_alarm()
 * @par Example
 * Please refer to "Register a user callback function for handling alarm" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_set_alarm_callback(const hal_rtc_alarm_callback_t callback_function, void *user_data);

#ifdef HAL_RTC_FEATURE_CALIBRATION
/**
 * @brief This function sets the tick value that will be adjusted to the RTC current time. Note, this function should be executed only once per second.
 * @param[in] ticks is the ticks that are adjusted to the RTC current time. The valid value is in a range from -2048(-62.5ms) to 2045(62.4ms). The unit is about 30.52us (1s/32768).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_one_shot_calibration()
 */
hal_rtc_status_t hal_rtc_set_one_shot_calibration(int16_t ticks);

/**
 * @brief This function gets the tick value that will be adjusted to the RTC current time. Note, ticks will be equal to zero after executing this function if one shot calibration is finished.
 * @param[in] ticks is the ticks that are adjusted to the RTC current time. The valid value is in a range from -2048(-62.5ms) to 2045(62.4ms). The unit is about 30.52us (1s/32768).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_one_shot_calibration()
 */
hal_rtc_status_t hal_rtc_get_one_shot_calibration(int16_t *ticks);

/**
 * @brief This function sets the ticks that are periodically adjusted to the RTC current time per 8 seconds.
 * @param[in] ticks_per_8_seconds is the ticks that are adjusted to the RTC current time per 8 seconds. The valid value is in a range from -8192(-31.25ms) to 8191(31.246ms). The unit is about 3.81us (1s/32768/8).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_repeat_calibration()
 */
hal_rtc_status_t hal_rtc_set_repeat_calibration(int16_t ticks_per_8_seconds);

/**
 * @brief This function gets the ticks that are periodically adjusted to the RTC current time per 8 seconds.
 * @param[in] ticks_per_8_seconds is the ticks that are adjusted to the RTC current time per 8 seconds. The valid value is in a range from -8192(-31.25ms) to 8191(31.246ms). The unit is about 3.81us (1s/32768/8).
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_repeat_calibration().
 */
hal_rtc_status_t hal_rtc_get_repeat_calibration(int16_t *ticks_per_8_seconds);
#endif /* #ifdef HAL_RTC_FEATURE_CALIBRATION */

/**
 * @brief This function stores data to the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup date is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] buf is the address of buffer to store the data write to the RTC spare registers.
 * @param[in] len is the datalength stored in the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_get_data(), #hal_rtc_clear_data().
 * @par       Example
 * @code
 *       // Write 3 bytes from buf to RTC spare registers, bytes 2 to 4.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_set_data(2, buf, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_set_data(uint16_t offset, const char *buf, uint16_t len);

/**
 * @brief This function reads data from the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup data is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] buf is the address of buffer to store the data received from the RTC spare registers.
 * @param[in] len is the datalength read from the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_data(), #hal_rtc_clear_data().
 * @par       Example
 * @code
 *       // Read 3 bytes from the RTC spare registers byte 2~4 and store the 3 bytes data to buf
 *       if(HAL_RTC_STATUS_OK != hal_rtc_get_data(2, buf, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_get_data(uint16_t offset, char *buf, uint16_t len);

/**
 * @brief This function stores zero to the RTC registers that won't be cleared even if the system power is off, except when the battery is removed. Note, the size of the backup date is #HAL_RTC_BACKUP_BYTE_NUM_MAX bytes.
 * @param[in] offset is the position of RTC spare registers to store data. The unit is in bytes.
 * @param[in] len is the datalength read from the RTC spare registers. The unit is in bytes.
 * @return #HAL_RTC_STATUS_ERROR means the RTC hardware isn't ready for use.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @sa #hal_rtc_set_data(), #hal_rtc_get_data().
 * @par       Example
 * @code
 *       // Set the RTC spare registers (bytes 2 to 4) with zeros.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_clear_data(2, 3)) {
 *           // Error handler
 *       }
 * @endcode
 */
hal_rtc_status_t hal_rtc_clear_data(uint16_t offset, uint16_t len);

/**
 * @brief This function gets the frequency value of 32.768kHz clock source, unit is in Hz.
 * @param[in] frequency is a pointer to store the frequency.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_get_f32k_frequency(uint32_t *frequency);

#ifdef HAL_RTC_FEATURE_RETENTION_SRAM
/**
 * @brief This function configures the memory cells in power down, sleep or
 *        normal mode. Note, that user cannot directly switch
 *        between power down and sleep modes.
 * @param[in] mask is the bitwise of any memory cell to be configured.
 *            The total memory cell number is #HAL_RTC_RETENTION_SRAM_NUMBER_MAX.
 * @param[in] mode is the operation mode of the memory cell to
 *       be configured. Details are described at #hal_rtc_sram_mode_t.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid magic parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *
 *       // Set memory cell 0 and memory cell 2 as Normal mode.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_retention_sram_config(0x5, HAL_RTC_SRAM_NORMAL_MODE)) {
 *           // Error handler
 *       }
 *
 * @endcode
 */
hal_rtc_status_t hal_rtc_retention_sram_config(uint32_t mask, hal_rtc_sram_mode_t mode);

/**
 * @brief This function sets the chip into retention mode.
 * @return #HAL_RTC_STATUS_ERROR, entering retention mode has failed as a wake up event occurred.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_enter_retention_mode(void);

/**
 * @brief This function clears the RTC retention flag, resets RTC retention SRAM configuration, and
 *        switches 32kHz clock source from EOSC to DCXO when internal 32k mode is used after exiting
 *        from the retention mode.
 */
void hal_rtc_exit_retention_mode(void);
#endif /* #ifdef HAL_RTC_FEATURE_RETENTION_SRAM */

#ifdef HAL_RTC_FEATURE_ELAPSED_TICK
/**
 * @brief This function sets the chip into rtc mode.
 * @return #HAL_RTC_STATUS_ERROR, entering rtc mode has failed as a wake up event occurred.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_enter_rtc_mode(void);

/**
 * @brief This function sets the RTC MD alarm time.
 * @param[in] time is a pointer to the hal_rtc_time_t structure that contains the date and time settings to configure the RTC MD alarm time.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid alarm time parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_set_md_alarm(const hal_rtc_time_t *time);

/**
 * @brief This function sets the MD alarm callback. This callback is executed if the RTC MD current and alarm times are the same.
 * @param[in] callback_function is the user-defined callback function.
 * @param[in] user_data is a pointer to the data assigned to callback function.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_set_md_alarm_callback(const hal_rtc_alarm_callback_t callback_function, void *user_data);

/**
 * @brief This function enables a md alarm.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Register a user callback function for handling alarm" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_enable_md_alarm(void);

/**
 * @brief This function disables a md alarm. Call this function if the alarm notification is no longer required.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
hal_rtc_status_t hal_rtc_disable_md_alarm(void);

/**
 * @brief This function gets the RTC md alarm time.
 * @param[out] time is a pointer to the hal_rtc_time_t structure to store the date and time settings received from the RTC alarm time.
 * @return #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par Example
 * Please refer to "Get RTC alarm time" in @ref HAL_RTC_Driver_Usage_Chapter.
 */
hal_rtc_status_t hal_rtc_get_md_alarm(hal_rtc_time_t *time);

/**
 * @brief This function stores the current RTC time and internal counter information
 *        if the parameter is false, and calculates the elapsed 32k tick number from
 *        last call of this function with parameter as false to the current time point
 *        if the parameter is true.
 * @param[in] is_after_sleep is the flag to determine whether system already back
 *            from deep sleep mode.
 * @param[out] elapsed_tick is the elapsed 32k tick number. If is_after_sleep is false,
 *             elapsed_tick is 0, if is_after_sleep is true, elapsed_tick is the
 *             elapsed 32k tick number from last call of this function with parameter
 *             as false to the current time point.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par       Example
 * @code
 *
 *       static uint32_t elapsed_tick;
 *       // Stores the current RTC time and internal counter information before entering deep sleep mode.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_get_elapsed_tick(false, &elapsed_tick)) {
 *           // Error handler
 *       }
 *
 * @endcode
 * @code
 *
 *       static uint32_t elapsed_tick;
 *       // Call function to get the elapsed 32k tick number after exiting deep sleep mode.
 *       if(HAL_RTC_STATUS_OK != hal_rtc_get_elapsed_tick(true, &elapsed_tick)) {
 *           // Error handler
 *       }
 *
 * @endcode
 */
hal_rtc_status_t hal_rtc_get_elapsed_tick(bool is_after_sleep, uint32_t *elapsed_tick);

/**
 * @brief This function configures the RTC GPIO output. RTC GPIO can either controlled by software directly or by RTC
 *        power-on event (EINT, Tick, or Alarm), user also need to clear RTC GPIO output high with their specific requirement.
 *        This API will not keep former register setting, user need configure all of the setting needed when call this API.
 * @param[in] pin is the pin number that needs to apply related configuration setting.
 * @param[in] gpio_control is the configuration setting for the RTC GPIO output control.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par  Example
 * @code
 *
 *       hal_rtc_gpio_control_t gpio_control;
 *       // Control RTC GPIO 0 output high by software.
 *       gpio_control.is_sw_control = true;
 *       gpio_control.is_sw_output_high = true;
 *       hal_rtc_configure_gpio(HAL_RTC_GPIO_0, &gpio_control);
 *
 *       // Control RTC GPIO 0 output low by software.
 *       gpio_control.is_sw_output_high = false;
 *       hal_rtc_configure_gpio(HAL_RTC_GPIO_0, &gpio_control);
 *
 * @endcode
 * @code
 *
 *       hal_rtc_gpio_control_t gpio_control;
 *       // Control RTC GPIO 0 as when an RTC alarm or EINT occurs, GPIO 0 will be set high.
 *       gpio_control.is_sw_control = false;
 *       gpio_control.is_alarm_output_high = true;
 *       gpio_control.is_eint_output_high = true;
 *       gpio_control.is_tick_output_high = false;
 *       hal_rtc_configure_gpio(HAL_RTC_GPIO_0, &gpio_control);
 *
 *       // Clear RTC GPIO 0 output high, which caused by related hardware signal.
 *       gpio_control.is_clear_output = true;
 *       hal_rtc_configure_gpio(HAL_RTC_GPIO_0, &gpio_control);
 *
 * @endcode
 */
hal_rtc_status_t hal_rtc_configure_gpio(hal_rtc_gpio_t pin, hal_rtc_gpio_control_t *gpio_control);

/**
 * @brief This function configures the RTC EINT setting. If RTC EINT is enabled, user can use this API
 *        to configure some detailed setting for EINT, like active edge choose, whether add debounce time or not.
 *        EINT signal input should be active at least for one 32k clock cycle, i.e. 30.6us, then de-active EINT signal.
 *        If debounce is enabled, this signal should be active at least for four 32k clock cycles.
 * @param[in] eint_config is the configuration setting for the RTC EINT control.
 * @return #HAL_RTC_STATUS_INVALID_PARAM, an invalid parameter is given.
 *         #HAL_RTC_STATUS_OK, the operation completed successfully.
 * @par  Example
 * @code
 *
 *       hal_rtc_eint_config_t eint_config;
 *       eint_config.is_enable_rtc_eint = true;
 *       eint_config.is_falling_edge_active = true;
 *       eint_config.is_enable_debounce = false;
 *       hal_rtc_eint_init(&eint_config);
 *
 * @endcode
 */
hal_rtc_status_t hal_rtc_eint_init(hal_rtc_eint_config_t *eint_config);

#endif /* #ifdef HAL_RTC_FEATURE_ELAPSED_TICK */

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/**
* @}
* @}
*/

#endif /* #ifdef HAL_RTC_MODULE_ENABLED */
#endif /* #ifndef __HAL_RTC_H__ */

