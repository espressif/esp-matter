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
#include <bl702_ir.h>
#include <bl702_glb.h>
#include <bl_irq.h>
#include <looprt.h> 
#include <loopset.h>
#include <loopset_ir.h>
#include <blog.h>

static void ir_gpio_init(int pin)
{
    GLB_GPIO_FUNC_Type gpioFuns = 11;
    GLB_GPIO_Func_Init(gpioFuns, (GLB_GPIO_Type*)&pin, 1);

    return;
}

static void ir_init(int pin, int ctrltype)
{
    IR_RxCfg_Type rxcfg = {
        IR_RX_NEC,      /* Set ir rx mode NEC */
        ENABLE,         /* Disable signal of input inverse */
        9000,           /* Pulse width threshold to trigger end condition, 4.5ms @2MHz source clock */
        3400,           /* Pulse width threshold for logic 0/1 detection, 1.7ms */
        DISABLE,        /* Disable input de-glitch function */
        0               /* De-glitch function cycle count */// TODO use De-Glitch
    };

    ir_gpio_init(pin);

    GLB_IR_LED_Driver_Enable();
    GLB_IR_RX_GPIO_Sel(pin);

    IR_Disable(IR_TXRX);
    IR_RxInit((IR_RxCfg_Type*)&rxcfg);
}

uint32_t bl_receivedata(void)
{ 
    return IR_ReceiveData(IR_WORD_0);
}

uint32_t bl_get_bitcount(void)
{
    return IR_GetRxDataBitCount();
}

static int data_check(uint32_t data)
{
    uint16_t hdata = 0;
    uint16_t ldata = 0;

    hdata = ((data >> 24) & 0xff) ^ ((data >> 16) & 0xff);
    ldata = ((data >> 8) & 0xff) ^ (data & 0xff);

    if (hdata == 0xff && ldata == 0xff) {
        return 0;
    } else {
        return -1;
    }
}

static void ir_interrupt_entry(void)
{
    uint32_t data;
    int flag;

    IR_Disable(IR_RX);
    IR_IntMask(IR_INT_RX, MASK);
    IR_ClrIntStatus(IR_INT_RX);

    data = bl_receivedata();
    flag = data_check(data);
    if (flag == -1 && data != 0) {
        blog_debug("invalid data \r\n");
        IR_Enable(IR_RX);
        IR_IntMask(IR_INT_RX, UNMASK);
        return;
    } else {
        /*empty here*/
    }

    //TODO use with irq context
    ir_async_post();
}

void bl_enable_rx_int(void)
{
    IR_Enable(IR_RX);
    IR_IntMask(IR_INT_RX, UNMASK);

    return;
}

int bl_ir_init(int pin, int ctrltype)
{
    ir_init(pin, ctrltype);
    bl_irq_register(IRRX_IRQn, ir_interrupt_entry);
    bl_irq_enable(IRRX_IRQn);
    IR_Enable(IR_RX);
    IR_IntMask(IR_INT_RX, UNMASK);

    return 0;
}
