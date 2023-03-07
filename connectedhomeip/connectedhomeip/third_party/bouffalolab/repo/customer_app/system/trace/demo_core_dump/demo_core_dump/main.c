/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <FreeRTOS.h>
#include <task.h>

#include <aos/kernel.h>
#include <aos/yloop.h>
#include <cli.h>
#include <blog.h>
#include <stdlib.h>

#include"demo.h"

/**
 *  APP main entry
 */
int main(void)
{
    blog_info("demo core dump!\r\n");
    test_cli_init();
    return 0;
}
