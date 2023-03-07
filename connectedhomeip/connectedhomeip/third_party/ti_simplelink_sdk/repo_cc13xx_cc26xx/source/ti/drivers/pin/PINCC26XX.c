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

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_aon_event.h)
#include DeviceFamily_constructPath(inc/hw_aon_ioc.h)
#include DeviceFamily_constructPath(inc/hw_gpio.h)
#include DeviceFamily_constructPath(inc/hw_ioc.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include DeviceFamily_constructPath(driverlib/chipinfo.h)

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
    #include DeviceFamily_constructPath(inc/hw_aon_sysctl.h)
#elif (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1 || \
       DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
       DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    #include DeviceFamily_constructPath(inc/hw_aon_pmctl.h)
#endif

/*!*****************************************************************************
 *  @file       PINCC26XX.c
 *  @brief      Device-specific PIN & GPIO driver for CC26xx family [impl]
 *
 *  # Overview #
 *  This is the device-specific implementation of the generic PIN driver for the
 *  CC26xx family of devices.
 *
 *******************************************************************************
 */

// Maximum number of pins (# available depends on package configuration)
#define MAX_NUM_PINS 31

// Macro used to return the minimum of two numbers
#ifndef MIN
#  define MIN(n, m)    (((n) > (m)) ? (m) : (n))
#endif

/// Last DIO number available on package + device combination
uint32_t pinUpperBound = 0;

/// First DIO number available on package + device combination
uint32_t pinLowerBound = 0;

/// Array of handles, one per pin (pin id is index)
PIN_Handle pinHandleTable[MAX_NUM_PINS];

/// Pointer to GPIO configuration set by PIN_init(...), save state in order to revert when PIN_close(...)
static const PIN_Config *defaultPinConfig;

/// Array of indexes into GPIO configuration defaultPinConfig, one per pin (pin id is index)
static uint8_t pinGpioConfigTable[MAX_NUM_PINS];

/// HW interrupt structure for I/O interrupt handler
static HwiP_Struct pinHwi;

/// SWI structure for the followup of the I/O interrupt handler
static SwiP_Struct pinSwi;

/// PIN driver semaphore used to implement synchronicity for PIN_open()
static SemaphoreP_Struct pinSemaphore;

/// Hardware attribute structure populated in board.c to set HWI and SWI priorities
extern const PINCC26XX_HWAttrs PINCC26XX_hwAttrs;

// Defaults to true, set to false by the weak GPIO_init();
static bool isGPIOIncluded = true;

// I/O SWI service routine that posts the callback in a SWI context
static void PIN_swi(uintptr_t arg0, uintptr_t arg1){

    uint32_t eventMask;
    unsigned char eventCounter;
    PIN_Handle handle;
    PIN_IntCb  callbackFxn;

    // Get the OR'd trigger value representing all events values prior to running the SWI
    eventMask = SwiP_getTrigger();

    // eventCounter cycles through all pins on the device up to the max number of pins
    for(eventCounter = 0; eventCounter <= pinUpperBound; eventCounter++){
        // Check if current eventCounter bit is set in eventMask
        if(eventMask & (1 << eventCounter)){
            // Get pin handle and registered callback function
            // Double paranthesis to supress GCC warning
            // Intentional assignment is intended, not the equality comparison
            if((handle = pinHandleTable[eventCounter])) {
                if((callbackFxn = handle->callbackFxn)) {
                    // Event from existing pin, with an associated handle and a
                    // registered callback -> call callback
                    // Run the callback function in a SWI context.
                    callbackFxn(handle, eventCounter);
                }
            }
        }
    }
}

// I/O HWI service routine
// This special bypass is used by the GPIO driver, if both drivers are included
void PIN_hwi_bypass(uint32_t eventMask) {
    // Include all GPIO's currently triggered in the SWI
    SwiP_or(&(pinSwi), eventMask);
}

// I/O HWI service routine
static void PIN_hwi(uintptr_t arg) {
    uint32_t eventMask;

    // Get event flag with lowest index (also pin ID)
    eventMask = HWREG(GPIO_BASE + GPIO_O_EVFLAGS31_0);

    // Clear this event flag
    HWREG(GPIO_NONBUF_BASE + GPIO_O_EVFLAGS31_0) = eventMask;

    // Include all GPIO's currently triggered in the SWI
    SwiP_or(&(pinSwi), eventMask);
}

// Internal utility function for setting IOCFG register for pin
static void PINCC26XX_setIoCfg(PIN_Config updateMask, PIN_Config pinCfg) {
    uint32_t tmpConfig;
    PIN_Id pinId = PIN_ID(pinCfg);
    bool invertChanges;
    uint32_t key;

    if (pinCfg & PIN_GEN) {
        // Translate from device-independent to device-specific PIN_Config values
        pinCfg ^= PIN_GEN | PIN_BM_INPUT_EN | PIN_BM_PULLING;
    }
    // Get existing IOCFG, determine whether inversion changes, mask away what will be updated
    tmpConfig = HWREG(IOC_BASE + IOC_O_IOCFG0 + 4 * pinId);
    invertChanges = (tmpConfig ^ pinCfg) & updateMask & PINCC26XX_INV_INOUT;
    tmpConfig &= ~updateMask;

    // Insert what we want to update, possibly revert IRQ edges, write back to IOCFG
    tmpConfig |= (pinCfg & updateMask & PINCC26XX_BM_IOCFG);
    if ((updateMask & PINCC26XX_BM_IRQ) == PINCC26XX_BM_IRQ && (tmpConfig & PINCC26XX_INV_INOUT) == 0) {
        // We're changing IRQ options but inversion will not be enabled -> keep IRQ options
    } else if ((updateMask & PINCC26XX_BM_IRQ) == 0 && !invertChanges) {
        // We're not changing IRQ options and inversion remains unchanged -> keep IRQ options
    } else {
        // We're updating IRQ options and inversion will be enabled, OR
        // we're not updating IRQ options but inversion settings change
        // -> reverse polarity of edge detection when positive-only or negative-only
        switch (tmpConfig & PINCC26XX_BM_IRQ) {
        case PINCC26XX_IRQ_POSEDGE:
            tmpConfig &= ~PINCC26XX_BM_IRQ;
            tmpConfig |= PINCC26XX_IRQ_NEGEDGE;
            break;
        case PINCC26XX_IRQ_NEGEDGE:
            tmpConfig &= ~PINCC26XX_BM_IRQ;
            tmpConfig |= PINCC26XX_IRQ_POSEDGE;
            break;
        default:
            break;
        }
    }

    /* Clear any pending events from the previous pin configuration before we write the new interrupt settings */
    PINCC26XX_clrPendInterrupt(pinId);

    /*
     * Writes to the first byte of the IOCFG register will cause a glitch
     * on the internal IO line. To avoid this, we only want to write
     * the upper 24-bits of the IOCFG register when updating the configuration
     * bits. We do this 1 byte at a time.
     */
    uint32_t iocfgReg = IOC_BASE + IOC_O_IOCFG0 + 4 * pinId;
    key = HwiP_disable();
    HWREGB(iocfgReg + 1) = (uint8_t) (tmpConfig >> 8);
    HWREGB(iocfgReg + 2) = (uint8_t) (tmpConfig >> 16);
    HWREGB(iocfgReg + 3) = (uint8_t) (tmpConfig >> 24);
    HwiP_restore(key);

    // Update GPIO output value and enable depending on previous output mode (enabled or disabled)
    {
        bool outputEnabled = (HWREG(GPIO_BASE + GPIO_O_DOE31_0) & (1 << pinId)) ? true : false;

        if(!outputEnabled) {
            if (updateMask & PINCC26XX_BM_GPIO_OUTPUT_VAL) {
                // Set GPIO output value
                HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + pinId) = (pinCfg & PINCC26XX_BM_GPIO_OUTPUT_VAL) ? 1 : 0;
            }
        }

        if (updateMask & PINCC26XX_BM_GPIO_OUTPUT_EN) {
            // Set GPIO output enable
            key = HwiP_disable();
                HWREG(GPIO_BASE + GPIO_O_DOE31_0) =
                    (HWREG(GPIO_BASE + GPIO_O_DOE31_0) & ~(1 << pinId)) |
                    ((pinCfg&PINCC26XX_BM_GPIO_OUTPUT_EN) ? (1 << pinId) : 0);
            HwiP_restore(key);
        }

        if(outputEnabled) {
            if (updateMask & PINCC26XX_BM_GPIO_OUTPUT_VAL) {
                // Set GPIO output value
                HWREGB(GPIO_BASE + GPIO_O_DOUT3_0 + pinId) = (pinCfg & PINCC26XX_BM_GPIO_OUTPUT_VAL) ? 1 : 0;
            }
        }
    }

    /* Clear any events from pin value changes as a result of the new configuration */
    PINCC26XX_clrPendInterrupt(pinId);
}



// Internal utility function for setting mux setting in IOCFG register for pin
static void PINCC26XX_setIoCfgMux(PIN_Id pinId, int32_t mux) {
    // Read in existing value in IOCFG register and update with supplied mux value
    if (mux < 0) {
        mux = PINCC26XX_MUX_GPIO;
    }
    uint32_t tmpConfig;
    tmpConfig = HWREG(IOC_BASE + IOC_O_IOCFG0 + 4 * pinId);
    tmpConfig &= ~IOC_IOCFG0_PORT_ID_M;
    tmpConfig |= mux & IOC_IOCFG0_PORT_ID_M;
    HWREG(IOC_BASE + IOC_O_IOCFG0 + 4 * pinId) = tmpConfig;
}

uint32_t PINCC26XX_getPinCount(){
    // Get number of pins available on device (from HW register)
    uint32_t pinCount = (( HWREG( FCFG1_BASE + FCFG1_O_IOCONF ) &
                         FCFG1_IOCONF_GPIO_CNT_M ) >>
                       FCFG1_IOCONF_GPIO_CNT_S ) ;

    // Workaround for CC26x4 or other devices that support > 32 pins.
    // Due to driverlib limitations, more than 32 pins cannot be supported
    // To prevent overflowing pinGpioConfigTable, limit the pin count here
    pinCount = MIN(MAX_NUM_PINS, pinCount);

    return pinCount;
}

/* Deliberate no-op. GPIO must be initialised before PIN for correct operation,
 * so we use this weak definition to accomplish this without unconditionally
 * including GPIO in every application
 */
__attribute__((weak)) void GPIO_init()
{
    isGPIOIncluded = false;
}

PIN_Status PIN_init(const PIN_Config pinConfig[]) {
    uint32_t i;
    uint32_t pinConfigMask = 0;            // Works as long as # pins <=32
    HwiP_Params hwiParams;
    SwiP_Params swiParams;
    uint32_t reservedPinMask = 0;

    // Its ok if Power init has already been called.
    Power_init();

    // GPIO_init() must complete first and is safe to call multiple times.
    GPIO_init();

    /* pinLowerBound is initialized to 0 by default.
     * All cases where this is not the case are handled here.
     */
    switch (ChipInfo_GetChipType()) {
        case CHIP_TYPE_CC1310:
        case CHIP_TYPE_CC1350:
        case CHIP_TYPE_CC1312:
        case CHIP_TYPE_CC1312R7:
            if (ChipInfo_GetPackageType() == PACKAGE_7x7) {
                pinLowerBound = 1;
                reservedPinMask |= 0x01;
            }
            break;
        case CHIP_TYPE_CC1352:
        case CHIP_TYPE_CC1352R7:
        case CHIP_TYPE_CC2672R3:
            pinLowerBound = 3;
            reservedPinMask |= 0x07;
            break;
        case CHIP_TYPE_CC1311P3:
        case CHIP_TYPE_CC2651P3:
        case CHIP_TYPE_CC1352P:
        case CHIP_TYPE_CC2652P:
        case CHIP_TYPE_CC1352P7:
        case CHIP_TYPE_CC2652P7:
        case CHIP_TYPE_CC2672P3:
            if (ChipInfo_GetPackageType() == PACKAGE_7x7 ||
                ChipInfo_GetPackageType() == PACKAGE_5x5) {
                pinLowerBound = 5;
                reservedPinMask |= 0x1F;
            }
            else if (ChipInfo_GetPackageType() == PACKAGE_7x7_SIP) {
                /* Do nothing for PACKAGE_7x7_SIP since the CC2652PSIP has
                 * DIO0-DIO31 bonded out.
                 */
            }
            break;
        default:
            /* Lower bound begins at DIO0. No lower bound restrictions needed.
             * CHIP_TYPE_CC1311R3
             * CHIP_TYPE_CC2640
             * CHIP_TYPE_CC2650
             * CHIP_TYPE_CC2640R2
             * CHIP_TYPE_CC2651R3
             * CHIP_TYPE_CC2642
             * CHIP_TYPE_CC2652
             * CHIP_TYPE_CC2652R7
             * CHIP_TYPE_CC2652RF4
             */
            break;
    }

    if ((HWREG(CCFG_BASE + CCFG_O_MODE_CONF) & CCFG_MODE_CONF_SCLK_LF_OPTION_M) >> CCFG_MODE_CONF_SCLK_LF_OPTION_S == 0x1) {
       // An IO is used for LF clock input
      reservedPinMask |= (1 << ((HWREG(CCFG_BASE+CCFG_O_EXT_LF_CLK) & CCFG_EXT_LF_CLK_DIO_M) >> CCFG_EXT_LF_CLK_DIO_S));
    }

    // Get the last addressable DIO number
    pinUpperBound = pinLowerBound + PINCC26XX_getPinCount() - 1;

    // Initialize table of pins that have default GPIO configuration
    for (i = 0; i <= pinUpperBound; i++) {
        pinGpioConfigTable[i] = PIN_UNASSIGNED;
    }


    // Read in pinConfig list and create bitmask of which IOs to initialize to
    // default values and which to initialize from pinConfig
    for (i = 0, pinConfigMask = 0; PIN_ID(pinConfig[i]) != PIN_TERMINATE; i++) {
        // Ignore unassigned pins
        if (PIN_ID(pinConfig[i]) == PIN_UNASSIGNED) {
            continue;
        }
        // Check that pin exists and is available
        if (PIN_ID(pinConfig[i]) > pinUpperBound || PIN_ID(pinConfig[i]) < pinLowerBound || reservedPinMask & (1 << PIN_ID(pinConfig[i]))) {
            return PIN_NO_ACCESS;
        }
        // Mark pin as being in pinConfig
        pinConfigMask |= (1 << PIN_ID(pinConfig[i]));
        // For quick reference, store index i in table
        pinGpioConfigTable[PIN_ID(pinConfig[i])] = i;
    }

    // Set Power dependecies & constraints
    Power_setDependency(PowerCC26XX_PERIPH_GPIO);

    // Save GPIO default setup
    defaultPinConfig = pinConfig;

    // Setup semaphore for sequencing accesses to PIN_open()
    SemaphoreP_constructBinary(&pinSemaphore, 1);

    // Loop thru all pins and configure
    for (i = 0; i <= pinUpperBound; i++) {
        if (reservedPinMask & (1 << i)) {
            // Pin is reserved for other purposes -> setup dummy handle
            pinHandleTable[i] = (PIN_State*)0x00000004;
        }
        else {
            if (pinConfigMask & (1 << i)) {
                // Setup all pins in pinConfig as instructed
                PINCC26XX_setIoCfg(PIN_BM_ALL, pinConfig[pinGpioConfigTable[i]]);
            }
            else if (!isGPIOIncluded) {
                // Setup all pins not in pinConfig to default configuration:
                // GPIO, input buffer disable, GPIO output disable, low GPIO output, no pull, no IRQ, no wakeup
                PINCC26XX_setIoCfg(PIN_BM_ALL, PIN_ID(i) | PINCC26XX_NOPULL);
            }
            // Set pin as GPIO and clear pin handle
            PINCC26XX_setIoCfgMux(PIN_ID(i), -1);
            pinHandleTable[i] = NULL;
        }
    }

    // Setup HWI handler
    HwiP_Params_init(&hwiParams);
    hwiParams.priority = PINCC26XX_hwAttrs.intPriority;

    /* Note: If GPIO is included, we do not need this HwiP. GPIO will already have constructed
     * a HwiP on INT_AON_GPIO_EDGE, and there is explicit callback forwarding logic in GPIO_hwiIntFxn.
     */
    if (!isGPIOIncluded) {
        HwiP_construct(&pinHwi, INT_AON_GPIO_EDGE, PIN_hwi, &hwiParams);
    }

    // Setup SWI handler
    SwiP_Params_init(&(swiParams));
    swiParams.priority = PINCC26XX_hwAttrs.swiPriority;
    swiParams.trigger = 0;
    SwiP_construct(&pinSwi, PIN_swi, &(swiParams));

    // Setup interrupts so that they wake up from standby (use MCU_WU1)
    HWREG(AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL) =
        (HWREG(AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL) & (~AON_EVENT_MCUWUSEL_WU1_EV_M)) |
            AON_EVENT_MCUWUSEL_WU1_EV_PAD;

    // Open latches out to I/Os
    // This might be unnecessary, but isn't when you start from debugger
    HWREG(AON_IOC_BASE + AON_IOC_O_IOCLATCH) = AON_IOC_IOCLATCH_EN;

    // If we boot from shutdown, the IOs are latched, this opens the latches again
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
    HWREG(AON_SYSCTL_BASE + AON_SYSCTL_O_SLEEPCTL) = AON_SYSCTL_SLEEPCTL_IO_PAD_SLEEP_DIS;
#else
    HWREG(AON_PMCTL_BASE + AON_PMCTL_O_SLEEPCTL) = AON_PMCTL_SLEEPCTL_IO_PAD_SLEEP_DIS;
#endif

    return PIN_SUCCESS;
}



PIN_Handle PIN_open(PIN_State* state, const PIN_Config pinList[]) {
    uint32_t i;
    bool pinsAllocated = true;
    PIN_Id pinId;

    if ((state == NULL) || (pinList == NULL)) {
        return (NULL);
    }

    // Ensure that only one client at a time can call PIN_open() or PIN_add()
    SemaphoreP_pend(&pinSemaphore, SemaphoreP_WAIT_FOREVER);

    // Check whether all pins in pinList are valid and available first
    for (i = 0; (pinId = PIN_ID(pinList[i])) != PIN_TERMINATE; i++) {
        /* Unassigned pins is allowed, but cannot generate a bitmask. */
        if (pinId != PIN_UNASSIGNED) {
            if ((pinId > pinUpperBound) ||
                    (pinId >= MAX_NUM_PINS) ||  // For Klocwork
                    pinHandleTable[pinId]) {
                pinsAllocated = false;
                break;
            }
        }
    }

    if (!pinsAllocated) {
        // Indicate that the pins were not allocatable
        state = NULL;
    } else {
        // Setup state object
        state->callbackFxn = NULL;
        state->portMask = 0;
        state->userArg = 0;

        // Configure I/O pins according to pinList
        for (i = 0; (pinId = PIN_ID(pinList[i])) != PIN_TERMINATE; i++) {
            // Check pinId < MAX_NUM_PINS for Klocwork
            if ((pinId != PIN_UNASSIGNED) && (pinId < MAX_NUM_PINS)) {
                pinHandleTable[pinId] = state;
                state->portMask |= (1 << pinId);
                PIN_setConfig(state, PIN_BM_ALL, pinList[i]);
            }
        }
    }

    SemaphoreP_post(&pinSemaphore);
    return state;
}



PIN_Status PIN_add(PIN_Handle handle, PIN_Config pinCfg) {
    PIN_Status returnStatus;
    PIN_Id pinId = PIN_ID(pinCfg);

    // Check that handle and pinId is valid
    if (!handle || (pinId > pinUpperBound) || (pinId >= MAX_NUM_PINS)) {
        return PIN_NO_ACCESS;
    }

    // Ensure that only one client at a time can call PIN_open() or PIN_add()
    SemaphoreP_pend(&pinSemaphore, SemaphoreP_WAIT_FOREVER);

    // Check whether pin is available
    if (pinHandleTable[pinId]) {
        // Pin already allocated -> do nothing
        returnStatus = PIN_ALREADY_ALLOCATED;
    } else {
        // Allocate pin
        pinHandleTable[pinId] = handle;
        handle->portMask |= (1 << pinId);
        PIN_setConfig(handle, PIN_BM_ALL, pinCfg);
        returnStatus = PIN_SUCCESS;
    }

    SemaphoreP_post(&pinSemaphore);
    return returnStatus;
}



PIN_Status PIN_remove(PIN_Handle handle, PIN_Id pinId) {
    if (handle && (handle->portMask & (1 << pinId))) {
        // Deallocate pin
        handle->portMask &= ~(1 << pinId);
        pinHandleTable[pinId] = NULL;
        // Find GPIO default value and revert to it
        if (pinGpioConfigTable[pinId] == PIN_UNASSIGNED) {
            // Revert pin to default configuration:
            // GPIO, input buffer disable, GPIO output disable, low GPIO output, no pull, no IRQ, no wakeup
            PINCC26XX_setIoCfg(PIN_BM_ALL, PIN_ID(pinId) | PIN_INPUT_DIS);
        } else {
            // Revert pin to previous GPIO configuration
            PINCC26XX_setIoCfg(PIN_BM_ALL, defaultPinConfig[pinGpioConfigTable[pinId]]);
        }
        // Revert to GPIO
        PINCC26XX_setIoCfgMux(PIN_ID(pinId), -1);
        return PIN_SUCCESS;
    } else {
        return PIN_NO_ACCESS;
    }
}



void PIN_close(PIN_Handle handle) {
    uint32_t i;

    // No need for sequencing accesses to PIN_close()
    // For each pin in port bitmask
    while (handle->portMask) {
        // Find lowest index pin
        i = PIN_ctz(handle->portMask);
        // Deallocate pin
        PIN_remove(handle, i);
    }
}



uint32_t PIN_getInputValue(PIN_Id pinId) {
    return PINCC26XX_getInputValue(pinId);
}



PIN_Status PIN_setOutputEnable(PIN_Handle handle, PIN_Id pinId, bool outputEnable) {
    if (PIN_CHKEN && (pinId > pinUpperBound || pinHandleTable[pinId] != handle)) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setOutputEnable(pinId, outputEnable);
    return PIN_SUCCESS;
}



PIN_Status PIN_setOutputValue(PIN_Handle handle, PIN_Id pinId, uint32_t val) {
    if (PIN_CHKEN && (pinId > pinUpperBound || pinHandleTable[pinId] != handle)) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setOutputValue(pinId, val);
    return PIN_SUCCESS;
}



uint32_t PIN_getOutputValue(PIN_Id pinId) {
    return PINCC26XX_getOutputValue(pinId);
}



PIN_Status PIN_setInterrupt(PIN_Handle handle, PIN_Config pinCfg) {
    if (PIN_CHKEN && ((PIN_ID(pinCfg) > pinUpperBound) ||
                      (PIN_ID(pinCfg) >= MAX_NUM_PINS) ||
                      (pinHandleTable[PIN_ID(pinCfg)] != handle))) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PIN_setConfig(handle, PIN_BM_IRQ, pinCfg);
    return PIN_SUCCESS;
}



PIN_Status PIN_clrPendInterrupt(PIN_Handle handle, PIN_Id pinId) {
    if (PIN_CHKEN && ((pinId > pinUpperBound) ||
                      (pinId >= MAX_NUM_PINS) ||
                      (pinHandleTable[pinId] != handle))) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PINCC26XX_clrPendInterrupt(pinId);
    return PIN_SUCCESS;
}



PIN_Status PIN_registerIntCb(PIN_Handle handle, PIN_IntCb pCb) {
    if (handle) {
        handle->callbackFxn = pCb;
        return PIN_SUCCESS;
    } else {
        return PIN_NO_ACCESS;
    }
}



PIN_Config PIN_getConfig(PIN_Id pinId) {
    // Translate from device-specific to device independent PIN_Config values
    return PIN_GEN | (PINCC26XX_getConfig(pinId) ^ (PIN_BM_INPUT_EN | PIN_BM_PULLING));
}



PIN_Status PIN_setConfig(PIN_Handle handle, PIN_Config updateMask, PIN_Config pinCfg) {
    if (PIN_CHKEN && (PIN_ID(pinCfg) > pinUpperBound ||
                (PIN_ID(pinCfg) >= MAX_NUM_PINS) ||
                (pinHandleTable[PIN_ID(pinCfg)] != handle))) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setIoCfg(updateMask, pinCfg);
    return PIN_SUCCESS;
}



PIN_Config PINCC26XX_getConfig(PIN_Id pinId) {
    // Get IOCFG register value and add in some extras:
    // * pinId
    // * pin GPIO output enable
    // * pin GPIO output value
    uint32_t tmpConfig;

    tmpConfig = HWREG(IOC_BASE + IOC_O_IOCFG0 + 4 * pinId);
    tmpConfig &= PINCC26XX_BM_IOCFG;
    tmpConfig |= PIN_ID(pinId);
    tmpConfig |= (PINCC26XX_getOutputValue(pinId)) ? PINCC26XX_GPIO_HIGH : PINCC26XX_GPIO_LOW;
    tmpConfig |= (HWREG(GPIO_BASE + GPIO_O_DOE31_0) & (1 << pinId)) ? PINCC26XX_GPIO_OUTPUT_EN : 0;
    return tmpConfig;
}



int32_t PINCC26XX_getMux(PIN_Id pinId) {
    if (PIN_CHKEN && pinId > pinUpperBound) {
        // Non-existing pin
        return PIN_NO_ACCESS;
    }
    int32_t tmpConfig;
    tmpConfig = HWREG(IOC_BASE + IOC_O_IOCFG0 + 4 * pinId);
    tmpConfig &= IOC_IOCFG0_PORT_ID_M;
    if (tmpConfig == PINCC26XX_MUX_GPIO) {
        tmpConfig = -1;
    }
    return tmpConfig;
}



PIN_Status PINCC26XX_setMux(PIN_Handle handle, PIN_Id pinId, int32_t mux) {
    // Add check for pinId >= MAX_NUM_PINS for Klocwork
    if (PIN_CHKEN && ((pinId > pinUpperBound) ||
                (pinId >= MAX_NUM_PINS) ||
                (pinHandleTable[pinId] != handle))) {
        // Non-existing pin or pin is not allocated to this client
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setIoCfgMux(pinId, mux);
    return PIN_SUCCESS;
}



PIN_Status PINCC26XX_setWakeup(const PIN_Config pinConfig[]) {
    uint32_t i;
    // TODO: is this enough?

    for (i = 0; PIN_ID(pinConfig[i]) != PIN_TERMINATE; i++) {
        PINCC26XX_setIoCfg(PINCC26XX_BM_ALL, pinConfig[i]);
    }
    return PIN_SUCCESS;
}



uint32_t PIN_getPortMask(PIN_Handle handle) {
    // On CC26xx there is only one port encompassing all pins
    if (handle) {
        return handle->portMask;
    } else {
        return 0;
    }
}



uint32_t PIN_getPortInputValue(PIN_Handle handle) {
    return PINCC26XX_getPortInputValue(handle);
}



uint32_t PIN_getPortOutputValue(PIN_Handle handle) {
    return PINCC26XX_getPortOutputValue(handle);
}



PIN_Status PIN_setPortOutputValue(PIN_Handle handle, uint32_t bmOutVal) {
    if (PIN_CHKEN && (handle == NULL || pinHandleTable[PIN_ctz(handle->portMask)] != handle)) {
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setPortOutputValue(handle, bmOutVal);
    return PIN_SUCCESS;
}



PIN_Status PIN_setPortOutputEnable(PIN_Handle handle, uint32_t bmOutEn) {
    if (PIN_CHKEN && (handle == NULL || pinHandleTable[PIN_ctz(handle->portMask)] != handle)) {
        return PIN_NO_ACCESS;
    }
    PINCC26XX_setPortOutputEnable(handle, bmOutEn);
    return PIN_SUCCESS;
}
