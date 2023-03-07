/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <hosal_flash.h>
#include <stdlib.h>
#include <blog.h>

#define DEMO_HOSAL_FLASH_TEST_ADDR 0x192000
#define DEMO_HOSAL_FLASH_TEST_LEN  1024

/**
 * hal flash R/W test demo
 */

void demo_hosal_flash_raw_test(void)
{
    int i;
    uint8_t *p_wbuf, *p_rbuf;

    p_wbuf = malloc(DEMO_HOSAL_FLASH_TEST_LEN);
    if (p_wbuf == NULL) {
        blog_error("no memory!\r\n");
        return;
    }

    p_rbuf = malloc(DEMO_HOSAL_FLASH_TEST_LEN);
    if (p_rbuf == NULL) {
        blog_error("no memory!\r\n");
        return;
    }

    for (i = 0; i < DEMO_HOSAL_FLASH_TEST_LEN; i++) {
        p_wbuf[i] = i & 0xff;
    }
   

    hosal_flash_raw_write(p_wbuf, DEMO_HOSAL_FLASH_TEST_ADDR, DEMO_HOSAL_FLASH_TEST_LEN);

    hosal_flash_raw_read(p_rbuf, DEMO_HOSAL_FLASH_TEST_ADDR, DEMO_HOSAL_FLASH_TEST_LEN);    

    /* check flash read data */
    if (memcmp(p_rbuf, p_wbuf, DEMO_HOSAL_FLASH_TEST_LEN) != 0) {
        blog_error("hal flash R/W failed!\r\n");
    } else {
        blog_info("hal flash R/W successful!\r\n");
    }
    
    free(p_wbuf);
    free(p_rbuf);
}
