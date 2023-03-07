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
#include <stdbool.h>
#if defined(__IAR_SYSTEMS_ICC__)
#include <intrinsics.h>
#endif

/*
 * By default disable both asserts and log for this module.
 * This must be done before DebugP.h is included.
 */
#ifndef DebugP_ASSERT_ENABLED
#define DebugP_ASSERT_ENABLED 0
#endif
#ifndef DebugP_LOG_ENABLED
#define DebugP_LOG_ENABLED 0
#endif

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC32XX.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC32XX.h>

/* driverlib header files */
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_gpio.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/gpio.h>
#include <ti/devices/cc32xx/driverlib/pin.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>

/* Mask values for defines stored in GPIO_PinConfig
 * Used to strip down values when passing into driverlib */
#define GPIO_PIN_STRENGTH_M     0x060
#define GPIO_PIN_TYPE_M         0x210
#define GPIO_INTERRUPT_M        0x007

/* Table of pin numbers (physical device pins) for use with PinTypeGPIO()
 * driverlib call. Indexed by GPIO number (0-31).
 */
#define PIN_XX  0xFF
static const uint8_t pinTable[] = {
    /* 00     01      02      03      04      05      06      07  */
    PIN_50, PIN_55, PIN_57, PIN_58, PIN_59, PIN_60, PIN_61, PIN_62,
    /* 08     09      10      11      12      13      14      15  */
    PIN_63, PIN_64, PIN_01, PIN_02, PIN_03, PIN_04, PIN_05, PIN_06,
    /* 16     17      18      19      20      21      22      23  */
    PIN_07, PIN_08, PIN_XX, PIN_XX, PIN_XX, PIN_XX, PIN_15, PIN_16,
    /* 24     25      26      27      28      29      30      31  */
    PIN_17, PIN_21, PIN_29, PIN_30, PIN_18, PIN_20, PIN_53, PIN_45,
    /* 32 */
    PIN_52
};

#define NUM_PORTS            5
#define NUM_PORTS_MASK       0x7
#define NUM_PINS_PER_PORT    8

/* Defines used by PinConfigSet() to set GPIO pin in tristate mode */
#define GPIOCC32XX_TRISTATE         PIN_TYPE_ANALOG
#define GPIOCC32XX_DUMMY_STRENGTH   0x0

/* Returns the GPIO port number or config struct */
#define indexToPortId(pinIndex)     (pinIndex / NUM_PINS_PER_PORT)
/* The mask is here to keep static analysis happy; it counts as a range check for out-of-bounds access */
#define indexToPortConfig(pinIndex) (portConfigs[indexToPortId(pinIndex) & NUM_PORTS_MASK])

/* Generates a per-port mask for a pin, e.g. the second pin in a port should be 0x10 */
#define indexToPortMask(pinIndex) (1 << (pinIndex % NUM_PINS_PER_PORT))

/* Uninitialized callbackInfo pinIndex */
#define CALLBACK_INDEX_NOT_CONFIGURED 0xFF

/* User defined pin indexes assigned to a port's 8 pins.
 * Used by port interrupt function to locate callback assigned
 * to a pin.
 */
typedef struct {
    const uint8_t interruptNum;
    const uint8_t powerResource;
    const uint32_t baseAddress;
} PortConfig;

/* Port information, including dynamic callback data. */
static const PortConfig portConfigs[NUM_PORTS] = {
    { INT_GPIOA0, PowerCC32XX_PERIPH_GPIOA0, GPIOA0_BASE },
    { INT_GPIOA1, PowerCC32XX_PERIPH_GPIOA1, GPIOA1_BASE },
    { INT_GPIOA2, PowerCC32XX_PERIPH_GPIOA2, GPIOA2_BASE },
    { INT_GPIOA3, PowerCC32XX_PERIPH_GPIOA3, GPIOA3_BASE },
    /* There is no listed interrupt value for Port 4 (which contains only one pin) */
    { 0xFF,       PowerCC32XX_PERIPH_GPIOA4, GPIOA4_BASE },
};

/* Bit mask used to determine if a Hwi has been created/constructed
 * for a port already.
 */
static uint8_t portHwiCreatedBitMask = 0;

/* Internal boolean to confirm that GPIO_init() has been called. */
static bool initCalled = false;
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* Notification for going into and waking up from LPDS */
static Power_NotifyObj powerNotifyObj;

__attribute__((weak))extern const GPIO_Config GPIO_config;

static int powerNotifyFxn(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg);

/*
 *  ======== GPIO_clearInt ========
 */
void GPIO_clearInt(uint_least8_t index)
{
    DebugP_assert(initCalled && index < GPIO_config.numberOfPinConfigs);

    /* Clear GPIO interrupt flag */
    MAP_GPIOIntClear(indexToPortConfig(index).baseAddress, indexToPortMask(index));

    DebugP_log2(
        "GPIO: port 0x%x, pin 0x%x interrupt flag cleared",
        indexToPortConfig(index).baseAddress,
        indexToPortMask(index)
    );
}

/*
 *  ======== GPIO_disableInt ========
 */
void GPIO_disableInt(uint_least8_t index)
{
    DebugP_assert(initCalled && index <= GPIO_pinUpperBound);

    uintptr_t key = HwiP_disable();
    GPIO_config.configs[index] &= ~GPIO_CFG_INT_ENABLE;
    MAP_GPIOIntDisable(indexToPortConfig(index).baseAddress, indexToPortMask(index));

    HwiP_restore(key);

    DebugP_log2(
        "GPIO: port 0x%x, pin 0x%x interrupts disabled",
        indexToPortConfig(index).baseAddress,
        indexToPortMask(index)
    );
}

/*
 *  ======== GPIO_enableInt ========
 */
void GPIO_enableInt(uint_least8_t index)
{
    DebugP_assert(initCalled && index <= GPIO_pinUpperBound);
    /* Pin 33 doesn't support interrupts, pins 26/27 are output only */
    DebugP_assert(index != 26 && index != 27 && index != 33);

    uintptr_t key = HwiP_disable();
    GPIO_config.configs[index] |= GPIO_CFG_INT_ENABLE;
    MAP_GPIOIntEnable(indexToPortConfig(index).baseAddress, indexToPortMask(index));
    HwiP_restore(key);

    DebugP_log2(
        "GPIO: port 0x%x, pin 0x%x interrupts enabled",
        indexToPortConfig(index).baseAddress,
        indexToPortMask(index)
    );
}

/*
 *  ======== GPIO_getConfig ========
 */
void GPIO_getConfig(uint_least8_t index, GPIO_PinConfig *pinConfig)
{
    *pinConfig = GPIO_config.configs[index];
}

/*
 *  ======== GPIO_hwiIntFxn ========
 *  Hwi function that processes GPIO interrupts.
 */
void GPIO_hwiIntFxn(uintptr_t portIndex)
{
    uint32_t pinIndex;
    uint32_t flagIndex;
    uint32_t eventMask;
    uint32_t portBase = portConfigs[portIndex].baseAddress;

    /* Find out which pins have their interrupt flags set */
    eventMask = MAP_GPIOIntStatus(portBase, 0xFF) & 0xFF;

    /* clear all the set bits at once */
    MAP_GPIOIntClear(portBase, eventMask);

    /* Match the interrupt to its corresponding callback function */
    while (eventMask) {
        /* Note MASK_TO_PIN only detects the highest set bit */
        flagIndex = GPIO_MASK_TO_PIN(eventMask);
        eventMask &= ~GPIO_PIN_TO_MASK(flagIndex);

        /* Need to go from port index up to global index */
        pinIndex = flagIndex + (portIndex * NUM_PINS_PER_PORT);

        if (GPIO_config.callbacks[pinIndex] != NULL)
        {
            GPIO_config.callbacks[pinIndex](pinIndex);
        }
    }
}

/*
 *  ======== GPIO_init ========
 */
void GPIO_init(void)
{
    uint32_t i;
    uintptr_t hwiKey;
    SemaphoreP_Handle sem;
    static SemaphoreP_Handle initSem;

    /* speculatively create a binary semaphore */
    sem = SemaphoreP_createBinary(1);

    /* There is no way to inform user of this fatal error. */
    if (sem == NULL) return;

    hwiKey = HwiP_disable();

    if (initSem == NULL) {
        initSem = sem;
        HwiP_restore(hwiKey);
    }
    else {
        /* init already called */
        HwiP_restore(hwiKey);
        /* delete unused Semaphore */
        if (sem) SemaphoreP_delete(sem);
    }

    /* now use the semaphore to protect init code */
    SemaphoreP_pend(initSem, SemaphoreP_WAIT_FOREVER);

    /* Only perform init once */
    if (initCalled) {
        SemaphoreP_post(initSem);
        return;
    }

    /* Configure pins and create Hwis per static array content
     * Note that initial callbacks are configured directly in Board_init()
     */
    for (i = 0; i <= GPIO_pinUpperBound; i++)
    {
        GPIO_setConfig(i, GPIO_config.configs[i]);
    }

    Power_registerNotify(
        &powerNotifyObj,
        PowerCC32XX_ENTERING_LPDS | PowerCC32XX_AWAKE_LPDS,
        powerNotifyFxn,
        (uintptr_t) NULL
    );

    initCalled = true;
    SemaphoreP_post(initSem);
}

/*
 *  ======== GPIO_read ========
 */
uint_fast8_t GPIO_read(uint_least8_t index)
{
    uint32_t value, pinMask;

    DebugP_assert(initCalled && index <= GPIO_pinUpperBound);
    DebugP_assert(index != 26 && index != 27);

    pinMask = indexToPortMask(index);
    value = MAP_GPIOPinRead(indexToPortConfig(index).baseAddress, pinMask);

    DebugP_log3(
        "GPIO: port 0x%x, pin 0x%x read 0x%x",
        getPort(config->port),
        config->pin,
        value
    );

    return value ? 1 : 0;
}

/*
 *  ======== GPIO_setConfig ========
 */
int_fast16_t GPIO_setConfig(uint_least8_t index, GPIO_PinConfig pinConfig)
{
    uintptr_t      key;
    uint32_t       pinMask;
    uint32_t       pin;
    uint32_t       portBase;
    uint32_t       portIndex;
    uint32_t       portBitMask;
    uint16_t       strength;
    uint16_t       pinType;
    HwiP_Handle    hwiHandle;
    HwiP_Params    hwiParams;

    DebugP_assert(initCalled && index <= GPIO_pinUpperBound);
    /* Pin 26 and pin 27 can only be configured as outputs */
    DebugP_assert((pinConfig & PIN_DIR_MODE_OUT) || (index != 26 && index != 27));

    if (pinConfig & GPIOCC32XX_DO_NOT_CONFIG) {
        return GPIO_STATUS_SUCCESS;
    }

    /* These pins are not configurable on this device. It's easier having
     * the logic once here than in multiple places like init and power wakeup
     */
    if (index >= 18 && index <= 21) {
        return GPIO_STATUS_ERROR;
    }

    key = HwiP_disable();

    pin = pinTable[index];
    pinMask = indexToPortMask(index);
    portBase = indexToPortConfig(index).baseAddress;
    strength = pinConfig & GPIO_PIN_STRENGTH_M;
    pinType = pinConfig & GPIO_PIN_TYPE_M;

    /* The default pin strength is nonzero, so add a catch for failing to set it */
    if (!strength) {
        strength = PIN_STRENGTH_2MA;
    }

    /* Configure GPIO pin as tristate and enable clocking for the port
     * Tristate is useful because none of our configuration values will take
     * effect until we disable this and set it back to normal GPIO mode.
     */
    MAP_PinConfigSet(pin, GPIOCC32XX_DUMMY_STRENGTH, GPIOCC32XX_TRISTATE);
    Power_setDependency(indexToPortConfig(index).powerResource);

    HwiP_restore(key);

    if ((pinConfig & GPIO_INTERRUPT_M) != GPIO_CFG_INT_NONE_INTERNAL) {
        portIndex = indexToPortId(index);
        portBitMask = 1 << portIndex;

        /* if Hwi has not already been created, do so */
        key = HwiP_disable();
        if ((portHwiCreatedBitMask & portBitMask) == 0) {
            portHwiCreatedBitMask |= portBitMask;
            HwiP_restore(key);

            HwiP_Params_init(&hwiParams);
            hwiParams.arg      = (uintptr_t)portIndex;
            hwiParams.priority = GPIO_config.intPriority;

            hwiHandle = HwiP_create(indexToPortConfig(index).interruptNum, GPIO_hwiIntFxn, &hwiParams);

            if (hwiHandle == NULL) {
                /* Error creating Hwi */
                DebugP_log1("GPIO: Error constructing Hwi for GPIO Port %d", getPort(pinConfig->port));
                return (GPIO_STATUS_ERROR);
            }
        }
        else {
            HwiP_restore(key);
        }
    }

    key = HwiP_disable();

    /* Configure the GPIO pin */
    if ((pinConfig & PIN_DIR_MODE_IN) == PIN_DIR_MODE_IN) {
        MAP_GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_IN);
    }
    else {
        MAP_GPIODirModeSet(portBase, pinMask, GPIO_DIR_MODE_OUT);

        /* Set output value */
        if (pinConfig & GPIO_CFG_OUT_HIGH) {
            MAP_GPIOPinWrite(portBase, pinMask, pinMask);
        } else {
            MAP_GPIOPinWrite(portBase, pinMask, 0x0);
        }
    }

    /* Configure pin output settings */
    MAP_PinConfigSet(pin, strength, pinType);

    /* Set the pin's pinType to GPIO and remove initial tristate setting */
    MAP_PinModeSet(pin, PIN_MODE_0);

    GPIO_setInterruptConfig(index, pinConfig);

    /* Update pinConfigs with the latest GPIO configuration and
     * clear the GPIO_DO_NOT_CONFIG bit if it was set.
     */
    GPIO_config.configs[index] = pinConfig;

    HwiP_restore(key);

    if (pinConfig & GPIO_CFG_INT_ENABLE)
    {
        GPIO_enableInt(index);
    }

    return GPIO_STATUS_SUCCESS;
}

/*
 *  ======== GPIO_setInterruptConfig ========
 */
void GPIO_setInterruptConfig(uint_least8_t index, GPIO_PinConfig config)
{
    uint32_t pinMask = indexToPortMask(index);
    uint32_t portBase = indexToPortConfig(index).baseAddress;

    if ((config & GPIO_INTERRUPT_M) != GPIO_CFG_IN_INT_NONE)
    {
        /* The interrupt values are all increased by 1, see the header file */
        uint32_t interruptType = (config & GPIO_INTERRUPT_M) - 1;
        /* Set type of interrupt and then clear it */
        MAP_GPIOIntTypeSet(portBase, pinMask, interruptType);

        /* Note there is no interrupt type for NONE, so no else block */
    }

    /* There is extra validation logic in enable and disable int for CC32XX, so
     * forward this request to them despite the performance hit
     */
    if (config & GPIO_CFG_INT_ENABLE)
    {
        GPIO_enableInt(index);
    }
    else
    {
        GPIO_disableInt(index);
    }

    MAP_GPIOIntClear(portBase, pinMask);
}

/*
 *  ======== GPIO_toggle ========
 */
void GPIO_toggle(uint_least8_t index)
{
    uintptr_t key;
    uint32_t  value, portBase, portPinMask;

    DebugP_assert(initCalled && index < GPIO_config.numberOfPinConfigs);
    DebugP_assert((GPIO_config.configs[index] & GPIO_CFG_INPUT) == GPIO_CFG_OUTPUT);

    portBase = indexToPortConfig(index).baseAddress;
    portPinMask = indexToPortMask(index);

    /* Make atomic update */
    key = HwiP_disable();

    value = MAP_GPIOPinRead(portBase, portPinMask);
    value ^= portPinMask;
    MAP_GPIOPinWrite(portBase, portPinMask, value);

    /* Update config table entry with value written */
    GPIO_config.configs[index] ^= GPIO_CFG_OUT_HIGH;

    HwiP_restore(key);

    DebugP_log2(
        "GPIO: port 0x%x, pin 0x%x toggled",
        indexToPortConfig(index),
        indexToPortMask(index)
    );
}

/*
 *  ======== GPIO_write ========
 */
void GPIO_write(uint_least8_t index, unsigned int value)
{
    uintptr_t key;
    uint32_t  portBase, portPinMask;

    DebugP_assert(initCalled && index <= GPIO_pinUpperBound);
    DebugP_assert((GPIO_config.configs[index] & GPIO_CFG_INPUT) == GPIO_CFG_OUTPUT);

    portBase = indexToPortConfig(index).baseAddress;
    portPinMask = indexToPortMask(index);

    key = HwiP_disable();

    if (value && !(GPIO_config.configs[index] & GPIO_CFG_OUT_HIGH))
    {
        GPIO_config.configs[index] |= GPIO_CFG_OUT_HIGH;
        MAP_GPIOPinWrite(portBase, portPinMask, portPinMask);
    }
    else if (!value && (GPIO_config.configs[index] & GPIO_CFG_OUT_HIGH))
    {
        GPIO_config.configs[index] &= ~GPIO_CFG_OUT_HIGH;
        MAP_GPIOPinWrite(portBase, portPinMask, 0);
    }

    HwiP_restore(key);

    DebugP_log3(
        "GPIO: port 0x%x, pin 0x%x wrote 0x%x",
        portBase,
        portPinMask,
        value
    );
}

/*
 *  ======== powerNotifyFxn ========
 */
static int powerNotifyFxn(unsigned int eventType, uintptr_t eventArg, uintptr_t clientArg)
{
    uint32_t                i;
    GPIO_PinConfig          config;

    if (eventType == PowerCC32XX_AWAKE_LPDS) {
        for (i = 0; i <= GPIO_pinUpperBound; i++) {
            if (!(GPIO_config.configs[i] & GPIOCC32XX_DO_NOT_CONFIG)) {
                /* setConfig takes care of enabling interrupts for us */
                GPIO_setConfig(i, GPIO_config.configs[i]);
            }
        }
    }
    else {
        /* Entering LPDS
         * For pins configured as GPIO output, if the GPIOCC32XX_CFG_USE_STATIC
         * configuration flag is *not* set, get the current pin state, and
         * then call to the Power manager to define the state to be held
         * during LPDS.
         * If GPIOCC32XX_CFG_USE_STATIC *is* defined, do nothing, and the pin
         * will be parked in the state statically defined in
         * PowerCC32XX_config.pinParkDefs[] in the board file.
         */
        for (i = 0; i < GPIO_pinUpperBound; i++) {
            config =  GPIO_config.configs[i];

            /* Only configure configured OUTPUT pins */
            if (!(config & GPIOCC32XX_DO_NOT_CONFIG) && (config & PIN_DIR_MODE_IN) != PIN_DIR_MODE_IN)
            {
                if (!(config & GPIOCC32XX_CFG_USE_STATIC))
                {
                    /* Set the new park state */
                    PowerCC32XX_setParkState(
                        (PowerCC32XX_Pin)pinTable[i],
                        config & GPIO_CFG_OUT_HIGH
                    );
                }
            }
        }
    }

    return (Power_NOTIFYDONE);
}
