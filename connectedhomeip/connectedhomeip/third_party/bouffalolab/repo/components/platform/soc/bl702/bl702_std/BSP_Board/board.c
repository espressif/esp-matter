/**
 * @file board.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_clock.h"
#include "hal_uart.h"
#include "bl702_glb.h"
#include "bl702_pwm.h"
#include "bl702_timer.h"
#include "bl702_clock.h"
#include "bl702_config.h"
#include "bflb_platform.h"

struct pin_mux_cfg {
    uint8_t pin;
    uint8_t func;
};

static const struct pin_mux_cfg af_pin_table[] = {
#ifdef CONFIG_GPIO0_FUNC
    { .pin = GLB_GPIO_PIN_0,
      .func = CONFIG_GPIO0_FUNC },
#endif
#ifdef CONFIG_GPIO1_FUNC
    { .pin = GLB_GPIO_PIN_1,
      .func = CONFIG_GPIO1_FUNC },
#endif
#ifdef CONFIG_GPIO2_FUNC
    { .pin = GLB_GPIO_PIN_2,
      .func = CONFIG_GPIO2_FUNC },
#endif
#ifdef CONFIG_GPIO3_FUNC
    { .pin = GLB_GPIO_PIN_3,
      .func = CONFIG_GPIO3_FUNC },
#endif
#ifdef CONFIG_GPIO4_FUNC
    { .pin = GLB_GPIO_PIN_4,
      .func = CONFIG_GPIO4_FUNC },
#endif
#ifdef CONFIG_GPIO5_FUNC
    { .pin = GLB_GPIO_PIN_5,
      .func = CONFIG_GPIO5_FUNC },
#endif
#ifdef CONFIG_GPIO6_FUNC
    { .pin = GLB_GPIO_PIN_6,
      .func = CONFIG_GPIO6_FUNC },
#endif
#ifdef CONFIG_GPIO7_FUNC
    { .pin = GLB_GPIO_PIN_7,
      .func = CONFIG_GPIO7_FUNC },
#endif
#ifdef CONFIG_GPIO8_FUNC
    { .pin = GLB_GPIO_PIN_8,
      .func = CONFIG_GPIO8_FUNC },
#endif
#ifdef CONFIG_GPIO9_FUNC
    { .pin = GLB_GPIO_PIN_9,
      .func = CONFIG_GPIO9_FUNC },
#endif
#ifdef CONFIG_GPIO10_FUNC
    { .pin = GLB_GPIO_PIN_10,
      .func = CONFIG_GPIO10_FUNC },
#endif
#ifdef CONFIG_GPIO11_FUNC
    { .pin = GLB_GPIO_PIN_11,
      .func = CONFIG_GPIO11_FUNC },
#endif
#ifdef CONFIG_GPIO12_FUNC
    { .pin = GLB_GPIO_PIN_12,
      .func = CONFIG_GPIO12_FUNC },
#endif
#ifdef CONFIG_GPIO13_FUNC
    { .pin = GLB_GPIO_PIN_13,
      .func = CONFIG_GPIO13_FUNC },
#endif
#ifdef CONFIG_GPIO14_FUNC
    { .pin = GLB_GPIO_PIN_14,
      .func = CONFIG_GPIO14_FUNC },
#endif
#ifdef CONFIG_GPIO15_FUNC
    { .pin = GLB_GPIO_PIN_15,
      .func = CONFIG_GPIO15_FUNC },
#endif
#ifdef CONFIG_GPIO16_FUNC
    { .pin = GLB_GPIO_PIN_16,
      .func = CONFIG_GPIO16_FUNC },
#endif
#ifdef CONFIG_GPIO17_FUNC
    { .pin = GLB_GPIO_PIN_17,
      .func = CONFIG_GPIO17_FUNC },
#endif
#ifdef CONFIG_GPIO18_FUNC
    { .pin = GLB_GPIO_PIN_18,
      .func = CONFIG_GPIO18_FUNC },
#endif
#ifdef CONFIG_GPIO19_FUNC
    { .pin = GLB_GPIO_PIN_19,
      .func = CONFIG_GPIO19_FUNC },
#endif
#ifdef CONFIG_GPIO20_FUNC
    { .pin = GLB_GPIO_PIN_20,
      .func = CONFIG_GPIO20_FUNC },
#endif
#ifdef CONFIG_GPIO21_FUNC
    { .pin = GLB_GPIO_PIN_21,
      .func = CONFIG_GPIO21_FUNC },
#endif
#ifdef CONFIG_GPIO22_FUNC
    { .pin = GLB_GPIO_PIN_22,
      .func = CONFIG_GPIO22_FUNC },
#endif
#ifdef CONFIG_GPIO23_FUNC
    { .pin = GLB_GPIO_PIN_23,
      .func = CONFIG_GPIO23_FUNC },
#endif
#ifdef CONFIG_GPIO24_FUNC
    { .pin = GLB_GPIO_PIN_24,
      .func = CONFIG_GPIO24_FUNC },
#endif
#ifdef CONFIG_GPIO25_FUNC
    { .pin = GLB_GPIO_PIN_25,
      .func = CONFIG_GPIO25_FUNC },
#endif
#ifdef CONFIG_GPIO26_FUNC
    { .pin = GLB_GPIO_PIN_26,
      .func = CONFIG_GPIO26_FUNC },
#endif
#ifdef CONFIG_GPIO27_FUNC
    { .pin = GLB_GPIO_PIN_27,
      .func = CONFIG_GPIO27_FUNC },
#endif
#ifdef CONFIG_GPIO28_FUNC
    { .pin = GLB_GPIO_PIN_28,
      .func = CONFIG_GPIO28_FUNC },
#endif
#ifdef CONFIG_GPIO29_FUNC
    { .pin = GLB_GPIO_PIN_29,
      .func = CONFIG_GPIO29_FUNC },
#endif
#ifdef CONFIG_GPIO30_FUNC
    { .pin = GLB_GPIO_PIN_30,
      .func = CONFIG_GPIO30_FUNC },
#endif
#ifdef CONFIG_GPIO31_FUNC
    { .pin = GLB_GPIO_PIN_31,
      .func = CONFIG_GPIO31_FUNC },
#endif
};

static void board_pin_mux_init(void)
{
    GLB_GPIO_Cfg_Type gpio_cfg;
    uint32_t tmpVal;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    uint8_t hbn_gpio_mask = 0x1f;
    uint8_t hbn_aon_ie = 0;

    for (int i = 0; i < sizeof(af_pin_table) / sizeof(af_pin_table[0]); i++) {
        gpio_cfg.gpioMode = GPIO_MODE_AF;
        gpio_cfg.pullType = GPIO_PULL_UP;
        gpio_cfg.gpioPin = af_pin_table[i].pin;
        gpio_cfg.gpioFun = af_pin_table[i].func;

        /*if using gpio9-gpio12 and func is not analog and output ,should set reg_aon_pad_ie_smt corresponding bit = 1*/
        if ((af_pin_table[i].pin > GLB_GPIO_PIN_8) && (af_pin_table[i].pin < GLB_GPIO_PIN_13)) {
            if ((af_pin_table[i].func != 10) && ((af_pin_table[i].func < GPIO_FUN_GPIO_OUTPUT_UP) || (af_pin_table[i].func > GPIO_FUN_GPIO_OUTPUT_NONE)))
                hbn_aon_ie |= (1 << (af_pin_table[i].pin - 9));
        }

        /*if reset state*/
        if (af_pin_table[i].func == GPIO_FUN_UNUSED) {
            continue;
        } else if (af_pin_table[i].func == GPIO_FUN_WAKEUP) {
            /*if hbn or pds gpio wakeup func*/
            if (af_pin_table[i].pin < GLB_GPIO_PIN_8) {
                /*enable pds gpio wakeup and irq unmask*/
                tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_INT_SELECT, af_pin_table[i].pin);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_INT_MODE, PDS_AON_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
                tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_INT_MASK);
                BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);
            } else if ((af_pin_table[i].pin > GLB_GPIO_PIN_8) && (af_pin_table[i].pin < GLB_GPIO_PIN_13)) {
                hbn_gpio_mask &= ~(1 << (af_pin_table[i].pin - 9));
            }
            continue;
        } else if ((af_pin_table[i].func == GPIO_FUN_USB) || (af_pin_table[i].func == GPIO_FUN_DAC) || (af_pin_table[i].func == GPIO_FUN_ADC)) {
            /*if analog func , for usb、adc、dac*/
            gpio_cfg.gpioFun = GPIO_FUN_ANALOG;
            gpio_cfg.gpioMode = GPIO_MODE_ANALOG;
            gpio_cfg.pullType = GPIO_PULL_NONE;
        } else if ((af_pin_table[i].func & 0xF0) == 0xF0) {
            /*if uart func*/
            gpio_cfg.gpioFun = GPIO_FUN_UART;
            uint8_t uart_func = af_pin_table[i].func & 0x07;
            uint8_t uart_sig = gpio_cfg.gpioPin % 8;
            /*link to one uart sig*/
            GLB_UART_Fun_Sel((GLB_UART_SIG_Type)uart_sig, (GLB_UART_SIG_FUN_Type)uart_func);
            GLB_UART_Fun_Sel((GLB_UART_SIG_Type)uart_func, (GLB_UART_SIG_FUN_Type)uart_sig);
        } else if (af_pin_table[i].func == GPIO_FUN_PWM) {
            /*if pwm func*/
            gpio_cfg.pullType = GPIO_PULL_DOWN;
        } else if (af_pin_table[i].func == GPIO_FUN_QDEC) {
            /* if qdec a/b */
            gpio_cfg.pullType = GPIO_PULL_NONE;
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.gpioFun = GPIO_FUN_QDEC;
        } else if (af_pin_table[i].func == GPIO_FUN_QDEC_LED) {
            /* if qdec led */
            gpio_cfg.pullType = GPIO_PULL_NONE;
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.gpioFun = GPIO_FUN_QDEC;
        } else if (af_pin_table[i].func == GPIO_FUN_CLK_OUT) {
            if (af_pin_table[i].pin % 2) {
                /*odd gpio output clock*/
                GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
            } else {
                /*even gpio output clock*/
                GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
            }
        } else if ((af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_UP) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_FALLING_EDGE) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_LOW_LEVEL)) {
            /*if common gpio func,include input、output and exti*/
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;

            if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_FALLING_EDGE) {
                GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            } else if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_LOW_LEVEL) {
                GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
            }
        } else if ((af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_DOWN) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_RISING_EDGE) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_HIGH_LEVEL)) {
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;

            if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_RISING_EDGE) {
                GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
            } else if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_HIGH_LEVEL) {
                GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
            }
        } else if (af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_NONE) {
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
        } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_UP) {
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;
        } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_DOWN) {
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;
        } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_NONE) {
            gpio_cfg.gpioFun = GPIO_FUN_GPIO;
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
        }
        GLB_GPIO_Init(&gpio_cfg);
    }

    /*disable unused reg_aon_pad_ie_smt bits and hbn_pin_wakeup_mask bits*/
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, hbn_gpio_mask);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, hbn_aon_ie);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
}

#if XTAL_TYPE != EXTERNAL_XTAL_32M
static void internal_rc32m_init(void)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_CAPCODE_EXTRA_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_INT_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_TSEN, tmpVal);

    for (uint32_t i = 0; i < 20000; i++) {
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
        BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
        BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
        if (BL_IS_REG_BIT_SET(BL_RD_REG(GLB_BASE, GLB_CLK_CFG0), GLB_CHIP_RDY))
            break;
    }
}
#endif

static void peripheral_clock_gate_all()
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_GPIP));
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC_DBG));
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ1));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ2));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_DMA));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_EMAC));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_UART0));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_UART1));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SPI));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_I2C));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_PWM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TMR));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_IRR));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CKS));
    tmpVal &= (~(1 << 24)); //QDEC0
    tmpVal &= (~(1 << 25)); //QDEC1
    tmpVal &= (~(1 << 26)); //QDEC2/I2S
    tmpVal &= (~(1 << 27)); //KYS
    tmpVal &= (~(1 << BL_AHB_SLAVE1_USB));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CAM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_MJPEG));
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);
}

void system_clock_init(void)
{
#if XTAL_TYPE != EXTERNAL_XTAL_32M
    internal_rc32m_init();
    AON_Power_Off_XTAL();
#endif
    /*select root clock*/
    GLB_Set_System_CLK(XTAL_TYPE, BSP_ROOT_CLOCK_SOURCE);
#if BSP_ROOT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_57P6M
    /* fix 57.6M */
    SystemCoreClockSet(57.6 * 1000 * 1000);
#endif
    /*set fclk/hclk and bclk clock*/
    GLB_Set_System_CLK_Div(BSP_FCLK_DIV, BSP_BCLK_DIV);
    /* Set MTimer the same frequency as SystemCoreClock */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_BCLK) / 1000 / 1000 - 1);
#ifndef FAST_WAKEUP
#ifdef BSP_AUDIO_PLL_CLOCK_SOURCE
    PDS_Set_Audio_PLL_Freq(BSP_AUDIO_PLL_CLOCK_SOURCE - ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ);
#endif
#endif
#if XTAL_32K_TYPE == INTERNAL_RC_32K
    HBN_32K_Sel(HBN_32K_RC);
    HBN_Power_Off_Xtal_32K();
#else
    HBN_Power_On_Xtal_32K();
    HBN_32K_Sel(HBN_32K_XTAL);
#endif
#if XTAL_TYPE == EXTERNAL_XTAL_32M
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
#else
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_RC32M);
#endif
}

void peripheral_clock_init(void)
{
    uint32_t tmpVal = 0;

    peripheral_clock_gate_all();

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
#if defined(BSP_USING_UART0)
    tmpVal |= (1 << BL_AHB_SLAVE1_UART0);
#endif
#if defined(BSP_USING_UART1)
    tmpVal |= (1 << BL_AHB_SLAVE1_UART1);
#endif
#if BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_96M
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_96M, BSP_UART_CLOCK_DIV);
#elif BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_FCLK, BSP_UART_CLOCK_DIV);
#else
#error "please select correct uart clock source"
#endif
#endif

#if defined(BSP_USING_I2C0)
#if BSP_I2C_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK
    tmpVal |= (1 << BL_AHB_SLAVE1_I2C);
    GLB_Set_I2C_CLK(ENABLE, BSP_I2C_CLOCK_DIV);
#else
#error "please select correct i2c clock source"
#endif
#endif

#if defined(BSP_USING_SPI0)
#if BSP_SPI_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK
    tmpVal |= (1 << BL_AHB_SLAVE1_SPI);
    GLB_Set_SPI_CLK(ENABLE, BSP_SPI_CLOCK_DIV);
#else
#error "please select correct spi clock source"
#endif
#endif

#if defined(BSP_USING_TIMER0)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#else
#error "please select correct timer0 clock source"
#endif
#endif

#if defined(BSP_USING_TIMER1)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#else
#error "please select correct timer1 clock source"
#endif
#endif

#if defined(BSP_USING_WDT)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#else
#error "please select correct watchdog timer clock source"
#endif
#endif

#if defined(BSP_USING_PWM_CH0) || defined(BSP_USING_PWM_CH1) || defined(BSP_USING_PWM_CH2) || defined(BSP_USING_PWM_CH3) || defined(BSP_USING_PWM_CH4) || defined(BSP_USING_PWM_CH5)
    tmpVal |= (1 << BL_AHB_SLAVE1_PWM);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    uint32_t timeoutCnt = 160 * 1000;
    uint32_t tmp_pwm;
    uint32_t PWMx;
#if BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_32K);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_BCLK);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_XCLK);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#else
#error "please select correct pwm clock source"
#endif
#endif

#if defined(BSP_USING_IR)
    tmpVal |= (1 << BL_AHB_SLAVE1_IRR);
    GLB_Set_IR_CLK(ENABLE, 0, BSP_IR_CLOCK_DIV);
#endif

#if defined(BSP_USING_I2S0)
    tmpVal |= (1 << BL_AHB_SLAVE1_I2S);
    GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);
#endif

#if defined(BSP_USING_ADC0)
    tmpVal |= (1 << BL_AHB_SLAVE1_GPIP);
#if BSP_ADC_CLOCK_SOURCE >= ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_AUDIO_PLL, BSP_ADC_CLOCK_DIV);
#elif BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, BSP_ADC_CLOCK_DIV);
#else
#error "please select correct adc clock source"
#endif
#endif

#if defined(BSP_USING_DAC0)
    tmpVal |= (1 << BL_AHB_SLAVE1_GPIP);
#if BSP_DAC_CLOCK_SOURCE >= ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_AUDIO_PLL, BSP_DAC_CLOCK_DIV + 1);
#elif BSP_DAC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_XCLK, BSP_DAC_CLOCK_DIV + 1);
#else
#error "please select correct dac clock source"
#endif
#endif

#if defined(BSP_USING_CAM0)
    tmpVal |= (1 << BL_AHB_SLAVE1_CAM);
#if BSP_CAM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_96M
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_DLL96M, BSP_CAM_CLOCK_DIV);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
#elif BSP_CAM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_XCLK, BSP_CAM_CLOCK_DIV);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
#else
#error "please select correct camera clock source"
#endif
#endif

#if defined(BSP_USING_QDEC0) || defined(BSP_USING_QDEC1) || defined(BSP_USING_QDEC2) || defined(BSP_USING_KEYSCAN)
#ifdef BSP_USING_KEYSCAN
    tmpVal |= (1 << 27);
#endif
#if defined(BSP_USING_QDEC0)
    tmpVal |= (1 << 24);
#endif
#if defined(BSP_USING_QDEC1)
    tmpVal |= (1 << 25);
#endif
#if defined(BSP_USING_QDEC2)
    tmpVal |= (1 << 26);
#endif
#if BSP_QDEC_KEYSCAN_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    GLB_Set_QDEC_CLK(GLB_QDEC_CLK_F32K, BSP_QDEC_KEYSCAN_CLOCK_DIV);
#elif BSP_QDEC_KEYSCAN_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_QDEC_CLK(GLB_QDEC_CLK_XCLK, BSP_QDEC_KEYSCAN_CLOCK_DIV);
#else
#error "please select correct qdec or keyscan clock source"
#endif
#endif

#if defined(BSP_USING_USB)
    tmpVal |= (1 << BL_AHB_SLAVE1_USB);
    GLB_Set_USB_CLK(1);
#endif
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);
}

static void board_clock_init(void)
{
    system_clock_init();
    peripheral_clock_init();
}

void bl_show_info(void)
{
    MSG("\r\n");
    MSG("  ____               __  __      _       _       _     \r\n");
    MSG(" |  _ \\             / _|/ _|    | |     | |     | |    \r\n");
    MSG(" | |_) | ___  _   _| |_| |_ __ _| | ___ | | __ _| |__  \r\n");
    MSG(" |  _ < / _ \\| | | |  _|  _/ _` | |/ _ \\| |/ _` | '_ \\ \r\n");
    MSG(" | |_) | (_) | |_| | | | || (_| | | (_) | | (_| | |_) |\r\n");
    MSG(" |____/ \\___/ \\__,_|_| |_| \\__,_|_|\\___/|_|\\__,_|_.__/ \r\n");
    MSG("\r\n");
    MSG("Build:%s,%s\r\n", __TIME__, __DATE__);
    MSG("Copyright (c) 2021 Bouffalolab team\r\n");

#if 0
    MSG("root clock:%dM\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_ROOT) / 1000000); /*root clock before f_div*/

    MSG("fclk clock:%dM\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_FCLK) / 1000000);       /*after f_div,this is system core clock*/
    MSG("bclk clock:%dM\r\n", Clock_System_Clock_Get(BL_SYSTEM_CLOCK_BCLK) / 1000000);

    MSG("uart clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_UART0) / 1000000);
    MSG("spi clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SPI0) / 1000000);
    MSG("i2c clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2C0) / 1000000);
    MSG("adc clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_GPADC) / 1000000);
    MSG("dac clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_GPDAC) / 1000000);
    MSG("i2s clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2S) / 1000000);
    MSG("pwm clock:%dhz\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PWM));
    MSG("cam clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_CAM) / 1000000);
    MSG("timer0 clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_TIMER0) / 1000000);
    MSG("timer1 clock:%dM\r\n", Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_TIMER1) / 1000000);
#endif
}

void board_init(void)
{
    board_clock_init();
    board_pin_mux_init();
}

uint8_t board_get_debug_uart_index(void)
{
    return UART0_INDEX;
}

/* Used for boot2 iap */
void system_mtimer_clock_init(void)
{
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_BCLK) / 1000 / 1000 - 1);
}

void system_mtimer_clock_reinit(void)
{
    /* reinit clock to 10M */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 7);
}

void bflb_early_init(int select_internal_flash)
{
    if(select_internal_flash){
        GLB_Select_Internal_Flash();
    }

    AON_Set_DCDC18_Top_0(0xc, 0x3);
    PDS_Set_Clkpll_Top_Ctrl(0x0);
    AON_Set_Xtal_CapCode(0x30, 0x30);
    AON_Set_Xtal_Cfg(0x2, 0x2, 0x0);
    AON_Set_Xtal_CapCode_Extra(1);

    GLB_Set_USB_CLK(0);
}
