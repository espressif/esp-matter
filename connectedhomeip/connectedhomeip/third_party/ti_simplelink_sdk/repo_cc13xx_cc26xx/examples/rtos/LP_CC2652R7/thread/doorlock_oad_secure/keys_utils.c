/******************************************************************************

 @file keys_utils.c

 @brief Utility functions for the launch pad keys

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include <stdbool.h>
#include <time.h>
#include <ti/drivers/GPIO.h>

/* Board Header files */
#include "ti_drivers_config.h"

#include "keys_utils.h"

/******************************************************************************
 Local Variables
 *****************************************************************************/
/* captures the keys pressed */
static volatile uint8_t keysPressed;

/* Pointer to application callback */
static KeyUtils_PressedCB_t appKeyChangeHandler = NULL;

/* Timer ID for the debounce key operation */
static timer_t keyTimerID;


/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Callback function for the GPIO Button
 *
 * @param index identifies the button pressed.
 * @return None
 */
void gpioButtonFxn(uint_least8_t index)
{
    struct itimerspec newTime = {0};
    struct itimerspec currTime;

    if (CONFIG_GPIO_BTN1 == index)
    {
        keysPressed |=  KEYS_LEFT;
    }
    else if(CONFIG_GPIO_BTN2 == index)
    {
        keysPressed |=  KEYS_RIGHT;
    }

    /* Arm timer if timer is disarmed */
    timer_gettime(keyTimerID, &currTime);
    if ((currTime.it_value.tv_sec == 0) && (currTime.it_value.tv_nsec == 0))
    {
        newTime.it_value.tv_sec  = (KEY_DEBOUNCE_TIMEOUT / 1000U);
        newTime.it_value.tv_nsec = ((KEY_DEBOUNCE_TIMEOUT % 1000U) * 1000000U);
        timer_settime(keyTimerID, 0, &newTime, NULL);
    }
}

/**
 * @brief       Callback function called when the key debounce
 *              period expires.
 *
 * @param val   input argument(ignored).
 * @return      None
 */
static void KeysUtils_changeHandler(union sigval val)
{
    (void) val;

    if(appKeyChangeHandler != NULL)
    {
        /* Notify the application */
        (*appKeyChangeHandler)(keysPressed);

        /* Clear keys */
        keysPressed = 0;
    }
}

/******************************************************************************
 External Functions
 *****************************************************************************/

/* refer keys_utils.h */
void KeysUtils_initialize(KeyUtils_PressedCB_t keyCb)
{
    /* install Button1 callback */
    GPIO_setCallback(CONFIG_GPIO_BTN1, gpioButtonFxn);

    /* Enable Button1 interrupt */
    GPIO_enableInt(CONFIG_GPIO_BTN1);

    /* install Button2 callback */
    GPIO_setCallback(CONFIG_GPIO_BTN2, gpioButtonFxn);

    /* Enable Button2 interrupt */
    GPIO_enableInt(CONFIG_GPIO_BTN2);

    /* Create timer for key debounce */
    struct sigevent event =
    {
        .sigev_notify_function = KeysUtils_changeHandler,
        .sigev_notify          = SIGEV_SIGNAL,
    };
    timer_create(CLOCK_MONOTONIC, &event, &keyTimerID);

    appKeyChangeHandler = keyCb;
}


