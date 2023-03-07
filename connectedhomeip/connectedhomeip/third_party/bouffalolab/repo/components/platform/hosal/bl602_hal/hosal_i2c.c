/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <FreeRTOS.h>
#include <task.h>
#include "bl602.h"
#include "bl_i2c.h"
#include "bl602_i2c.h"
#include "bl602_glb.h"
#include "hosal_i2c.h"
#include "blog.h"
#include "bl_timer.h"

#define HOSAL_PUT_UINT32_LE(n, b, i)                \
{                                                   \
    (b)[(i)    ] = (uint8_t) ( (n)       );         \
    (b)[(i) + 1] = (uint8_t) ( (n) >>  8 );         \
    (b)[(i) + 2] = (uint8_t) ( (n) >> 16 );         \
    (b)[(i) + 3] = (uint8_t) ( (n) >> 24 );         \
}


static int i2c_master_send(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg, uint32_t timeout)
{
    uint8_t i = 0;
    uint32_t temp = 0;
    uint32_t I2Cx = I2C_BASE;
    int64_t time_start, time_end;

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_WRITE, cfg);
    I2C_Enable(i2cNo);

    time_start = bl_timer_now_us64() / 1000;
    /* Set I2C write data */
    for (i = 0; i < cfg->dataSize; i++) {
        temp += (cfg->data[i] << ((i % 4) * 8));
        if ((i + 1) % 4 == 0) {
            while (BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_TX_FIFO_CNT) == 0) {
                time_end = bl_timer_now_us64() / 1000;
                if (HOSAL_WAIT_FOREVER == timeout) {
                    continue;
                }
                if ((time_end - time_start) >= timeout) {
                    I2C_Disable(i2cNo);
                    return -1;
                }
            }
            BL_WR_REG(I2Cx, I2C_FIFO_WDATA, temp);
            temp = 0;
        }
    }
    if ((cfg->dataSize % 4) != 0) {
        while (BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_TX_FIFO_CNT) == 0) {
            time_end = bl_timer_now_us64() / 1000;
            if (HOSAL_WAIT_FOREVER == timeout) {
                continue;
            }
            if ((time_end - time_start) >= timeout) {
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        BL_WR_REG(I2Cx, I2C_FIFO_WDATA, temp);
    }

    while (I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo)) {
        time_end = bl_timer_now_us64() / 1000;
        if (HOSAL_WAIT_FOREVER == timeout) { 
            continue;
        }
        if ((time_end - time_start) >= timeout) {
            I2C_Disable(i2cNo);
            return -1;
        }
    }

    I2C_Disable(i2cNo);

    return 0;
}

static int i2c_master_receive(I2C_ID_Type i2cNo, I2C_Transfer_Cfg *cfg, uint32_t timeout)
{
    uint8_t i = 0;
    uint32_t temp = 0;
    uint32_t I2Cx = I2C_BASE;
    int64_t time_start, time_end;

    I2C_Disable(i2cNo);
    I2C_Init(i2cNo, I2C_READ, cfg);
    I2C_Enable(i2cNo);
    
    time_start = bl_timer_now_us64() / 1000;
    /* Read I2C data */
    while (cfg->dataSize - i >= 4) {
        while (BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_RX_FIFO_CNT) == 0) {
            time_end = bl_timer_now_us64() / 1000;
            if (HOSAL_WAIT_FOREVER == timeout) {
                continue;
            }
            if ((time_end - time_start) >= timeout) {
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        temp = BL_RD_REG(I2Cx, I2C_FIFO_RDATA);
        HOSAL_PUT_UINT32_LE(temp, cfg->data, i);
        i += 4;
    }
    if (i < cfg->dataSize) {
        while (BL_GET_REG_BITS_VAL(BL_RD_REG(I2Cx, I2C_FIFO_CONFIG_1), I2C_RX_FIFO_CNT) == 0) {
            time_end = bl_timer_now_us64() / 1000;
            if (HOSAL_WAIT_FOREVER == timeout) {
                continue;
            }
            if ((time_end - time_start) >= timeout) {
                I2C_Disable(i2cNo);
                return -1;
            }
        }
        temp = BL_RD_REG(I2Cx, I2C_FIFO_RDATA);
        while (i < cfg->dataSize) {
            cfg->data[i] = (temp & 0xff);
            temp = (temp >> 8);
            i++;
        }
    }

    while (I2C_IsBusy(i2cNo) || !I2C_TransferEndStatus(i2cNo)) {
        time_end = bl_timer_now_us64() / 1000;
        if (HOSAL_WAIT_FOREVER == timeout) {
            continue;
        }
        if ((time_end - time_start) >= timeout) {
            I2C_Disable(i2cNo);
            return -1;
        }
    }

    I2C_Disable(i2cNo);

    return 0;
}

int hosal_i2c_init(hosal_i2c_dev_t *i2c)
{
    GLB_GPIO_Type gpiopins[2];

    if (NULL == i2c || i2c->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    gpiopins[0] = i2c->config.scl;
    gpiopins[1] = i2c->config.sda;

    GLB_GPIO_Func_Init(GPIO_FUN_I2C, gpiopins, sizeof(gpiopins) / sizeof(gpiopins[0]));
    I2C_Disable(i2c->port);
    i2c_set_freq(i2c->config.freq, 0);

    return 0;
}

int hosal_i2c_master_send(hosal_i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_Transfer_Cfg i2c_cfg_send = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 0,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    i2c_cfg_send.slaveAddr = dev_addr;
    i2c_cfg_send.data = (uint8_t *)data;
    i2c_cfg_send.dataSize = size;

    return i2c_master_send(0, &i2c_cfg_send, timeout);
}

int hosal_i2c_master_recv(hosal_i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    I2C_Transfer_Cfg i2c_cfg_recv = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 0,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    i2c_cfg_recv.slaveAddr = dev_addr;
    i2c_cfg_recv.data = (uint8_t *)data;
    i2c_cfg_recv.dataSize = size;

    return i2c_master_receive(0, &i2c_cfg_recv, timeout);
}

int hosal_i2c_slave_send(hosal_i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    blog_error("not support now!\r\n");

    return -1;
}

int hosal_i2c_slave_recv(hosal_i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    blog_error("not support now!\r\n");

    return -1;
}

int hosal_i2c_mem_write(hosal_i2c_dev_t *i2c, uint16_t dev_addr, uint32_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    I2C_Transfer_Cfg i2c_cfg_send = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 2,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    i2c_cfg_send.slaveAddr = dev_addr;
    i2c_cfg_send.subAddr = mem_addr;
    i2c_cfg_send.subAddrSize = mem_addr_size;
    i2c_cfg_send.data = (uint8_t *)data;
    i2c_cfg_send.dataSize = size;

    return i2c_master_send(0, &i2c_cfg_send, timeout);
}

int hosal_i2c_mem_read(hosal_i2c_dev_t *i2c, uint16_t dev_addr, uint32_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    I2C_Transfer_Cfg i2c_cfg_recv = {
        .slaveAddr = 0x50,
        .stopEveryByte = DISABLE,
        .subAddrSize = 2,
        .subAddr = 0x00,
        .dataSize = 0,
        .data = NULL,
    };

    if (NULL == i2c || NULL == data || size < 1) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    i2c_cfg_recv.slaveAddr = dev_addr;
    i2c_cfg_recv.subAddr = mem_addr;
    i2c_cfg_recv.subAddrSize = mem_addr_size;
    i2c_cfg_recv.data = (uint8_t *)data;
    i2c_cfg_recv.dataSize = size;

    return i2c_master_receive(0, &i2c_cfg_recv, timeout);
}

int hosal_i2c_finalize(hosal_i2c_dev_t *i2c)
{
    if (i2c == NULL || i2c->port != 0) {
        blog_error("parameter is error!\r\n");
        return -1;
    }

    I2C_Disable(i2c->port);

    return 0;
}
