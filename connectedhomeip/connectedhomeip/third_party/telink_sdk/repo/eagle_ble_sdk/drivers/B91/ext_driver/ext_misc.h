/********************************************************************************************************
 * @file	ext_misc.h
 *
 * @brief	This is the header file for B91
 *
 * @author	BLE Group
 * @date	2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#ifndef DRIVERS_B91_EXT_MISC_H_
#define DRIVERS_B91_EXT_MISC_H_

#include "../analog.h"
#include "../clock.h"
#include "../dma.h"
#include "../gpio.h"
#include "../pm.h"
#include "../timer.h"
#include "../flash.h"
#include "../gpio.h"
#include "../mdec.h"
#include "../trng.h"
#include "../sys.h"
#include "../plic.h"
#include "../pm.h"
#include "../stimer.h"
#include "../sys.h"
#include "../timer.h"
#include "../trng.h"
#include "../uart.h"
#include "common/types.h"
#include "compiler.h"

/******************************* stimer_start ******************************************************************/

#define reg_system_tick_irq reg_system_irq_level

typedef enum
{
    STIMER_IRQ_MASK         = BIT(0),
    STIMER_32K_CAL_IRQ_MASK = BIT(1),
} stimer_irq_mask_e;

typedef enum
{
    FLD_IRQ_SYSTEM_TIMER = BIT(0),
} system_timer_irq_mask_e;

typedef enum
{
    STIMER_IRQ_CLR         = BIT(0),
    STIMER_32K_CAL_IRQ_CLR = BIT(1),
} stimer_irq_clr_e;

/**
 * @brief    This function serves to enable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_enable(void)
{
    reg_irq_src0 |= BIT(IRQ1_SYSTIMER);
    // plic_interrupt_enable(IRQ1_SYSTIMER);
}

/**
 * @brief    This function serves to disable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_disable(void)
{
    reg_irq_src0 &= ~BIT(IRQ1_SYSTIMER);
    // plic_interrupt_disable(IRQ1_SYSTIMER);
}

static inline void systimer_set_irq_mask(void)
{
    reg_system_irq_mask |= STIMER_IRQ_MASK;
}

static inline void systimer_clr_irq_mask(void)
{
    reg_system_irq_mask &= (~STIMER_IRQ_MASK);
}

static inline unsigned char systimer_get_irq_status(void)
{
    return reg_system_cal_irq & FLD_IRQ_SYSTEM_TIMER;
}

static inline void systimer_clr_irq_status(void)
{
    reg_system_cal_irq = STIMER_IRQ_CLR;
}

static inline void systimer_set_irq_capture(unsigned int tick)
{
    reg_system_irq_level = tick;
}

static inline unsigned int systimer_get_irq_capture(void)
{
    return reg_system_irq_level;
}

static inline int tick1_exceed_tick2(u32 tick1, u32 tick2)
{
    return (u32) (tick1 - tick2) < BIT(30);
}
/******************************* stimer_end ********************************************************************/

/******************************* core_start ******************************************************************/
#ifndef USE_ZEPHYR_HAL_HEADERS
#define irq_disable core_interrupt_disable
#define irq_enable core_interrupt_enable
#define irq_restore(en) core_restore_interrupt(en)
#endif /* USE_ZEPHYR_HAL_HEADERS */
/******************************* core_end ********************************************************************/



/******************************* analog_start ******************************************************************/
#define analog_write analog_write_reg8
#define analog_read analog_read_reg8

/******************************* analog_end ********************************************************************/



/******************************* clock_start ******************************************************************/
typedef enum
{
    SYSCLK_16M = 16,
    SYSCLK_24M = 24,
    SYSCLK_32M = 32,
    SYSCLK_48M = 48,
    SYSCLK_64M = 64,
} sys_clk_fre_t;

static inline unsigned char clock_get_system_clk()
{
    return sys_clk.cclk;
}
/******************************* clock_end ********************************************************************/



/******************************* trng_start ******************************************************************/
#define random_generator_init trng_init

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     This function performs to generate a series of random numbers
 * @param[in]  len - data length
 * @param[out] data - data pointer
 * @return    none
 **/
void generateRandomNum(int len, unsigned char * data);

/******************************* trng_end ********************************************************************/



/******************************* sys_start ******************************************************************/
#define sleep_us(x) delay_us(x)
#define sleep_ms(x) delay_ms(x)

/******************************* sys_end ********************************************************************/



/******************************* plic_start ******************************************************************/
enum
{ // todo
    FLD_IRQ_EXCEPTION_EN,
    FLD_IRQ_SYSTIMER_EN,
    FLD_IRQ_ALG_EN,
    FLD_IRQ_TIMER1_EN,
    FLD_IRQ_TIMER0_EN,
    FLD_IRQ_DMA_EN,
    FLD_IRQ_BMC_EN,
    FLD_IRQ_USB_CTRL_EP_SETUP_EN,
    FLD_IRQ_USB_CTRL_EP_DATA_EN,
    FLD_IRQ_USB_CTRL_EP_STATUS_EN,
    FLD_IRQ_USB_CTRL_EP_SETINF_EN,
    FLD_IRQ_USB_ENDPOINT_EN,
    FLD_IRQ_ZB_DM_EN,
    FLD_IRQ_ZB_BLE_EN,
    FLD_IRQ_ZB_BT_EN,
    FLD_IRQ_ZB_RT_EN,
    FLD_IRQ_PWM_EN,
    FLD_IRQ_PKE_EN, // add
    FLD_IRQ_UART1_EN,
    FLD_IRQ_UART0_EN,
    FLD_IRQ_DFIFO_EN,
    FLD_IRQ_I2C_EN,
    FLD_IRQ_SPI_APB_EN,
    FLD_IRQ_USB_PWDN_EN,
    FLD_IRQ_EN,
    FLD_IRQ_GPIO2RISC0_EN,
    FLD_IRQ_GPIO2RISC1_EN,
    FLD_IRQ_SOFT_EN,

    FLD_IRQ_NPE_BUS0_EN,
    FLD_IRQ_NPE_BUS1_EN,
    FLD_IRQ_NPE_BUS2_EN,
    FLD_IRQ_NPE_BUS3_EN,
    FLD_IRQ_NPE_BUS4_EN,

    FLD_IRQ_USB_250US_EN,
    FLD_IRQ_USB_RESET_EN,
    FLD_IRQ_NPE_BUS7_EN,
    FLD_IRQ_NPE_BUS8_EN,

    FLD_IRQ_NPE_BUS13_EN = 42,
    FLD_IRQ_NPE_BUS14_EN,
    FLD_IRQ_NPE_BUS15_EN,

    FLD_IRQ_NPE_BUS17_EN = 46,

    FLD_IRQ_NPE_BUS21_EN = 50,
    FLD_IRQ_NPE_BUS22_EN,
    FLD_IRQ_NPE_BUS23_EN,
    FLD_IRQ_NPE_BUS24_EN,
    FLD_IRQ_NPE_BUS25_EN,
    FLD_IRQ_NPE_BUS26_EN,
    FLD_IRQ_NPE_BUS27_EN,
    FLD_IRQ_NPE_BUS28_EN,
    FLD_IRQ_NPE_BUS29_EN,
    FLD_IRQ_NPE_BUS30_EN,
    FLD_IRQ_NPE_BUS31_EN,

    FLD_IRQ_NPE_COMB_EN,
    FLD_IRQ_PM_TM_EN,
    FLD_IRQ_EOC_EN,

};

/******************************* plic_end ********************************************************************/



/******************************* flash_start *****************************************************************/
unsigned int flash_get_jedec_id(void) __attribute__((section(".text")));
void flash_set_capacity(flash_capacity_e flash_cap);
flash_capacity_e flash_get_capacity(void);

/******************************* flash_end *******************************************************************/



/******************************* usb_end *********************************************************************/
#define reg_usb_irq REG_ADDR8(0x100839)
/******************************* usb_end *********************************************************************/



/******************************* core_start ******************************************************************/
// #define SUPPORT_PFT_ARCH 1
#define SUPPORT_PFT_ARCH 0
/******************************* core_end ********************************************************************/



/******************************* uart_start ******************************************************************/
void uart_receive_dma_set(dma_chn_e chn, unsigned char * addr, unsigned int rev_size) __attribute__((section(".ram_code")))
__attribute__((noinline));
/******************************* uart_end ********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_B91_EXT_MISC_H_ */
