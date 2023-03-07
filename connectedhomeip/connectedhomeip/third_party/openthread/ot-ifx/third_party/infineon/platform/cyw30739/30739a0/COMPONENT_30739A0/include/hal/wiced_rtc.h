/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/** @file
*
* List of parameters and defined functions needed to access Rtc driver
*
*/

#ifndef __WICED_RTC_H__
#define __WICED_RTC_H__

/**  \addtogroup RTCDriver
* \ingroup HardwareDrivers
*/
/*! @{ */
/**
* Defines a driver to facilitate interfacing with the RTC.
*
*/

/** broken-down calendar representation of time */
typedef struct
{
    uint16_t  second;                      /**< seconds (0 - 59), not support leap seconds*/
    uint16_t  minute;                      /**< minutes (0 - 59),*/
    uint16_t  hour;                        /**< hours (0 - 23),  */
    uint16_t  day;                         /**< day of the month (1 - 31) */
    uint16_t  month;                       /**< month (0 - 11, 0=January) */
    uint16_t  year;                        /**< year, should larger then 2010 */
} wiced_rtc_time_t;

/** Real time clock read from hardware (48 bits) */
typedef union
{
    uint16_t          wiced_rtc64;

    struct
    {
        uint16_t      wiced_rtc16[4];
    }wiced_reg16map;

    struct
    {
        uint32_t      wiced_rtc32[2];
    }wiced_reg32map;
} wiced_real_time_clock_t;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_rtc_init
 *
 *                  Initialize the RTC block.
 *
 * @param[in]       void
 *
 * @return          void
 */
void wiced_rtc_init(void);

/**
 * Function         wiced_rtc_get_raw_clock
 *
 *                  Reads Real time clock value from hardware clock.
 *
 * @param[out]       rtcClock   : buffer to for the read data
 *
 * @return          void
 */
void wiced_rtc_get_raw_clock(wiced_real_time_clock_t *rtcClock);

/**
 * Function         wiced_rtc_get_time
 *
 *                  Get current time in wiced_rtc_time_t broken-down calender format
 *
 * @param[out]       timebuf   : timer object pointer to be stored for the return value
 *
 * @return          void
 */
void wiced_rtc_get_time(wiced_rtc_time_t *timebuf);

/**
 * Function         wiced_rtc_set_reference_time
 *
 *                  Set the reference time to the given reference. Useful when the app sets one time
 *                  and then enters deep sleep, and then wakes up and then wants the ref time to
 *                  still be the one that was set the reference to the original wiced_set_rtc_time call.
 *
 * @param[out]       ref_time   : The original reference time
 *
 * @return          void
 */
void wiced_rtc_set_reference_time(wiced_rtc_time_t* ref_time);

/**
 * Function         wiced_set_rtc_time
 *
 *                  Set the current time.
 *
 * @param[in]       newTime   : timer object pointer for current time
 *
 * @return          WICED_TRUE if success else return WICED_FALSE
 *
 * Note : This function call will assume the current time is later then 2010/1/1.
 */
wiced_bool_t wiced_set_rtc_time(wiced_rtc_time_t  *newTime);

/**
 * Function         wiced_rtc_ctime
 *
 *                  This function call will convert the wiced_rtc_time_t object pointed by wiced_rtc_time_t to
 *                  c string containing a human-readable verion of the correspoing
 *                  local time and data. Caller should make sure *outbuf size larger then 22 bytes.
 *
 *                  The returned string has the folloing format
 *                  Mmm dd hh:mm:ss yyyy
 *                  where
 *                      Mmm - the month in letters
 *                      dd  - the day of the month
 *                      hh:mm:ss -  time
 *                      yyyy - year
 *
 * @param[in]       timer   : timer object pointer to be converted
 * @param[out]      outbuf  : buffer for the converted string
 *
 * @return          WICED_TRUE if success else return WICED_FALSE
 */
wiced_bool_t wiced_rtc_ctime(wiced_rtc_time_t *timer, char *outbuf);

/**
 * Function         wiced_seconds_to_rtc_time
 *
 *                  convert the 32 bit seconds to RTC_time broken down format
 *
 * @param[in]       second   : 32-bits seconds to be converted
 * @param[out]      rtctime  : converted timer object pointer
 *
 * @return          void
 */
void wiced_seconds_to_rtc_time(uint32_t second, wiced_rtc_time_t *rtctime);

/**
 * Function         wiced_rtc_time_to_seconds
 *
 *                  Convert RTC_time broken-down format to 32-bits seocnds
 *
 * @param[in]       rtctime   : timer object pointer to be converted
 * @param[out]      second    : converted uint32_t seconds
 *
 * @return          void
 */
void wiced_rtc_time_to_seconds(wiced_rtc_time_t *rtctime, uint32_t *second);

/* @} */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_RTC_H__
