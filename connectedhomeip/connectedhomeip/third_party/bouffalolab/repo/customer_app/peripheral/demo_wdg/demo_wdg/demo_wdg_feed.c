/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_wdg.h>
#include <blog.h>


void demo_hosal_wdg_feed(void)
{
    hosal_wdg_dev_t wdg;
    int ret = -1;

    /* wdg port set */
    wdg.port = 0;
    /* max timeout is 65532/16 ms */
    wdg.config.timeout = 3000;
    /* init wdg with the given settings */
    hosal_wdg_init(&wdg);

    blog_info("wdg will not trig reboot\r\n");
    /* feed wdg every 2s */
    while(1) {
        hosal_wdg_reload(&wdg);
        vTaskDelay(2000);
    }

    hosal_wdg_finalize(&wdg);
}
