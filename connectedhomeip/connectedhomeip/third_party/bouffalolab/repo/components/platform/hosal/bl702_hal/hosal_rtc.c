/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <hosal_rtc.h>
#include "bl_rtc.h"
#include <FreeRTOS.h>
#include <task.h>
#include <blog.h>

#define SEC_PER_MIN  ((time_t)60)
#define SEC_PER_HOUR ((time_t)60 * SEC_PER_MIN)
#define SEC_PER_DAY  ((time_t)24 * SEC_PER_HOUR)

static struct tm *s_rtc_base = NULL;

static const uint8_t leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static inline int clock_isleapyear(int year)
{
    return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int clock_daysbeforemonth(int month, uint8_t leapyear)
{
    int retval = g_noleap_daysbeforemonth[month];

    if (month >= 2 && leapyear) {
        retval++;
    }

    return retval;
}

static inline int __bcd_to_int(unsigned char value)
{
    int temp = 0;
    temp = (value>>4)*10;
    temp += value&0x0f;
    return temp;
}

static inline unsigned char __int_to_bcd(int value)
{
	unsigned char temp = 0;
	temp = (value/10)*16;
	temp += (value%10);
	return temp;
}

static void clock_utc2calendar(time_t days, int *year, int *month,
                               int *day)
{

    /* There is one leap year every four years, so we can get close with the
     * following:
     */

    int value   = days  / (4 * 365 + 1); /* Number of 4-years periods since the epoch */
    days   -= value * (4 * 365 + 1); /* Remaining days */
    value <<= 2;                     /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    uint8_t leapyear;
    int  tmp;

    for (; ;) {
        /* Is this year a leap year (we'll need this later too) */

        leapyear = clock_isleapyear(value + 1970);

        /* Get the number of days in the year */

        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */

        if (days >= tmp) {
            /* Yes.. bump up the year */

            value++;
            days -= tmp;
        } else {
            /* Nope... then go handle months */

            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */

    *year = 1970 + value;

    /* Handle the month (zero based) */
    int  min = 0;
    int  max = 11;

    do {
        /* Get the midpoint */

        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */

        tmp = clock_daysbeforemonth(value + 1, leapyear);

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */

        if (tmp > days) {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */

            tmp = clock_daysbeforemonth(value, leapyear);

            if (tmp > days) {
                /* No... The one we want is somewhere between min and value-1 */

                max = value - 1;
            } else {
                /* Yes.. 'value' contains the month that we want */

                break;
            }
        } else {
            /* No... The one we want is somwhere between value+1 and max */

            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */

        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */
    days -= clock_daysbeforemonth(value, leapyear);

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */

    *month = value + 1; /* 1-based */
    *day   = days + 1;  /* 1-based */
}

static int __check_tm_ok(struct tm *rtctime)
{

    if (rtctime->tm_year < 70 || rtctime->tm_year >= 200) {
        goto error_time;
    }

    int32_t leap = 1;

    leap = clock_isleapyear(rtctime->tm_year + 1900);

    if (rtctime->tm_sec < 0 || rtctime->tm_sec >= 60) {
        goto error_time;
    }

    if (rtctime->tm_min < 0 || rtctime->tm_min >= 60) {
        goto error_time;
    }

    if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24) {
        goto error_time;
    }

    if (rtctime->tm_mon < 0 || rtctime->tm_mon >= 12) {
        goto error_time;
    }

    if (leap) {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > leap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    } else {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > noleap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    }

    return 0;
error_time:
    return -1;
}

#define _INT_TO_BCD(is_bcd, num) (is_bcd)?__int_to_bcd((num)):(num)
#define _BCD_TO_INT(is_bcd, num) (is_bcd)?__bcd_to_int((num)):(num)

static void __tm_to_rtctime(hosal_rtc_time_t *rtc_time, const struct tm *time, uint8_t is_bcd)
{
    rtc_time->sec = _INT_TO_BCD(is_bcd, time->tm_sec);
    rtc_time->min = _INT_TO_BCD(is_bcd, time->tm_min);
    rtc_time->hr = _INT_TO_BCD(is_bcd, time->tm_hour);
    //rtc_time->weekday = _INT_TO_BCD(is_bcd, time->tm_wday);
    rtc_time->date = _INT_TO_BCD(is_bcd, time->tm_mday);
    rtc_time->month = _INT_TO_BCD(is_bcd, time->tm_mon + 1);
    rtc_time->year = _INT_TO_BCD(is_bcd, time->tm_year - 70);
}

static void __rtctime_to_tm(struct tm *tim, const hosal_rtc_time_t *time, uint8_t is_bcd)
{
    tim->tm_sec = _BCD_TO_INT(is_bcd, time->sec);
    tim->tm_min = _BCD_TO_INT(is_bcd, time->min);
    tim->tm_hour = _BCD_TO_INT(is_bcd, time->hr);
    //tim->tm_wday = _BCD_TO_INT(is_bcd, time->weekday);
    tim->tm_mday = _BCD_TO_INT(is_bcd, time->date);
    tim->tm_mon = _BCD_TO_INT(is_bcd, time->month);
    tim->tm_mon -= 1;
    tim->tm_year = _BCD_TO_INT(is_bcd, time->year);
    tim->tm_year += 70;
}
static time_t __clock_calendar2utc(int year, int month, int day)
{
    time_t days;

    /* Years since epoch in units of days (ignoring leap years). */

    days = (year - 1970) * 365;

    /* Add in the extra days for the leap years prior to the current year. */

    days += (year - 1969) >> 2;

    /* Add in the days up to the beginning of this month. */

    days += (time_t)clock_daysbeforemonth(month, clock_isleapyear(year));

    /* Add in the days since the beginning of this month (days are 1-based). */

    days += day - 1;

    /* Then convert the seconds and add in hours, minutes, and seconds */

    return days;
}

static struct tm *__gmtime_r(const time_t *timer, struct tm *result)
{
    time_t epoch;
    time_t jdn;
    int    year;
    int    month;
    int    day;
    int    hour;
    int    min;
    int    sec;

    /* Get the seconds since the EPOCH */

    epoch = *timer;

    /* Convert to days, hours, minutes, and seconds since the EPOCH */

    jdn    = epoch / SEC_PER_DAY;
    epoch -= SEC_PER_DAY * jdn;

    hour   = epoch / SEC_PER_HOUR;
    epoch -= SEC_PER_HOUR * hour;

    min    = epoch / SEC_PER_MIN;
    epoch -= SEC_PER_MIN * min;

    sec    = epoch;

    /* Convert the days since the EPOCH to calendar day */
    clock_utc2calendar(jdn, &year, &month, &day);

    /* Then return the struct tm contents */

    result->tm_year  = (int)year - 1900; /* Relative to 1900 */
    result->tm_mon   = (int)month - 1;   /* zero-based */
    result->tm_mday  = (int)day;         /* one-based */
    result->tm_hour  = (int)hour;
    result->tm_min   = (int)min;
    result->tm_sec   = (int)sec;
    return result;
}

static time_t __mktime(struct tm *tp)
{
    time_t ret;
    time_t jdn;

    /* Get the EPOCH-relative julian date from the calendar year,
     * month, and date
     */

    ret = __check_tm_ok(tp);
    if (ret < 0) {
        return -1;
    }

    jdn = __clock_calendar2utc(tp->tm_year + 1900, tp->tm_mon, tp->tm_mday);

    /* Return the seconds into the julian day. */

    ret = ((jdn * 24 + tp->tm_hour) * 60 + tp->tm_min) * 60 + tp->tm_sec;

    return ret;
}

 /**
 * This function will initialize the on board CPU real time clock
 *
 *
 * @param[in]  rtc  rtc device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_init(hosal_rtc_dev_t *rtc)
{
    if (rtc == NULL) {
        return -1;
    }
    if (NULL == s_rtc_base) {
        s_rtc_base = pvPortMalloc(sizeof(struct tm));
        if (NULL == s_rtc_base) {
            return -1;
        }
        bl_rtc_init();
    }

    return 0;
}

/**
 * This function will return the value of time read from the on board CPU real time clock.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_get_time(hosal_rtc_dev_t *rtc, hosal_rtc_time_t *time)
{
    struct tm tim;
    uint64_t time_stamp_ms = bl_rtc_get_timestamp_ms();
    
    memset(&tim, 0, sizeof(struct tm));
    if (time == NULL || rtc == NULL) {
        return -1;
    }

    time_stamp_ms = time_stamp_ms / 1000;
    time_stamp_ms += __mktime(s_rtc_base);
    __gmtime_r((const time_t *)&time_stamp_ms, &tim);

    __tm_to_rtctime(time, &tim, (rtc->config.format == HOSAL_RTC_FORMAT_BCD));

    return 0;
}

/**
 * This function will set MCU RTC time to a new value.
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_set_time(hosal_rtc_dev_t *rtc, const hosal_rtc_time_t *time)
{
    int ret;
    struct tm tim;

    if (time == NULL || rtc == NULL) {
        return -1;
    }
    if (NULL == s_rtc_base) {
        return -1;
    }

    __rtctime_to_tm(&tim, time, (rtc->config.format == HOSAL_RTC_FORMAT_BCD));
    if ((ret = __check_tm_ok(&tim)) != 0) {
        return ret;
    }
    __rtctime_to_tm(s_rtc_base, time, (rtc->config.format == HOSAL_RTC_FORMAT_BCD));
    return 0;
}

/**
 * This function will set MCU RTC time to a new value by time stamp
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time_stamp new rtc time vaalue  
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_set_count(hosal_rtc_dev_t *rtc, uint64_t *time_stamp)
{
    struct tm tim;
    hosal_rtc_time_t time;
    if (rtc == NULL) {
        return -1;
    }
    if (NULL == s_rtc_base) {
        return -1;
    }
    memset(&tim, 0, sizeof(struct tm));
    memset(&time, 0, sizeof(struct tm));
    __gmtime_r((const time_t *)time_stamp, s_rtc_base);
    return 0;
}

/**
 * This function will return the value of time read from the on board CPU real time clock in time stamp
 *
 * @param[in]   rtc   rtc device
 * @param[out]  time  pointer to a time structure
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_get_count(hosal_rtc_dev_t *rtc, uint64_t *time_stamp)
{
    uint64_t time_stamp_ms = bl_rtc_get_timestamp_ms();
    if (rtc == NULL) {
        return -1;
    }
    time_stamp_ms = time_stamp_ms / 1000;
    time_stamp_ms += __mktime(s_rtc_base);
    *time_stamp = time_stamp_ms;
    return 0;
}

/**
 * De-initialises an RTC interface, Turns off an RTC hardware interface
 *
 * @param[in]  RTC  the interface which should be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int hosal_rtc_finalize(hosal_rtc_dev_t *rtc)
{
    if (rtc == NULL) {
        return -1;
    }

    if (s_rtc_base) {
        vPortFree(s_rtc_base);
        s_rtc_base = NULL;
    }

    return 0;
}

