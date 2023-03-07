/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_dac.h>
#include <blog.h>

#ifdef CONF_USER_BL702
#define DAC_PIN         11
#elif CONF_USER_BL602
#define DAC_PIN         14
#endif


static hosal_dac_dev_t dac0;

void demo_hosal_dac(void)
{
    int ret = 0;

    dac0.port = 0;                          /* only one dac,so port must be 0 */
    dac0.config.dma_enable = 0;
    dac0.config.pin = DAC_PIN;              /* for 602 :only two channel:0-->channel A -->gpio13, 1-->channel B-->gpio14 */
                                            /* for 702 :only two channel:0-->channel A -->gpio11, 1-->channel B-->gpio17*/
    dac0.config.freq = 8000;                /* only support 8k,16k,32k */

    blog_info("we are testing dac,wait a miniter!\r\n");
    ret = hosal_dac_init(&dac0);
    if (ret != 0) {
        hosal_dac_finalize(&dac0);
        blog_error("hosal dac init failed!\r\n");
        return;
    }

    /* start dac output valule, this api just for no use dma!!!*/
    ret = hosal_dac_start(&dac0);
    if (ret != 0) {
        hosal_dac_finalize(&dac0);
        blog_error("hosal dac start failed!\r\n");
        return;
    }

    hosal_dac_set_value(&dac0, 1000000);    /* we set output 1V */
    ret = hosal_dac_get_value(&dac0);
    blog_info("value:%d\r\n", ret);

    vTaskDelay(10);

    hosal_dac_stop(&dac0);
    hosal_dac_finalize(&dac0);
}
