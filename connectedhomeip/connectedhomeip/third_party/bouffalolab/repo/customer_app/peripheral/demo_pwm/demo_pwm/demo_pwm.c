/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_pwm.h>
#include <blog.h>

hosal_pwm_dev_t pwm;
void demo_hosal_pwm_init(void)
{
    uint32_t p_freq;
    uint32_t duty;
    /* pwm port and pin set  note: There is corresponding relationship between port and pin, for bl602, map is  port = pin%5 */
    pwm.port = 0;
    /* pwm config */
    pwm.config.pin = 0;
    pwm.config.duty_cycle = 5000; //duty_cycle range is 0~10000 correspond to 0~100%
    pwm.config.freq = 1000;       //freq range is between 0~40MHZ,for more detail you can reference https://dev.bouffalolab.com/media/doc/602/open/reference_manual/zh/html/content/PWM.html
    /* init pwm with given settings */
    hosal_pwm_init(&pwm);
}

void demo_hosal_pwm_start(void)
{
    /* start pwm */
    hosal_pwm_start(&pwm);
}
void demo_hosal_pwm_change_param(void)
{
    /* change pwm param */
    hosal_pwm_config_t para;
    para.duty_cycle = 8000; //8000/10000=80%
    para.freq = 500;

    hosal_pwm_para_chg(&pwm, para);
}

void demo_hosal_pwm_stop(void)
{
    /* stop pwm */
    hosal_pwm_stop(&pwm);
    hosal_pwm_finalize(&pwm);
}

