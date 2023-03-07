/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_flash.h>
#include <stdlib.h>
#include <blog.h>

#define DEMO_HOSAL_FLASH_TEST_BLOCK 1024

/**
 * hal flash R/W test demo
 */
void demo_hosal_flash_test(void)
{
    int i;
    uint32_t offset = 0;
    uint8_t *p_wbuf, *p_rbuf;
    hosal_flash_dev_t *p_flash;
    hosal_logic_partition_t part;

    /* flash partition open use address 0 */
    p_flash = hosal_flash_open("DATA", HOSAL_FLASH_FLAG_ADDR_0);
    if (p_flash == NULL) {
        blog_error("no partition name DATA!\r\n");
        return;
    }
    
    /* get flash partition info */
    hosal_flash_info_get(p_flash, &part);
    blog_info("partition name         : %s\r\n", part.partition_description);
    blog_info("partition start address: 0x%08x\r\n", part.partition_start_addr);
    blog_info("partition length       : %d\r\n", part.partition_length);

    p_wbuf = malloc(DEMO_HOSAL_FLASH_TEST_BLOCK);
    if (p_wbuf == NULL) {
        blog_error("no memory!\r\n");
        return;
    }

    p_rbuf = malloc(DEMO_HOSAL_FLASH_TEST_BLOCK);
    if (p_rbuf == NULL) {
        blog_error("no memory!\r\n");
        return;
    }

    for (i = 0; i < DEMO_HOSAL_FLASH_TEST_BLOCK; i++) {
        p_wbuf[i] = i & 0xff;
    }

    /* erase flash partition and write data */
    offset = 0;
    hosal_flash_erase_write(p_flash, &offset, p_wbuf, DEMO_HOSAL_FLASH_TEST_BLOCK);
    
    /* read flash partition data */
    offset = 0;
    hosal_flash_read(p_flash, &offset, p_rbuf, DEMO_HOSAL_FLASH_TEST_BLOCK);
    
    /* check flash read data */
    if (memcmp(p_rbuf, p_wbuf, DEMO_HOSAL_FLASH_TEST_BLOCK) != 0) {
        blog_error("hal flash R/W failed!\r\n");
    } else {
        blog_info("hal flash R/W successful!\r\n");
    }
    
    /* close the flash partition and free buf */
    hosal_flash_close(p_flash);
    free(p_wbuf);
    free(p_rbuf);
}
