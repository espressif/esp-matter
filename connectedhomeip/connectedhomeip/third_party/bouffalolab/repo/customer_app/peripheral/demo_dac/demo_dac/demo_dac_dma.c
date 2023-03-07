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
#include <vfs.h>
#include <fs/vfs_romfs.h>
#include "audio.h"

#ifdef CONF_USER_BL702
#define DAC_PIN         11
#elif CONF_USER_BL602
#define DAC_PIN         13
#endif

static hosal_dac_dev_t dac0;

/* dac dma callback */
static void user_cb(void *arg)
{
    hosal_dac_dev_t *dac = (hosal_dac_dev_t *)arg;
    hosal_dac_dma_stop(dac);
    hosal_dac_finalize(dac);
    blog_info("success\r\n");
}

void demo_hosal_dac_dma(void)
{
    int ret = 0;
    uint32_t *p_u32addr;
    uint32_t bufsize;

    dac0.port = 0;                   /* only one dac,so port must be 0 */
    dac0.config.dma_enable = 1;
    dac0.config.pin = DAC_PIN;              /* for 602 :only two channel:0-->channel A -->gpio13, 1-->channel B-->gpio14 */
                                            /* for 702 :only two channel:0-->channel A -->gpio11, 1-->channel B-->gpio17*/
    dac0.config.freq = 32000;               /* if you play audio ,freq must same as audio sample rate ,only support 8k,16k,32k */

    p_u32addr = (uint32_t *)audio_32k;
    bufsize = (uint32_t)audio_32k_len;

    ret = hosal_dac_init(&dac0);
    if (ret != 0) {
        hosal_dac_finalize(&dac0);
        blog_error("dac init failed \r\n");
        return;
    }
    
    hosal_dac_dma_cb_reg(&dac0, user_cb, &dac0);

    ret = hosal_dac_dma_start(&dac0, p_u32addr, bufsize);

    if (ret != 0) {
        hosal_dac_finalize(&dac0);
        blog_error("dac start failed \r\n");
        return;
    }

}
