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

/*
 *  ======== temperatureNotify.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Temperature.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Function Prototypes */
void deltaNotificationFxn(int16_t currentTemperature,
                          int16_t thresholdTemperature,
                          uintptr_t clientArg,
                          Temperature_NotifyObj *notifyObject);
void aboveBelowNotificationFxn(int16_t currentTemperature,
                               int16_t thresholdTemperature,
                               uintptr_t clientArg,
                               Temperature_NotifyObj *notifyObject);

/* Defines */
#define THRESHOLD_DELTA 5
#define THRESHOLD_CUTOFF 20

/* Globals */
Temperature_NotifyObj deltaNotification;
Temperature_NotifyObj aboveBelowNotification;

/*
 *  ======== deltaNotificationFxn ========
 *  Temperature notification function that triggers whenever
 *  the temperature moves THRESHOLD_DELTA or more degrees C.
 *  It then reregisters itself with an updated set of
 *  thresholds based on the current temperature.
 */
void deltaNotificationFxn(int16_t currentTemperature,
                          int16_t thresholdTemperature,
                          uintptr_t clientArg,
                          Temperature_NotifyObj *notifyObject) {
    int_fast16_t status;

    GPIO_toggle(CONFIG_GPIO_LED_0);

    status = Temperature_registerNotifyRange(notifyObject,
                                             currentTemperature + THRESHOLD_DELTA,
                                             currentTemperature - THRESHOLD_DELTA,
                                             deltaNotificationFxn,
                                             (uintptr_t)NULL);

    if (status != Temperature_STATUS_SUCCESS) {
        while(1);
    }
}

/*
 *  ======== aboveBelowNotificationFxn ========
 *  Temperature notification function that triggers whenever
 *  the chip temperature crosses THRESHOLD_CUTOFF degrees C. When the
 *  temperature rises above THRESHOLD_CUTOFF, LED1 is turned on. When the
 *  temperature drops below THRESHOLD_CUTOFF again, LED1 is turned off.
 */
void aboveBelowNotificationFxn(int16_t currentTemperature,
                               int16_t thresholdTemperature,
                               uintptr_t clientArg,
                               Temperature_NotifyObj *notifyObject) {
    int_fast16_t status;

    if (currentTemperature >= THRESHOLD_CUTOFF) {
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);

        status = Temperature_registerNotifyLow(notifyObject,
                                               THRESHOLD_CUTOFF,
                                               aboveBelowNotificationFxn,
                                               (uintptr_t)NULL);

    }
    else {
        GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

        status = Temperature_registerNotifyHigh(notifyObject,
                                                THRESHOLD_CUTOFF,
                                                aboveBelowNotificationFxn,
                                                (uintptr_t)NULL);
    }

    if (status != Temperature_STATUS_SUCCESS) {
        while(1);
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    int16_t currentTemperature;
    Display_Handle displayHandle;

    /* Call driver init functions */
    GPIO_init();
    Temperature_init();
    Display_init();

    /* Open the UART display for output */
    displayHandle = Display_open(Display_Type_UART, NULL);
    if (displayHandle == NULL) {
        while(1);
    }

    Display_printf(displayHandle, 0, 0, "Temperature notify demo starting.");

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);

    currentTemperature = Temperature_getTemperature();
    Display_printf(displayHandle,
                   0,
                   0,
                   "Current temperature: %d degrees C",
                   currentTemperature);

    /* Call the notification function with the current temperature. It will
     * register the notification itself.
     */
    deltaNotificationFxn(currentTemperature,
                         currentTemperature + THRESHOLD_DELTA,
                         (uintptr_t)NULL,
                         &deltaNotification);

    /* Call the notification function with the current temperature. It will
     * register the notification itself.
     */
    aboveBelowNotificationFxn(currentTemperature,
                              THRESHOLD_CUTOFF,
                              (uintptr_t)NULL,
                              &aboveBelowNotification);

    while (1) {
        /* Sleep for 1s */
        sleep(1);

        /* Measure and send out the current temperature once per second */
        currentTemperature = Temperature_getTemperature();
        Display_printf(displayHandle,
                       0,
                       0,
                       "Current temperature: %d degrees C",
                       currentTemperature);
    }
}
