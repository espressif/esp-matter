/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_i2c.h>
#include <blog.h>

/* About Pin:
*  if one pin % 2 is 0 ===> this pin can be used as i2c scl function
*  if one pin % 2 is 1 ===> this pin can be used as i2c sda function
*  such as: GLB_GPIO_PIN_0 ===> scl
*           GLB_GPIO_PIN_1 ===> sda
*/

#define CK_IIC_SLAVE_ADDR         0x50

static hosal_i2c_dev_t i2c0;
/* data buffer */
static uint8_t i2c_data_txbuf[32] = {0};
static uint8_t i2c_data_rxbuf[32] = {0};

void demo_hosal_i2c_master(void)
{
    int ret     = -1;
    int i       = 0;

    i2c0.port = 0;
    i2c0.config.freq = 100000;                                    /* only support 305Hz~100000Hz */
    i2c0.config.address_width = HOSAL_I2C_ADDRESS_WIDTH_7BIT;     /* only support 7bit */
    i2c0.config.mode = HOSAL_I2C_MODE_MASTER;                     /* only support master */
    i2c0.config.scl = 4;
    i2c0.config.sda = 3;

    /* init i2c with the given settings */
    ret = hosal_i2c_init(&i2c0);
    if (ret != 0) {
        hosal_i2c_finalize(&i2c0);
        blog_error("hosal i2c init failed!\r\n");
        return;
    }

    /* init the tx buffer */
    for (i = 0; i < 32; i++) {
        i2c_data_txbuf[i] = i;
    }

    ret = hosal_i2c_master_send(&i2c0, CK_IIC_SLAVE_ADDR, i2c_data_txbuf, 32, 1000);
    vTaskDelay(500);
    if (ret != 0) {
        hosal_i2c_finalize(&i2c0);
        blog_error("hosal i2c write failed!\r\n");
        return;
    } 
    
    ret = hosal_i2c_master_recv(&i2c0, CK_IIC_SLAVE_ADDR, i2c_data_rxbuf, 32, 3000);
    if (ret != 0) {
        hosal_i2c_finalize(&i2c0);
        blog_error("hosal i2c read failed!\r\n");
        return;
    }
    vTaskDelay(500);

    ret = hosal_i2c_finalize(&i2c0);
    if (ret != 0) {
        blog_error("hosal i2c finalize failed!\r\n");
        return;
    }
}


