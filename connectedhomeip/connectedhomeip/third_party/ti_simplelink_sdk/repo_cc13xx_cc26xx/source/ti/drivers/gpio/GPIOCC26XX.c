/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_ioc.h)
#include DeviceFamily_constructPath(inc/hw_gpio.h)
#include DeviceFamily_constructPath(inc/hw_aon_event.h)
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/gpio.h)

static bool initCalled = false;

// HW interrupt structure for I/O interrupt handler
static HwiP_Struct gpioHwi;

// Link to config values defined by sysconfig
extern GPIO_Config GPIO_config;
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/*
 *  ======== GPIO_clearInt ========
 */
void GPIO_clearInt(uint_least8_t index)
{
    GPIO_clearEventDio(index);
}

/*
 *  ======== GPIO_disableInt ========
 */
void GPIO_disableInt(uint_least8_t index)
{
    /* Interrupt enable is bit 18. Here we mask 0x4 out of byte 2 to disable
     * interrupts. Note we cannot just read-write the whole register.
     * See the IOCFG comment in setConfig().
     */
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index) + 2;
    HWREGB(iocfgRegAddr) = HWREGB(iocfgRegAddr) & ~0x4;
}

/*
 *  ======== GPIO_enableInt ========
 */
void GPIO_enableInt(uint_least8_t index)
{
    /* Interrupt enable is bit 18. Here we or 0x4 into byte 2 to enable
     * interrupts. Note we cannot just read-write the whole register.
     * See the IOCFG comment in setConfig().
     */
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index) + 2;
    HWREGB(iocfgRegAddr) = HWREGB(iocfgRegAddr) | 0x4;
}

/* ======== PIN_hwi_bypass ========
 * Compatibility for PIN and GPIO coexistence
 */
/* Deliberate no-op; if the application does not include the PIN driver,
 * this method will be executed instead. This avoids pulling in PIN
 * unconditionally.
 */
__attribute__((weak)) void PIN_hwi_bypass(uint32_t eventMask);

/*
 *  ======== GPIO_hwiIntFxn ========
 *  Hwi function that processes GPIO interrupts.
 */
void GPIO_hwiIntFxn(uintptr_t arg)
{
    uint32_t flagIndex;
    uint32_t eventMask;

    /* Keep track of unhandled interrupts to forward to PIN
     * This will be removed along with PIN in 2Q22's SDK
     */
    uint32_t pinEventMask = 0;

    // Get and clear the interrupt mask
    eventMask = HWREG(GPIO_BASE + GPIO_O_EVFLAGS31_0);
    HWREG(GPIO_BASE + GPIO_O_EVFLAGS31_0) = eventMask;

    while (eventMask)
    {
        // MASK_TO_PIN only detects the highest set bit
        flagIndex = GPIO_MASK_TO_PIN(eventMask);

        // So it's safe to use PIN_TO_MASK to clear that bit
        eventMask &= ~GPIO_PIN_TO_MASK(flagIndex);

        if (GPIO_config.callbacks[flagIndex] != NULL)
        {
            GPIO_config.callbacks[flagIndex](flagIndex);
        }
        else
        {
            pinEventMask |= GPIO_PIN_TO_MASK(flagIndex);
        }
    }

    /* Forward unhandled interrupts to the PIN driver, if included
     * If not included, just calls the empty function above
     * This will be removed along with PIN in 2Q22's SDK
     */
    if (pinEventMask)
    {
        PIN_hwi_bypass(pinEventMask);
    }

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    uint32_t eventMaskUpper = HWREG(GPIO_BASE + GPIO_O_EVFLAGS47_32);
    HWREG(GPIO_BASE + GPIO_O_EVFLAGS47_32) = eventMaskUpper;

    while (eventMaskUpper)
    {
        // MASK_TO_PIN only detects the highest set bit
        flagIndex = GPIO_MASK_TO_PIN(eventMaskUpper);

        // So it's safe to use PIN_TO_MASK to clear that bit
        eventMaskUpper &= ~GPIO_PIN_TO_MASK(flagIndex);

        if (GPIO_config.callbacks[flagIndex + 32] != NULL)
        {
            GPIO_config.callbacks[flagIndex + 32](flagIndex + 32);
        }
    }
#endif
}

/*
 *  ======== GPIO_init ========
 */
void GPIO_init()
{
    uintptr_t key;
    unsigned int i;
    HwiP_Params hwiParams;
    uint32_t enableMask = 0x0;

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    uint32_t tempPinConfigs[64];
#else
    uint32_t tempPinConfigs[32];
#endif

    key = HwiP_disable();

    if (initCalled)
    {
        HwiP_restore(key);
        return;
    }
    initCalled = true;
    HwiP_restore(key);

    // This is safe even if Power_init has already been called.
    Power_init();

    // Set Power dependecies & constraints
    Power_setDependency(PowerCC26XX_PERIPH_GPIO);

    // Setup HWI handler
    HwiP_Params_init(&hwiParams);
    hwiParams.priority = ~0;
    HwiP_construct(&gpioHwi, INT_AON_GPIO_EDGE, GPIO_hwiIntFxn, &hwiParams);

    // Note: pinUpperBound is inclusive, so we use <= instead of just <
    for (i = GPIO_pinLowerBound; i <= GPIO_pinUpperBound; i++)
    {
        uint32_t pinConfig = GPIO_config.configs[i];

        /* Need to mask off mux byte, since it contains special configs */
        tempPinConfigs[i] = pinConfig & 0xFFFFFF00;

        if (!(pinConfig & GPIOCC26XX_CFG_PIN_IS_INPUT_INTERNAL))
        {
            enableMask |= 1 << i;
            GPIO_write(i, pinConfig & GPIO_CFG_OUT_HIGH ? 1 : 0);
        }
    }

    HWREG(GPIO_BASE + GPIO_O_DOE31_0) = enableMask;

    /* Apply all the masked values directly to IOC
     * pinUpperBound is inclusive, so we need to add 1 to get the full range
     * Multiply by 4 because each pin config and IOC register is 4 bytes wide
     */
    memcpy((void*) (IOC_BASE + IOC_O_IOCFG0 + (4 * GPIO_pinLowerBound)),
           (void*) &tempPinConfigs[GPIO_pinLowerBound],
           ((GPIO_pinUpperBound + 1) - GPIO_pinLowerBound) * 4);

    // Setup wakeup source to wake up from standby (use MCU_WU1)
    HWREG(AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL) = (HWREG(AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL) &
                                                   (~AON_EVENT_MCUWUSEL_WU1_EV_M)) |
                                                   AON_EVENT_MCUWUSEL_WU1_EV_PAD;
}

/*
 *  ======== GPIO_read ========
 */
uint_fast8_t GPIO_read(uint_least8_t index)
{
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    if (index > 32)
    {
        return HWREG(GPIO_BASE + GPIO_O_DIN47_32) &
               GPIO_PIN_TO_MASK(index - 32)
               ? 1 : 0;
    }
#endif
    return HWREG(GPIO_BASE + GPIO_O_DIN31_0)
           & GPIO_PIN_TO_MASK(index)
           ? 1 : 0;
}

/*
 *  ======== GPIO_setConfig ========
 */
int_fast16_t GPIO_setConfig(uint_least8_t index, GPIO_PinConfig pinConfig)
{
    uintptr_t key;
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index);
    uint32_t previousConfig = HWREG(iocfgRegAddr);

    /* Note: Do not change this to check PIN_IS_OUTPUT, because that is 0x0 */
    uint32_t pinWillBeOutput = !(pinConfig & GPIOCC26XX_CFG_PIN_IS_INPUT_INTERNAL);

    /* Special configurations are stored in the lowest 8 bits and need to be removed
     * We can make choices based on these values, but must not write them to hardware */
    GPIO_PinConfig tmpConfig = pinConfig & 0xFFFFFF00;

    if ((previousConfig & 0xFF) != GPIO_MUX_GPIO)
    {
        /* If we're updating mux as well, we can write the whole register */
        HWREG(iocfgRegAddr) = tmpConfig | GPIO_MUX_GPIO;
    }
    else
    {
        /*
        * Writes to the first byte of the IOCFG register will cause a glitch
        * on the internal IO line. To avoid this, we only want to write
        * the upper 24-bits of the IOCFG register when updating the configuration
        * bits. We do this 1 byte at a time.
        */
        key = HwiP_disable();
        HWREGB(iocfgRegAddr + 1) = (uint8_t) (tmpConfig >> 8);
        HWREGB(iocfgRegAddr + 2) = (uint8_t) (tmpConfig >> 16);
        HWREGB(iocfgRegAddr + 3) = (uint8_t) (tmpConfig >> 24);
        HwiP_restore(key);
    }

    /* If this pin is being configured to an output, set the new output value
     * It's important to do this before we change from INPUT to OUTPUT if
     * applicable. If we're already an output this is fine, and if we're input
     * changing to input this statement will not execute. */
    if (pinWillBeOutput)
    {
        GPIO_write(index, pinConfig & GPIO_CFG_OUT_HIGH ? 1 : 0);
    }

    key = HwiP_disable();
    GPIO_setOutputEnableDio(index, pinWillBeOutput ? GPIO_OUTPUT_ENABLE : GPIO_OUTPUT_DISABLE);
    HwiP_restore(key);

    return GPIO_STATUS_SUCCESS;
}

/*
 *  ======== GPIO_setInterruptConfig ========
 */
void GPIO_setInterruptConfig(uint_least8_t index, GPIO_PinConfig config)
{
    uintptr_t key;
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index) + 2;

    /* Shift down and mask away all non-interrupt configuration */
    uint8_t maskedConfig = (config >> 16) & 0x7;

    /* Mask out current interrupt config and apply the new one */
    key = HwiP_disable();
    uint8_t currentRegisterConfig = HWREGB(iocfgRegAddr) & 0xF8;
    HWREGB(iocfgRegAddr) = currentRegisterConfig | maskedConfig;
    HwiP_restore(key);
}

/*
 *  ======== GPIO_getConfig ========
 */
void GPIO_getConfig(uint_least8_t index, GPIO_PinConfig *pinConfig)
{
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index);
    *pinConfig = HWREG(iocfgRegAddr);
}

/*
 *  ======== GPIO_setMux ========
 */
void GPIO_setMux(uint_least8_t index, uint32_t mux)
{
    uintptr_t key;
    uint32_t iocfgRegAddr = IOC_BASE + IOC_O_IOCFG0 + (4 * index);
    uint32_t previousConfig = HWREG(iocfgRegAddr);

    key = HwiP_disable();
    if ((previousConfig & 0xFF) != mux)
    {
        HWREGB(iocfgRegAddr) = (uint8_t) (mux);
    }
    HwiP_restore(key);
}

/*
 *  ======== GPIO_toggle ========
 */
void GPIO_toggle(uint_least8_t index)
{
    GPIO_toggleDio(index);
}

/*
 *  ======== GPIO_write ========
 */
void GPIO_write(uint_least8_t index, unsigned int value)
{
    HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + index) = (value & 0x1);
}
