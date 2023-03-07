/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_rng.h>
#include <blog.h>

/**
 * hal rng demo
 */

void demo_hosal_rng(void)
{
    uint8_t buf[8];
    int ret = -1;
    /*init rng */
    hosal_rng_init();

    /*fill random value into buf */
    ret = hosal_random_num_read(buf, sizeof(buf)/sizeof(buf[0]));
    if (ret != 0) {
        blog_error("rng read error\r\n");
    }
    for (int i = 0; i < sizeof(buf)/sizeof(buf[0]); i++) {
        blog_info("%d \r\n", buf[i]);
    }
}
