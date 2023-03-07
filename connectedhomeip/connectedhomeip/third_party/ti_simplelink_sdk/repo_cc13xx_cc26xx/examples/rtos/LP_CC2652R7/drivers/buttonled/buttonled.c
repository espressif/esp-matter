/*
 * Copyright (c) 2019-2021, Texas Instruments Incorporated
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
 *    ======== buttonled.c ========
 */
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/display/Display.h>
#include <ti/drivers/utils/RingBuf.h>
#include <ti/drivers/apps/LED.h>
#include <ti/drivers/apps/Button.h>

/* Driver Configuration */
#include "ti_drivers_config.h"

#define BLINKCOUNT            3
#define FASTBLINK             500
#define SLOWBLINK             1000
#define FIFTYMS               50000
#define EVENTBUFSIZE          10


#ifndef CONFIG_BUTTONCOUNT
#define CONFIG_BUTTONCOUNT     2
#endif

#ifndef CONFIG_LEDCOUNT
#define CONFIG_LEDCOUNT        2
#else
#define CONFIG_LED2            2
#endif

typedef struct buttonStats
{
    unsigned int pressed;
    unsigned int clicked;
    unsigned int released;
    unsigned int longPress;
    unsigned int longClicked;
    unsigned int doubleclicked;
    unsigned int lastpressedduration;
} buttonStats;

Button_Handle    buttonHandle[CONFIG_BUTTONCOUNT];
LED_Handle       ledHandle[CONFIG_LEDCOUNT];
Display_Handle   display;
buttonStats      bStats;
RingBuf_Object   ringObj;
uint8_t          eventBuf[EVENTBUFSIZE];


/*
 *  ======== doEventLogs ========
 */
void doEventLogs(void)
{
    uint8_t event;
    while(RingBuf_get(&ringObj, &event) >= 0)
    {
        if(event & Button_EV_CLICKED)
        {
            Display_print0(display, 0, 0, "Button:Click");
        }
        if(event & Button_EV_DOUBLECLICKED)
        {
            Display_print0(display, 0, 0, "Button:Double Click");
        }
        if(event & Button_EV_LONGPRESSED)
        {
            Display_print0(display, 0, 0, "Button:Long Pressed");
        }
    }
}

/*
 *  ======== handleButtonCallback ========
 */
void handleButtonCallback(Button_Handle handle, Button_EventMask events)
{
    uint_least8_t ledIndex = (buttonHandle[CONFIG_BUTTON_0] == handle) ?
                              CONFIG_LED_0 : CONFIG_LED_1;
    LED_Handle led = ledHandle[ledIndex];

    if(Button_EV_PRESSED == (events & Button_EV_PRESSED))
    {
        bStats.pressed++;
    }

    if(Button_EV_RELEASED == (events & Button_EV_RELEASED))
    {
        bStats.released++;
    }

    if(Button_EV_CLICKED == (events & Button_EV_CLICKED))
    {
        bStats.clicked++;
        bStats.lastpressedduration =
                Button_getLastPressedDuration(handle);

        /* Put event in ring buffer for printing */
        RingBuf_put(&ringObj, events);

        if(LED_STATE_BLINKING == LED_getState(led))
        {
            LED_stopBlinking(led);
            LED_setOff(led);
        }
        else
        {
            LED_toggle(led);
        }
    }

    if(Button_EV_LONGPRESSED == (events & Button_EV_LONGPRESSED))
    {
        bStats.longPress++;

        /* Put event in ring buffer for printing */
        RingBuf_put(&ringObj, events);

        LED_startBlinking(led, SLOWBLINK, LED_BLINK_FOREVER);
    }

    if(Button_EV_LONGCLICKED == (events & Button_EV_LONGCLICKED))
    {
        bStats.longClicked++;
        bStats.lastpressedduration = Button_getLastPressedDuration(handle);
        LED_stopBlinking(led);
    }

    if(Button_EV_DOUBLECLICKED == (events & Button_EV_DOUBLECLICKED))
    {
        bStats.doubleclicked++;

        /* Put event in ring buffer for printing */
        RingBuf_put(&ringObj, events);

        if(LED_STATE_BLINKING != LED_getState(led))
        {
            LED_startBlinking(led, FASTBLINK, BLINKCOUNT);
        }
        else
        {
            LED_stopBlinking(led);
            LED_setOff(led);
        }
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    int inc;
    bool dir = true;

    Button_Params  buttonParams;
    LED_Params     ledParams;

    GPIO_init();
    Button_init();
    LED_init();

    /* Create ring buffer to store button events */
    RingBuf_construct(&ringObj, eventBuf, EVENTBUFSIZE);

    /* Open the UART display for output */
    display = Display_open(Display_Type_UART, NULL);
    if(display == NULL)
    {
        while(1);
    }

    Display_print0(display, 0, 0, "Button/LED Demo:\n"
                   "Each button controls an LED. Click to toggle, "
                   "double click to fast blink three times, "
                   "hold the button to slow blink.\n");

    /* Open button 1 and button 2 */
    Button_Params_init(&buttonParams);
    buttonParams.buttonCallback = handleButtonCallback;
    buttonHandle[CONFIG_BUTTON_0] = Button_open(CONFIG_BUTTON_0, &buttonParams);
    buttonHandle[CONFIG_BUTTON_1] = Button_open(CONFIG_BUTTON_1, &buttonParams);

    /* Check if the button open is successful */
    if((buttonHandle[CONFIG_BUTTON_1]  == NULL) ||
        (buttonHandle[CONFIG_BUTTON_0]  == NULL))
    {
        Display_print0(display, 0, 0, "Button Open Failed!");
    }

    /* Open LED0 and LED1 with default params */
    LED_Params_init(&ledParams);
    ledHandle[CONFIG_LED_0] = LED_open(CONFIG_LED_0, &ledParams);
    ledHandle[CONFIG_LED_1] = LED_open(CONFIG_LED_1, &ledParams);
    if((ledHandle[CONFIG_LED_0] == NULL) || (ledHandle[CONFIG_LED_1] == NULL))
    {
        Display_print0(display, 0, 0, "LED Open Failed!");
    }

#if CONFIG_LEDCOUNT > 2
    /* Open a PWM LED if our board has one */
    ledParams.setState = LED_STATE_ON;
    ledHandle[CONFIG_LED_2] = LED_open(CONFIG_LED_2, &ledParams);
    if(ledHandle[CONFIG_LED_2] == NULL)
    {
        Display_print0(display, 0, 0, "PWM LED Open Failed!");
    }
#endif


    while(1)
    {

        /* Does a "heart beat" effect for the PWM LED if we opened one */
        for(inc = 0; inc < 100; inc += 5)
        {
#if CONFIG_LEDCOUNT > 2
            int duty;
            if(dir)
            {
                duty = inc;
            }
            else
            {
                duty = 100 - inc;
            }
            LED_setOn(ledHandle[CONFIG_LED_2], duty);
#endif

            /* Print out button events */
            doEventLogs();
            usleep(FIFTYMS);
        }
        dir = !dir;
    }
}
