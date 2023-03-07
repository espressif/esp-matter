/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_rtc.h>
#include <blog.h>


void demo_hosal_rtc_DEC(void) {
    int ret1 = -1;
    int ret2 = -1;
    /* init rtc DEC format*/
    hosal_rtc_dev_t rtc;
    rtc.port = 0;
    rtc.config.format = HOSAL_RTC_FORMAT_DEC; 
    hosal_rtc_init(&rtc);

    /* set rtc time */
    hosal_rtc_time_t time_buf;
    time_buf.sec     = 59;
    time_buf.min     = 59;
    time_buf.hr      = 23;
    time_buf.date    = 31;
    time_buf.month   = 12;
    time_buf.year    = 37;
    ret1 = hosal_rtc_set_time(&rtc, &time_buf);
    if (ret1 != 0) {
        blog_error("set error\r\n");
    }
    blog_info( "set time sec     = %d\r\n", time_buf.sec);
    blog_info( "set time min     = %d\r\n", time_buf.min);
    blog_info( "set time hr      = %d\r\n", time_buf.hr);
    blog_info( "set time date    = %d\r\n", time_buf.date);
    blog_info( "set time month   = %d\r\n", time_buf.month);
    blog_info( "set time year    = %d\r\n", time_buf.year + 2000);

    /* clear time buf and delay 10s */
    memset(&time_buf, 0, sizeof(hosal_rtc_time_t));
    vTaskDelay(10000);
    /* get rtc time */
    ret2 = hosal_rtc_get_time(&rtc, &time_buf);
    if (ret2 != 0) {
        blog_error("set error\r\n");
    }
    blog_info( "get time sec     = %d\r\n", time_buf.sec);
    blog_info( "get time min     = %d\r\n", time_buf.min);
    blog_info( "get time hr      = %d\r\n", time_buf.hr);
    blog_info( "get time date    = %d\r\n", time_buf.date);
    blog_info( "get time month   = %d\r\n", time_buf.month);
    blog_info( "get time year    = %d\r\n", time_buf.year + 2000);

    hosal_rtc_finalize(&rtc);

}

