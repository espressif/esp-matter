
/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_spi.h>
#include <blog.h>
/* About Pin:
 * if pin % 4 is 0 ===> this pin can be used as spi mosi function
 * if pin % 4 is 1 ===> this pin can be used as spi miso function
 * if pin % 4 is 2 ===> this pin can be used as spi cs   function
 * if pin % 4 is 3 ===> this pin can be used as spi sclk function
 * such as: GLB_GPIO_PIN_0 ===> mosi
 *          GLB_GPIO_PIN_1 ===> miso
 *          GLB_GPIO_PIN_2 ===> cs
 *          GLB_GPIO_PIN_3 ===> sclk
 * about cs pin: for master device, user can use hardware cs pin like pin2,and can also use software to select any pin for cs , for slave device ,user can only use hardwrae cs
 * about mosi and miso pin: mosi can be used as miso when miso can be uesd as mosi
 */

uint8_t send_data[6];
uint8_t recv_data[6];
/* spi callback */
void spi_master_cb(void *arg)
{
    for (int i = 0; i < 6; i++) {
        blog_info("master send is %d\r\n",send_data[i]);
        blog_info("master recv is %d\r\n",recv_data[i]);
    }
    blog_info("master send complete\r\n");
}

void demo_hosal_spi_master(void)
{
    hosal_spi_dev_t spi;
    /* spi port set */
    spi.port = 0;
    /* spi master mode */
    spi.config.mode  = HOSAL_SPI_MODE_MASTER;  
   
    /* 1: enable dma, 0: disable dma */
     
    spi.config.dma_enable = 0;               
     /* 0: phase 0, polarity low                                           
      * 1: phase 1, polarity low                                           
      * 2: phase 0, polarity high                                          
      * 3: phase 0, polarity high
      */
    spi.config.polar_phase= 0;               
    /* 0 ~ 40M */
    spi.config.freq= 100000;
    spi.config.pin_clk = 3;
    /* hardware cs now is pin 2 */
    spi.config.pin_mosi= 0;
    spi.config.pin_miso= 1;
    /* init spi device */
    hosal_spi_init(&spi);

    /* register trans complete callback */
    hosal_spi_irq_callback_set(&spi, spi_master_cb, (void*)&spi);
    memset(recv_data, 0, 6);

    /* prepara send buf */
    for (int i = 0; i < sizeof(send_data)/sizeof(send_data[0]); i++) {
        send_data[i] = i + 2;
    }
    /* software set pin4 as cs pin and set cs low to slect slave */
    hosal_spi_set_cs(4, 0);

    /*send and recv data, timeout is 10s */
    hosal_spi_send_recv(&spi, (uint8_t *)send_data, (uint8_t *)recv_data, 6, 10000);
    /* set cs high, release slave */
    hosal_spi_set_cs(4, 1);
    hosal_spi_finalize(&spi);
}

