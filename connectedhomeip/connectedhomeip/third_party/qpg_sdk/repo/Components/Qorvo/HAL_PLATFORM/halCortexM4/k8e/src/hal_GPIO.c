/*
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for ARM-based devices.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
#define GP_COMPONENT_ID     GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "hal_defs.h"
#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define halGpio_WakeUpMode2Enum(mode) ((hal_WakeUpModeNone == mode)    ? GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE : \
                                      ((hal_WakeUpModeRising == mode)  ? GP_WB_ENUM_WAKEUP_PIN_MODE_RISING_EDGE : \
                                      ((hal_WakeUpModeFalling == mode) ? GP_WB_ENUM_WAKEUP_PIN_MODE_FALLING_EDGE : \
                                      ((hal_WakeUpModeBoth == mode)    ? GP_WB_ENUM_WAKEUP_PIN_MODE_BOTH_EDGES : 0xFF))))

/*****************************************************************************
 *                    GPIO
 *****************************************************************************/
#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
#define __init(port,idx) { ((0x##port-0xA) * 32 + (idx) * 4), ((0x##port-0xA) << 8 | idx) }
#else
#define __init(port,idx) { ((0x##port-0xA) * 32 + (idx) * 4) }
#endif
const hal_gpiodsc_t gpios[] =
{
    [0]  = __init(A,0),
    [1]  = __init(A,1),
    [2]  = __init(A,2),
    [3]  = __init(A,3),
    [4]  = __init(A,4),
    [5]  = __init(A,5),
    [6]  = __init(A,6),
    [7]  = __init(A,7),

    [8]  = __init(B,0),
    [9]  = __init(B,1),
    [10] = __init(B,2),
    [11] = __init(B,3),
    [12] = __init(B,4),
    [13] = __init(B,5),
    [14] = __init(B,6),
    [15] = __init(B,7),

    [16] = __init(C,0),
    [17] = __init(C,1),
    [18] = __init(C,2),
    [19] = __init(C,3),
    [20] = __init(C,4),
    [21] = __init(C,5),
    [22] = __init(C,6),
};
#undef __init
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
typedef struct hal_gpioExti{
    UInt16 gpioPort;         /* LSB = gpioNum num & MSB = gpioNum port */
    hal_cbGpioExti_t cbGpioIsr;
} hal_gpioExti_t;
#endif // HAL_DIVERSITY_GPIO_INTERRUPT
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
 #ifdef HAL_DIVERSITY_GPIO_INTERRUPT
 static hal_gpioExti_t hal_gpioExtiMap[HAL_GPIO_MAX_INTERRUPT_SOURCES];
 #endif // HAL_DIVERSITY_GPIO_INTERRUPT

/*****************************************************************************
 *                    Extern Data Definitions
 *****************************************************************************/
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
static UInt16 hal_gpioGetExtiFromGpio(UInt8 gpio);

static void hal_gpioEnGlobalInt(Bool en)
{
    HAL_DISABLE_GLOBAL_INT();
    /* GPIO INT - route to ARM-CM4 ISR*/
    GP_WB_WRITE_INT_CTRL_MASK_INT_GPIO_INTERRUPT(en);

    /* GPIO ISR */
    NVIC_ClearPendingIRQ(GPIO_IRQn);
    if(en)
    {
        NVIC_EnableIRQ(GPIO_IRQn);
    }
    else
    {
        NVIC_DisableIRQ(GPIO_IRQn);
    }
    HAL_ENABLE_GLOBAL_INT();
}

static UInt16 hal_gpioGetExtiFromGpio(UInt8 gpio)
{
    return gpios[gpio].portIndex;

}
#endif // HAL_DIVERSITY_GPIO_INTERRUPT

Bool hal_gpioGet(hal_gpiodsc_t gpio)
{
    return GP_WB_READ_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_INPUT_VALUE_ADDRESS) + gpio.bitBandOffset);
}

Bool hal_gpioGetSetClr(hal_gpiodsc_t gpio)
{
    return GP_WB_READ_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS) + gpio.bitBandOffset);
}

void hal_gpioSet(hal_gpiodsc_t gpio)
{
    GP_WB_WRITE_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS) + gpio.bitBandOffset, 1);
}

void hal_gpioClr(hal_gpiodsc_t gpio)
{
    GP_WB_WRITE_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS) + gpio.bitBandOffset, 0);
}

void hal_gpioModePP(hal_gpiodsc_t gpio, Bool enable)
{
    GP_WB_WRITE_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_DIRECTION_ADDRESS) + gpio.bitBandOffset, enable);
}

Bool hal_gpioGetModePP(hal_gpiodsc_t gpio)
{
    return GP_WB_READ_U32(GP_WB_ADDR_TO_BITBAND(GP_WB_GPIO_GPIO0_DIRECTION_ADDRESS) + gpio.bitBandOffset);
}

void hal_gpioModePU(UInt8 gpio, Bool enable)
{
    UInt8 shift = ((gpio%4)*2);
    UInt8 setting = enable ? (GP_WB_ENUM_GPIO_MODE_PULLUP << shift) : (GP_WB_ENUM_GPIO_MODE_FLOAT << shift);

    //Set IOB to floating or buskeeper
    GP_WB_MWRITE_U8(GP_WB_IOB_GPIO_0_CFG_ADDRESS + (gpio/4), 0x3 << shift, setting);
}

void hal_gpioModePD(UInt8 gpio, Bool enable)
{
    UInt8 shift = ((gpio%4)*2);
    UInt8 setting = enable ? (GP_WB_ENUM_GPIO_MODE_PULLDOWN << shift) : (GP_WB_ENUM_GPIO_MODE_FLOAT << shift);

    //Set IOB to floating or buskeeper
    GP_WB_MWRITE_U8(GP_WB_IOB_GPIO_0_CFG_ADDRESS + (gpio/4), 0x3 << shift, setting);
}

void hal_gpioSetWakeUpMode(UInt8 gpio, hal_WakeUpMode_t mode)
{
    UInt8 shift;
    UInt8 setting;
    gpHal_Address_t address;

    // Limited set of gpios is capable of setting wakeup mode
    // Wakeup mode is programmed via array of registers, 2 bits per GPIO, corresponding to 3 discontinuous ranges of GPIOs.
    // Map GPIO number to register address and bit index.
    if (/* 0 <= gpio && */ gpio <= 22)
    {
        shift = ((gpio%4)*2);
        address = GP_WB_PMUD_WAKEUP_PIN_MODE_0_ADDRESS + (gpio/4);
    }
    else
    {
        //Unsupported pin for wakeup
        GP_ASSERT_DEV_EXT(false);
        return;
    }

    // Map SW wakeup mode to digital wakeup_pin_mode
    setting = halGpio_WakeUpMode2Enum(mode);

    //Set wakeup mode for pin
    GP_WB_MWRITE_U8(address, 0x3 << shift, setting << shift);
}


#ifdef HAL_DIVERSITY_GPIO_INTERRUPT
/** @brief Disable GPIO interrupt for this GPIO
*
*   @param gpio          GPIO number ( see datasheet )
*/
void hal_gpioDisableInt(UInt8 gpio)
{
    UInt8 gpioExtiMask, gpioToExtiInd;

    gpioToExtiInd = hal_gpioGetExtiFromGpio(gpio);

    HAL_DISABLE_GLOBAL_INT();

    gpioExtiMask = GP_WB_READ_INT_CTRL_MASK_GPIO_INTERRUPTS();
    BIT_CLR(gpioExtiMask, gpioToExtiInd);
    GP_WB_WRITE_INT_CTRL_MASK_GPIO_INTERRUPTS(gpioExtiMask);

    HAL_ENABLE_GLOBAL_INT();
}

/** @brief Enable GPIO interrupt for this GPIO
*
*   @param gpio          GPIO number ( see datasheet )
*/
void hal_gpioEnableInt(UInt8 gpio)
{
    UInt8 gpioExtiMask, gpioToExtiInd;

    gpioToExtiInd = hal_gpioGetExtiFromGpio(gpio);

    HAL_DISABLE_GLOBAL_INT();

    gpioExtiMask = GP_WB_READ_INT_CTRL_MASK_GPIO_INTERRUPTS();
    BIT_SET(gpioExtiMask, gpioToExtiInd);
    GP_WB_WRITE_INT_CTRL_MASK_GPIO_INTERRUPTS(gpioExtiMask);

    HAL_ENABLE_GLOBAL_INT();
}

/** @brief Set GPIO expected value. The expected value is matched against the pin input value. An interrupt is triggered if not-equal
*
*   @param gpio          GPIO number ( see datasheet )
*   @param val              Set expected level (1/0)
*/
void hal_gpioSetExpValue(UInt8 gpio, UInt8 val)
{
    UInt8 gpioExtiinitExpValue = 0;

    UInt8 gpioToExtiInd = hal_gpioGetExtiFromGpio(gpio);

    HAL_DISABLE_GLOBAL_INT();

    if(val)
    {
        BIT_SET(gpioExtiinitExpValue, gpioToExtiInd);
    }
    else
    {
        BIT_CLR(gpioExtiinitExpValue, gpioToExtiInd);
    }

    GP_WB_WRITE_GPIO_EXTI_EXPECTED_VALUE(gpioExtiinitExpValue);

    HAL_ENABLE_GLOBAL_INT();
}

/** @brief Get GPIO expected value
*
*   @param gpio          GPIO number ( see datasheet )
*   @return                 GPIO expected value as set in the register
*/
Bool hal_gpioGetExpValue(UInt8 gpio)
{
    UInt8 gpioToExtiInd = hal_gpioGetExtiFromGpio(gpio);

    UInt8 gpioExtiinitExpValue = GP_WB_READ_GPIO_EXTI_EXPECTED_VALUE();

    return BIT_TST(gpioExtiinitExpValue, gpioToExtiInd);
}

/** @brief Configure GPIO interrupt
*
*   @param gpio          GPIO number ( see datasheet )
*   @param initExpVal       Initial expected value of the pin
*   @param cbExti           Callback handler pertaining to this GPIO interrupt
*/
void hal_gpioConfigureInterrupt(UInt8 gpio, Bool initExpVal, hal_cbGpioExti_t cbExti)
{

    UInt8 gpioExtiMask, gpioToExtiInd, gpioExtiPort;

    UInt16 gpioExtiMapPos = hal_gpioGetExtiFromGpio(gpio);

    gpioToExtiInd = (UInt8)gpioExtiMapPos;
    gpioExtiPort = (UInt8) (gpioExtiMapPos >> 8);

    HAL_DISABLE_GLOBAL_INT();

    gpioExtiMask = GP_WB_READ_INT_CTRL_MASK_GPIO_INTERRUPTS();

    if (gpioExtiMask & BM(gpioToExtiInd))
    {
        //GP_LOG_PRINTF("GPIO EXTI mask is already set, cannot be set again");
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
        BIT_SET(gpioExtiMask, gpioToExtiInd);
        if(NULL == hal_gpioExtiMap[gpioToExtiInd].cbGpioIsr)
        {
            hal_gpioExtiMap[gpioToExtiInd].gpioPort = gpioExtiMapPos;
            hal_gpioExtiMap[gpioToExtiInd].cbGpioIsr = cbExti;
        }
        else
        {
            //GP_LOG_PRINTF("GPIO EXTI already enabled for this GPIO");
            GP_ASSERT_SYSTEM(false);
        }
    }

    /* Set EXTI port, setting individually as the port select is a 3-bit property field in a 32-bit register width.
     * Using register properties keeps the code unchanged if the width gets changed in new family of chip
     */
    if      (gpioToExtiInd == 0) GP_WB_WRITE_GPIO_EXTI0_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 1) GP_WB_WRITE_GPIO_EXTI1_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 2) GP_WB_WRITE_GPIO_EXTI2_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 3) GP_WB_WRITE_GPIO_EXTI3_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 4) GP_WB_WRITE_GPIO_EXTI4_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 5) GP_WB_WRITE_GPIO_EXTI5_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 6) GP_WB_WRITE_GPIO_EXTI6_PORT_SEL(gpioExtiPort);
    else if (gpioToExtiInd == 7) GP_WB_WRITE_GPIO_EXTI7_PORT_SEL(gpioExtiPort);
    else    { GP_ASSERT_DEV_EXT(false); }

    hal_gpioSetExpValue(gpio, initExpVal);

    /* clear a possible pending interrupt condition from before the value was changed */
    GP_WB_WRITE_GPIO_CLR_EXTI_INTERRUPTS(BM(gpioToExtiInd));

    GP_WB_WRITE_INT_CTRL_MASK_GPIO_INTERRUPTS(gpioExtiMask);

    HAL_ENABLE_GLOBAL_INT();

    /* Enable GPIO interrupt */
    hal_gpioEnGlobalInt(true);

}

/** @brief Unconfigure GPIO interrupt
*
*   @param gpio          GPIO number ( see datasheet )
*/
void hal_gpioUnconfigureInterrupt(UInt8 gpio)
{
    UInt8 gpioExtiMask, gpioToExtiInd;

    UInt16 gpioExtiMapPos = hal_gpioGetExtiFromGpio(gpio);

    gpioToExtiInd = (UInt8)gpioExtiMapPos;

    HAL_DISABLE_GLOBAL_INT();

    gpioExtiMask = GP_WB_READ_INT_CTRL_MASK_GPIO_INTERRUPTS();

    if (gpioExtiMask & BM(gpioToExtiInd))
    {
        BIT_CLR(gpioExtiMask, gpioToExtiInd);
        if(NULL != hal_gpioExtiMap[gpioToExtiInd].cbGpioIsr)
        {
            hal_gpioExtiMap[gpioToExtiInd].gpioPort = 0xFFFF;
            hal_gpioExtiMap[gpioToExtiInd].cbGpioIsr = NULL;
        }
        else
        {
            //GP_LOG_PRINTF("GPIO EXTI callback is not configured");
            GP_ASSERT_SYSTEM(false);
        }
    }
    else
    {
        //GP_LOG_PRINTF("GPIO EXTI is not configured");
        GP_ASSERT_SYSTEM(false);
    }

    /* Set EXTI port, setting individually as the port select is a 3-bit property field in a 32-bit register width.
     * Using register properties keeps the code unchanged if the width gets changed in new family of chip
     * There is no invalid port, setting default mapping PORT B
     */
    if      (gpioToExtiInd == 0) GP_WB_WRITE_GPIO_EXTI0_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 1) GP_WB_WRITE_GPIO_EXTI1_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 2) GP_WB_WRITE_GPIO_EXTI2_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 3) GP_WB_WRITE_GPIO_EXTI3_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 4) GP_WB_WRITE_GPIO_EXTI4_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 5) GP_WB_WRITE_GPIO_EXTI5_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 6) GP_WB_WRITE_GPIO_EXTI6_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else if (gpioToExtiInd == 7) GP_WB_WRITE_GPIO_EXTI7_PORT_SEL(GP_WB_ENUM_GPIO_PORT_SEL_PORTB);
    else    { GP_ASSERT_DEV_EXT(false); }

    /* Set EXTI mask */
    GP_WB_WRITE_INT_CTRL_MASK_GPIO_INTERRUPTS(gpioExtiMask);

    HAL_ENABLE_GLOBAL_INT();
}

/**
 * Initialize GPIO HAL interrupt
 */
void hal_gpioInit(void)
{
    for (UIntLoop i = 0; i < HAL_GPIO_MAX_INTERRUPT_SOURCES; i++)
    {
        hal_gpioExtiMap[i].gpioPort = 0xFFFF;
        hal_gpioExtiMap[i].cbGpioIsr = NULL;
    }

    GP_WB_WRITE_GPIO_GPIO_CLOCK_ENABLE(1);
    hal_gpioEnGlobalInt(false);
}

void gpio_handler_impl(void)
{
    UInt8 gpioExtiInt;

    hal_gpioEnGlobalInt(false);

    /* Poll all EXTI interrupt and call registered callback functions, fixed priority EXTI0 = highest, EXTI7 = lowest */
    gpioExtiInt = GP_WB_READ_INT_CTRL_MASKED_GPIO_INTERRUPTS();

    for (UIntLoop i = 0 ; i < HAL_GPIO_MAX_INTERRUPT_SOURCES; i++)
    {
        if(BIT_TST(gpioExtiInt, i) && hal_gpioExtiMap[i].cbGpioIsr != NULL)
        {
            /* call ISR handler */
            hal_gpioExtiMap[i].cbGpioIsr();
        }
    }

    /* Clear interrupts */
    GP_WB_WRITE_GPIO_CLR_EXTI_INTERRUPTS(gpioExtiInt);

    hal_gpioEnGlobalInt(true);
}
#endif //HAL_DIVERSITY_GPIO_INTERRUPT
