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
 * hal flash busaddr test demo
 */
void demo_hosal_flash_busaddr_test(void)
{
    hosal_flash_dev_t *p_flash;
    hosal_logic_partition_t part;

    /* flash partition open use address 0 */
    p_flash = hosal_flash_open("DATA", HOSAL_FLASH_FLAG_ADDR_0 | HOSAL_FLASH_FLAG_BUSADDR);
    if (p_flash == NULL) {
        blog_error("no partition name DATA!\r\n");
        return;
    }

    /* get flash partition info */
    hosal_flash_info_get(p_flash, &part);
    blog_info("partition name         : %s\r\n", part.partition_description);
    blog_info("partition start address: 0x%08x\r\n", part.partition_start_addr);
    blog_info("partition length       : %d\r\n", part.partition_length);

    /* close the flash partition and free buf */
    hosal_flash_close(p_flash);
}
