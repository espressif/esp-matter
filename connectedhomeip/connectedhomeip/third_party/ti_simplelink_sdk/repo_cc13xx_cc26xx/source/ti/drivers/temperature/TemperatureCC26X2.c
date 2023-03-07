/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
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

/* Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/Temperature.h>
#include <ti/drivers/temperature/TemperatureCC26X2.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)
#include DeviceFamily_constructPath(driverlib/aon_event.h)

/* Macros */
#define MIN(x,y)   (((x) < (y)) ?  (x) : (y))
#define MAX(x,y)   (((x) > (y)) ?  (x) : (y))

/* Defines */
#define BATMON_TEMPERATURE_MAX (250)
#define BATMON_TEMPERATURE_MIN (-250)
#define BATMON_TEMPERATURE_CODE_MAX 0x0000FF00
#define BATMON_TEMPERATURE_CODE_MIN 0x00010000
#define BATMON_TEMPERATURE_MASK_POSITIVE 0x0000FF00
#define BATMON_TEMPERATURE_MASK_NEGATIVE 0x0001FF00

/* Offset to apply to all thresholds before programming the hardware.
 *
 * When the hardware samples the temperature sensor, the returned reading
 * is drawn from a probability distribution of measurements for each true
 * temperature. In order for a notification to trigger, two separate
 * measurements must cross the configured threshold temperature.
 * The first is initiated by the hardware in the background to check whether
 * to trigger the HWI. The second is triggered by software within the HWI
 * function. If the first measurement is part of the long tail of the
 * distribution, it is highly probable that the second measurement will not
 * cross the threshold. This is effectively a spurrious interrupt that wastes
 * energy and CPU cycles.
 * If we program the actual hardware registers with an additional offset, we
 * effectively shift the distribution up or down such that the first measurement
 * triggers on the long tail of the distribution that is DISTRIBUTION_OFFSET
 * degrees away from the threshold provided by the application. The second
 * measurement then has a much higher probability of crossing the threshold
 * and triggering the notification and an update of the thresholds.
 *
 * The risk is of course that a particular chip has a compressed distribution
 * where the long tail does not reach far enough to trigger the HWI with the
 * offset applied.
 * Additionally, the device does not sample nearly as frequently when in
 * standby.
 *
 * Both of these risks result in a less accurate but overall more useful system.
 * If the temperature keeps rising, both scenarios still cause a notification
 * to trigger. Given that temperature changes are usually not instantaneous,
 * this should be considered an acceptable risk.
 */
#define DISTRIBUTION_OFFSET 2

#define INVALID_TEMPERATURE_MAX BATMON_TEMPERATURE_MAX
#define INVALID_TEMPERATURE_MIN BATMON_TEMPERATURE_MIN

/* Forward declarations */
static void walkNotifyList();
static void setNextThresholds();
static void temperatureHwiFxn(uintptr_t arg0);
static void updateThresholds(int16_t thresholdHigh, int16_t thresholdLow);
static uint32_t degreesToCode(int32_t temperatureDegreesC);
static void setTempLowerLimit(int16_t thresholdLow);
static void setTempUpperLimit(int16_t thresholdHigh);
static void enableTempLowerLimit();
static void enableTempUpperLimit();
static void disableTempLowerLimit();
static void disableTempUpperLimit();
static void clearEventFlags();

/* Globals */

/* Hwi struct for the shared batmon interrupt */
static HwiP_Struct batmonHwi;

/* Global list that stores all registered notifications */
static List_List notificationList;

/* Current threshold values. These should always reflect the state of the
 * batmon registers without the need to read them out, shift down, and sign
 * extend the values.
 */
static volatile int16_t currentThresholdHigh = INVALID_TEMPERATURE_MAX;
static volatile int16_t currentThresholdLow = INVALID_TEMPERATURE_MIN;

static bool isInitialized = 0;

extern const TemperatureCC26X2_Config TemperatureCC26X2_config;

/*
 *  ======== degreesToCode ========
 */
static uint32_t degreesToCode(int32_t temperatureDegreesC) {
    /* Voltage dependent temp correction with 8 fractional bits */
    int32_t correctionOffset;
    /* Signed byte value representing the TEMP offset slope with battery
     * voltage in degrees C/V with 4 fractional bits. This must be multiplied
     * by the voltage delta between the current voltage and the voltage used
     * to compute this slope.
     */
    int8_t voltageSlope;
    int32_t temperatureCode;

    /* Typecasting voltageSlope to int8_t prior to assignment in order to make
     * sure sign extension works properly.
     * Using byte read (HWREGB) in order to make more efficient code since
     * voltageSlope is assigned to bits[7:0] of FCFG1_O_MISC_TRIM
     */
    voltageSlope = ((int8_t)HWREGB(FCFG1_BASE + FCFG1_O_MISC_TRIM));

    /* Get the current supply voltage */
    correctionOffset = ((int32_t)HWREG(AON_BATMON_BASE + AON_BATMON_O_BAT));
    /* The voltageSlope is measured at 3V in production test. We need to remove
     * this from the current voltage to find the delta we need to apply.
     * At 3V, there should be no adjustment necessary. The BATMON voltage
     * measurement has 8 fractional bits.
     */
    correctionOffset = correctionOffset - (3 << 8);
    /* Multiply the delta with the voltageSlope. */
    correctionOffset = correctionOffset * voltageSlope;
    /* Right shift by four to remove the fractional bits */
    correctionOffset = correctionOffset >> 4;

    /* Shift up and then back down to sign-extend the temperatureCode.
     * Shift the temperature up by net 8 bit positions to move the integer part
     * into bits 16:8. This is what the register expects and gives us 8
     * fractional bits to work with as well for voltage compensation.
     */
    temperatureCode = (int32_t)((uint32_t)temperatureDegreesC << (32 - AON_BATMON_TEMP_INT_W));
    temperatureCode = temperatureCode >> (32 - AON_BATMON_TEMP_INT_W - AON_BATMON_TEMP_INT_S);

    /* 0x80 represents the rounding factor of half the previous net shift value */
    temperatureCode = temperatureCode - 0x80;

    temperatureCode = temperatureCode + correctionOffset;

    if (temperatureDegreesC <= BATMON_TEMPERATURE_MIN) {
        temperatureCode = BATMON_TEMPERATURE_CODE_MIN;
    }
    else if (temperatureDegreesC >= BATMON_TEMPERATURE_MAX) {
        temperatureCode = BATMON_TEMPERATURE_CODE_MAX;
    }

    if (temperatureCode < 0) {
        temperatureCode &= BATMON_TEMPERATURE_MASK_NEGATIVE;
    }
    else if (temperatureCode >= 0) {
        temperatureCode &= BATMON_TEMPERATURE_MASK_POSITIVE;
    }

    return (uint32_t)(temperatureCode);
}

/*
 *  ======== setTempLowerLimit ========
 */
static void setTempLowerLimit(int16_t thresholdLow) {
    uint32_t temperatureCode = degreesToCode(thresholdLow - DISTRIBUTION_OFFSET);

    HWREG(AON_BATMON_BASE + AON_BATMON_O_TEMPLL) = temperatureCode;

    currentThresholdLow = thresholdLow;
}

/*
 *  ======== setTempUpperLimit ========
 */
static void setTempUpperLimit(int16_t thresholdHigh) {
    uint32_t temperatureCode = degreesToCode(thresholdHigh + DISTRIBUTION_OFFSET);

    HWREG(AON_BATMON_BASE + AON_BATMON_O_TEMPUL) = temperatureCode;

    currentThresholdHigh = thresholdHigh;
}

/*
 *  ======== enableTempLowerLimit ========
 */
static void enableTempLowerLimit() {
    HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENTMASK) |= AON_BATMON_EVENTMASK_TEMP_BELOW_LL_MASK;
}

/*
 *  ======== enableTempUpperLimit ========
 */
static void enableTempUpperLimit() {
    HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENTMASK) |= AON_BATMON_EVENTMASK_TEMP_OVER_UL_MASK;
}

/*
 *  ======== disableTempLowerLimit ========
 */
static void disableTempLowerLimit() {
    HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENTMASK) &= ~AON_BATMON_EVENTMASK_TEMP_BELOW_LL_MASK;
}

/*
 *  ======== disableTempUpperLimit ========
 */
static void disableTempUpperLimit() {
    HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENTMASK) &= ~AON_BATMON_EVENTMASK_TEMP_OVER_UL_MASK;
}

/*
 *  ======== clearEventFlags ========
 */
static void clearEventFlags() {
    do {
        HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENT) &= AON_BATMON_EVENT_TEMP_BELOW_LL |
                                                       AON_BATMON_EVENT_TEMP_OVER_UL;
    } while (HWREG(AON_BATMON_BASE + AON_BATMON_O_EVENT) &
             (AON_BATMON_EVENT_TEMP_BELOW_LL | AON_BATMON_EVENT_TEMP_OVER_UL));
}

/*
 *  ======== setNextThresholds ========
 */
static void setNextThresholds() {
    List_Elem *notifyLink;
    int16_t nextThresholdHigh = INVALID_TEMPERATURE_MAX;
    int16_t nextThresholdLow = INVALID_TEMPERATURE_MIN;
    uint32_t key;

    key = HwiP_disable();

    /* Starting with the head of the list, keep track of the smallest high
     * threshold and largest low threshold.
     */
    notifyLink = List_head(&notificationList);

    while (notifyLink != NULL) {
        Temperature_NotifyObj* notifyObject = (Temperature_NotifyObj *)notifyLink;

        nextThresholdHigh = MIN(nextThresholdHigh,
                                notifyObject->thresholdHigh);
        nextThresholdLow = MAX(nextThresholdLow,
                               notifyObject->thresholdLow);

        notifyLink = List_next(notifyLink);
    }

    /* Now that we have found the next upper and lower thresholds, set them.
     * These could be INVALID_TEMPERATURE_MAX and/or INVALID_TEMPERATURE_MIN
     * if the list is empty or only high/low notifications were registered.
     */
    updateThresholds(nextThresholdHigh, nextThresholdLow);

    HwiP_restore(key);
}

/*
 *  ======== walkNotifyList ========
 */
static void walkNotifyList() {
    List_Elem *notifyLink       = List_head(&notificationList);
    int16_t currentTemperature  = Temperature_getTemperature();

    /* If the notification list is empty, the head pointer will be
     * NULL and the while loop will never execute the statement.
     */
    while (notifyLink != NULL) {
        Temperature_NotifyObj* notifyObject = (Temperature_NotifyObj *)notifyLink;

        /* Buffer the next link in case the notification triggers.
         * Without buffering, we might skip list entries if the
         * notifyObject is freed or reregistered and the notifyObject->link.next
         * pointer is altered.
         */
        List_Elem *notifyLinkNext = List_next(notifyLink);

        /* If the current temperature is below this notification's low
         * threshold or above its high threshold, remove it from the list and
         * call the callback fxn
         */
        if (currentTemperature <= notifyObject->thresholdLow ||
            currentTemperature >= notifyObject->thresholdHigh) {

            /* Choose the threshold to provide to the notifyFxn based on the
             * thresholds and the current temperature.
             */
            int16_t threshold = (currentTemperature <= notifyObject->thresholdLow) ?
                                notifyObject->thresholdLow : notifyObject->thresholdHigh;

            List_remove(&notificationList, notifyLink);
            notifyObject->isRegistered = false;

            notifyObject->notifyFxn(currentTemperature,
                                    threshold,
                                    notifyObject->clientArg,
                                    notifyObject);
        }

        notifyLink = notifyLinkNext;
    }
}

/*
 *  ======== updateThresholds ========
 */
static void updateThresholds(int16_t thresholdHigh, int16_t thresholdLow) {
    if (thresholdHigh < currentThresholdHigh) {
        setTempUpperLimit(thresholdHigh);
        enableTempUpperLimit();
    }

    if (thresholdLow > currentThresholdLow) {
        setTempLowerLimit(thresholdLow);
        enableTempLowerLimit();
    }
}

/*
 *  ======== temperatureHwiFxn ========
 *
 *  Batmon interrupt triggered on high or low temperature event
 */
static void temperatureHwiFxn(uintptr_t arg0) {

    setTempUpperLimit(INVALID_TEMPERATURE_MAX);
    disableTempUpperLimit();

    setTempLowerLimit(INVALID_TEMPERATURE_MIN);
    disableTempLowerLimit();


    /* Walk the notification list and issue any callbacks that have triggered
     * at the current temperature.
     */
    walkNotifyList();

    /* Walk the queue another time to find and set the next set of thresholds.
     * This is faster than making even one extra access to AON_BATMON.
     */
    setNextThresholds();



    /* Clear event flags. They may not immediately clear properly. */
    clearEventFlags();

    IntPendClear(INT_BATMON_COMB);
}

/*
 *  ======== Temperature_init ========
 */
void Temperature_init() {
    uint32_t key;

    key = HwiP_disable();

    if (isInitialized == false) {
        /* Initialise the batmon hwi. The temperature sensor shares this
         * interrupt with the battery voltage monitoring events.
         */
        HwiP_Params hwiParams;
        HwiP_Params_init(&hwiParams);
        hwiParams.priority = TemperatureCC26X2_config.intPriority;
        hwiParams.enableInt = true;
        HwiP_construct(&batmonHwi, INT_BATMON_COMB, temperatureHwiFxn, &hwiParams);

        disableTempUpperLimit();
        disableTempLowerLimit();

        AONBatMonEnable();

        /* Set the combined BATMON interrupt as a wakeup source. This means the
         * BATMON can bring the device out of standby when an event is
         * triggered.
         * We use WU2 since WU0 is the RTC and WU1 is a pad (GPIO) event.
         */
        AONEventMcuWakeUpSet(AON_EVENT_MCU_WU2, AON_EVENT_BATMON_COMBINED);

        isInitialized = true;
    }

    HwiP_restore(key);
}

/*
 *  ======== Temperature_getTemperature ========
 */
int16_t Temperature_getTemperature(void) {
    /* The temperature on CC13X2/CC26X2 is stored in a 32-bit register
     * containing a 9-bit signed integer part and a 2-bit unsigned fractional
     * part.
     * The driverlib fxn handles this as well as compensating for the battery
     * voltage which also affects the measured temperature.
     */
    int16_t currentTemperature = AONBatMonTemperatureGetDegC();

    return currentTemperature;
}

/*
 *  ======== Temperature_registerNotifyHigh ========
 */
int_fast16_t Temperature_registerNotifyHigh(Temperature_NotifyObj *notifyObject,
                                            int16_t thresholdHigh,
                                            Temperature_NotifyFxn notifyFxn,
                                            uintptr_t clientArg) {
    uint32_t key;

    key = HwiP_disable();

    notifyObject->thresholdHigh = thresholdHigh;
    notifyObject->thresholdLow  = INVALID_TEMPERATURE_MIN;
    notifyObject->notifyFxn     = notifyFxn;
    notifyObject->clientArg     = clientArg;

    if (notifyObject->isRegistered == false) {
        /* Add the notification to the end of the list.
         * There is the implicit assumption that the notification is not already
         * in the list. Otherwise the list linkage will be corrupted.
         */
        List_put(&notificationList, &notifyObject->link);

        notifyObject->isRegistered = true;
    }

    updateThresholds(notifyObject->thresholdHigh, notifyObject->thresholdLow);

    HwiP_restore(key);

    return Temperature_STATUS_SUCCESS;
}

/*
 *  ======== Temperature_registerNotifyLow ========
 */
int_fast16_t Temperature_registerNotifyLow(Temperature_NotifyObj *notifyObject,
                                           int16_t thresholdLow,
                                           Temperature_NotifyFxn notifyFxn,
                                           uintptr_t clientArg){
    uint32_t key;

    key = HwiP_disable();

    notifyObject->thresholdHigh = INVALID_TEMPERATURE_MAX;
    notifyObject->thresholdLow  = thresholdLow;
    notifyObject->notifyFxn     = notifyFxn;
    notifyObject->clientArg     = clientArg;

    if (notifyObject->isRegistered == false) {
        /* Add the notification to the end of the list.
         * There is the implicit assumption that the notification is not already
         * in the list. Otherwise the list linkage will be corrupted.
         */
        List_put(&notificationList, &notifyObject->link);

        notifyObject->isRegistered = true;
    }

    updateThresholds(notifyObject->thresholdHigh, notifyObject->thresholdLow);

    HwiP_restore(key);

    return Temperature_STATUS_SUCCESS;
}

/*
 *  ======== Temperature_registerNotifyRange ========
 */
int_fast16_t Temperature_registerNotifyRange(Temperature_NotifyObj *notifyObject,
                                             int16_t thresholdHigh,
                                             int16_t thresholdLow,
                                             Temperature_NotifyFxn notifyFxn,
                                             uintptr_t clientArg) {
    uint32_t key;

    key = HwiP_disable();

    notifyObject->thresholdHigh = thresholdHigh;
    notifyObject->thresholdLow  = thresholdLow;
    notifyObject->notifyFxn     = notifyFxn;
    notifyObject->clientArg     = clientArg;

    if (notifyObject->isRegistered == false) {
        /* Add the notification to the end of the list.
         * There is the implicit assumption that the notification is not already
         * in the list. Otherwise the list linkage will be corrupted.
         */
        List_put(&notificationList, &notifyObject->link);

        notifyObject->isRegistered = true;
    }

    updateThresholds(notifyObject->thresholdHigh, notifyObject->thresholdLow);

    HwiP_restore(key);

    return Temperature_STATUS_SUCCESS;
}

/*
 *  ======== Temperature_unregisterNotify ========
 */
int_fast16_t Temperature_unregisterNotify(Temperature_NotifyObj *notifyObject) {
    uint32_t key;

    key = HwiP_disable();

    if (notifyObject->isRegistered == true) {
        /* Remove the notification from the list */
        List_remove(&notificationList, &(notifyObject->link));

        notifyObject->isRegistered = false;
    }

    /* Find the next set of thresholds and update the registers */
    setNextThresholds();

    HwiP_restore(key);

    return Temperature_STATUS_SUCCESS;
}
