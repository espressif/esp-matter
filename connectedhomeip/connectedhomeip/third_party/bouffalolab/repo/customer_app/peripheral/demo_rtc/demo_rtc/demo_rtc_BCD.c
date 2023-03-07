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

void demo_hosal_rtc_BCD(void) {
    int ret1 = -1;
    int ret2 = -1;
    /* init rtc BCD format*/
    hosal_rtc_dev_t rtc;
    rtc.port = 0;
    rtc.config.format = HOSAL_RTC_FORMAT_BCD; 
    hosal_rtc_init(&rtc);

    /* set rtc time */
    hosal_rtc_time_t time_buf;
    time_buf.sec     = 0x59;
    time_buf.min     = 0x59;
    time_buf.hr      = 0x23;   
    time_buf.date    = 0x31;
    time_buf.month   = 0x12;
    time_buf.year    = 0x37;
   
    ret1 = hosal_rtc_set_time(&rtc, &time_buf);
    if (ret1 != 0) {
        blog_error("set error\r\n");
    }
    blog_info( "set time sec     = %x\r\n", time_buf.sec);
    blog_info( "set time min     = %x\r\n", time_buf.min);
    blog_info( "set time hr      = %x\r\n", time_buf.hr);
    blog_info( "set time date    = %x\r\n", time_buf.date);
    blog_info( "set time month   = %x\r\n", time_buf.month);
    blog_info( "set time year    = %x\r\n", time_buf.year + 0x2000);

    /* clear time buf and delay 10s */
    memset(&time_buf, 0, sizeof(hosal_rtc_time_t));
    vTaskDelay(10000);

    /* get rtc time */
    ret2 = hosal_rtc_get_time(&rtc, &time_buf);
    if (ret2 != 0) {
        blog_error("set error\r\n");
    }
    blog_info( "get time sec     = %x\r\n", time_buf.sec);
    blog_info( "get time min     = %x\r\n", time_buf.min);
    blog_info( "get time hr      = %x\r\n", time_buf.hr);
    blog_info( "get time date    = %x\r\n", time_buf.date);
    blog_info( "get time month   = %x\r\n", time_buf.month);
    blog_info( "get time year    = %x\r\n", time_buf.year + 0x2000);

    hosal_rtc_finalize(&rtc);

}
