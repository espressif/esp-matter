/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cli.h>
#include <hosal_adc.h>
#include <blog.h>
#include <bl_adc.h>

/**********   BL602  ************
*    channel0   ----->     gpio12 
*    channel1   ----->     gpio4
*    channel2   ----->     gpio14
*    channel3   ----->     gpio13
*    channel4   ----->     gpio5
*    channel5   ----->     gpio6
*    channel7   ----->     gpio9
*    channel9   ----->     gpio10
*    channel10  ----->     gpio11
*    channel11  ----->     gpio15
*/

/**********   BL702  ************
*    channel0   ----->     gpio8 
*    channel1   ----->     gpio15
*    channel2   ----->     gpio17
*    channel3   ----->     gpio11
*    channel4   ----->     gpio12
*    channel5   ----->     gpio14
*    channel6   ----->     gpio7
*    channel7   ----->     gpio9
*    channel8   ----->     gpio18
*    channel9   ----->     gpio19
*    channel10  ----->     gpio20
*    channel11  ----->     gpio21
*/

static hosal_adc_dev_t adc0;

void demo_hosal_adc(void)
{
    int ret = 0;
    int i = 0;
    
    adc0.port = 0;
    adc0.config.sampling_freq = 340;

#ifdef CONF_USER_BL602
    adc0.config.pin = 5;
#elif CONF_USER_BL702
    adc0.config.pin = 12;
#endif

    adc0.config.mode = 0;

    /* fot bl602, we only have one adc, if we use tsen for wifi, we may init adc first, 
     * so we can not init adc again, and we can not stop/finish adc if tsen is used.
     * we can use  hosal_adc_device_get() function to get device handle. */
    ret = hosal_adc_init(&adc0);
    if (ret != 0) {
        blog_error("init adc failed. \r\n");
        return;
    }
   
    /* add gpio5(602)/gpio12(702) ,and also channel4 */
    hosal_adc_add_channel(&adc0, 4);

    while (1) {
        vTaskDelay(2000);
        if (i > 10) {
            log_info("finish adc\r\n");
            break;
        }
        i++;    
        ret = hosal_adc_value_get(&adc0, 4, 100);
        if(ret < 0){
            log_error("hosal_adc_value_get error\r\n");
            goto end;
        }
        log_info("adc_output_data = %ld mv \r\n", ret);
    }

end:
    hosal_adc_finalize(&adc0);
}

