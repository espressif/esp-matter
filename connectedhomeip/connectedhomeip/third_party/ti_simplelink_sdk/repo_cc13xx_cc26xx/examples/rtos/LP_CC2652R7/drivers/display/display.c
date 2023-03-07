/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
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
 *  ======== display.c ========
 */
#include <stdint.h>
#include <unistd.h>

/* TI-Drivers Header files */
#include <ti/drivers/GPIO.h>

/* Display Header files */
#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>
#include <ti/display/DisplayExt.h>
#include <ti/display/AnsiColor.h>

/* Driver Configuration */
#include "ti_drivers_config.h"

/* Example GrLib image */
#include "splash_image.h"

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    unsigned int ledPinValue;
    unsigned int loopCount = 0;

    GPIO_init();
    Display_init();

    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Initialize display and try to open both UART and LCD types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    /*
     * Open both an available LCD display and an UART display.
     * Whether the open call is successful depends on what is present in the
     * Display_config[] array of the driver configuration file.
     */
    Display_Handle hLcd = Display_open(Display_Type_LCD, &params);
    Display_Handle hSerial = Display_open(Display_Type_UART, &params);

    if (hSerial == NULL && hLcd == NULL) {
        /* Failed to open a display */
        while (1) {}
    }

    /* Check if the selected Display type was found and successfully opened */
    if (hSerial) {
        Display_printf(hSerial, 0, 0, "Hello Serial!");
    }
    else
    {
        /*
         * Print unavail message on LCD. Note it's not necessary to
         * check for NULL handles with Display APIs, so just assume hLcd works.
         */
        Display_printf(hLcd, 4, 0, "Serial display");
        Display_printf(hLcd, 5, 0, "not present");
        sleep(1);
    }

    /* Check if the selected Display type was found and successfully opened */
    if (hLcd) {
        Display_printf(hLcd, 5, 3, "Hello LCD!");

        /* Wait a while so text can be viewed. */
        sleep(3);

        /*
         * Use the GrLib extension to get the GraphicsLib context object of the
         * LCD, if it is supported by the display type.
         */
        Graphics_Context *context = DisplayExt_getGraphicsContext(hLcd);

        /* It's possible that no compatible display is available. */
        if (context) {
            /* Calculate X-Y coordinate to center the splash image */
            int splashX = (Graphics_getDisplayWidth(context) - Graphics_getImageWidth(&splashImage)) / 2;
            int splashY = (Graphics_getDisplayHeight(context) - Graphics_getImageHeight(&splashImage)) / 2;

            /* Draw splash */
            Graphics_drawImage(context, &splashImage, splashX, splashY);
            Graphics_flushBuffer(context);
        }
        else {
            /* Not all displays have a GraphicsLib back-end */
            Display_printf(hLcd, 0, 0, "Display driver");
            Display_printf(hLcd, 1, 0, "is not");
            Display_printf(hLcd, 2, 0, "GrLib capable!");
        }

        /* Wait for a bit, then clear */
        sleep(3);
        Display_clear(hLcd);
    }
    else
    {
        Display_printf(hSerial, 1, 0, "LCD display not present");
        sleep(1);
    }

    char *serialLedOn = "On";
    char *serialLedOff = "Off";

    /*
     * If serial display can handle ANSI colors, use colored strings instead.
     *
     * You configure DisplayUart to use the ANSI variant by choosing what
     * function pointer list it should use. Ex:
     *
     * const Display_Config Display_config[] = {
     *   {
     *      .fxnTablePtr = &DisplayUartAnsi_fxnTable, // Alt: DisplayUartMin_
     *      ...
     */
    if (Display_getType(hSerial) & Display_Type_ANSI)
    {
        serialLedOn = ANSI_COLOR(FG_GREEN, ATTR_BOLD) "On" ANSI_COLOR(ATTR_RESET);
        serialLedOff = ANSI_COLOR(FG_RED, ATTR_UNDERLINE) "Off" ANSI_COLOR(ATTR_RESET);
    }

    /* Loop forever, alternating LED state and Display output. */
    while (1) {
        ledPinValue = GPIO_read(CONFIG_GPIO_LED_0);

        /* Print to LCD and clear alternate lines if the LED is on or not. */
        Display_clearLine(hLcd, ledPinValue ? 1 : 0);
        Display_printf(hLcd, ledPinValue ? 0 : 1, 0, "LED: %s",
                (ledPinValue == CONFIG_GPIO_LED_ON) ? "On!":"Off!");

        /* Print to UART */
        Display_clearLine(hSerial, ledPinValue ? 1 : 0);
        Display_printf(hSerial, ledPinValue ? 0 : 1, 0, "LED: %s",
                (ledPinValue == CONFIG_GPIO_LED_ON) ? serialLedOn : serialLedOff);

        /* If ANSI is supported, print a "log" in the scrolling region */
        if (Display_getType(hSerial) & Display_Type_ANSI)
        {
            char *currLedState = (ledPinValue == CONFIG_GPIO_LED_ON) ? serialLedOn : serialLedOff;
            Display_printf(hSerial, DisplayUart_SCROLLING, 0, "[ %d ] LED: %s", loopCount++, currLedState);
        }

        sleep(1);

        /* Toggle LED */
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }
}
