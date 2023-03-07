/******************************************************************************

 @file uart.c

 @brief Debug Uart implimentation for OpenThread

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
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

#include <openthread/config.h>

#include <stddef.h>

#include <utils/code_utils.h>
#include <openthread/platform/uart.h>
#include <openthread/platform/debug_uart.h>

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include DeviceFamily_constructPath(driverlib/prcm.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

#include "ti_drivers_config.h"
#include "system.h"

#if OPENTHREAD_CONFIG_ENABLE_DEBUG_UART

/*
 * Initialize the second uart for debug purposes.
 */
void
platformDebugUartInit(void)
{
    /* UART1 is in the Periph Domain */
    Power_setDependency(PowerCC26XX_DOMAIN_PERIPH);

    PRCMPeripheralRunEnable(PRCM_PERIPH_UART1);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_UART1);
    PRCMPeripheralDeepSleepEnable(PRCM_PERIPH_UART1);
    PRCMLoadSet();

    while (!PRCMLoadGet())
    {
        ;
    }
    Power_setConstraint(PowerCC26XX_SB_DISALLOW);

    UARTConfigSetExpClk( UART1_BASE,
                         SysCtrlClockGet(),
                         115200,
                         UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );

#if !defined(DEBUG_UART_TXD_PIN)
    /*
     * Choose default values.
     */
#define DEBUG_UART_TXD_PIN IOID_18
#define DEBUG_UART_RXD_PIN IOID_19
#endif

    IOCPortConfigureSet( DEBUG_UART_TXD_PIN, IOC_PORT_MCU_UART1_TX, IOC_STD_INPUT);
    IOCPortConfigureSet( DEBUG_UART_RXD_PIN, IOC_PORT_MCU_UART1_RX, IOC_STD_INPUT);

    UARTEnable( UART1_BASE );
}

/* This holds the last key pressed */
static int debug_uart_ungetbuf;

/*
 * Documented in "src/core/common/debug_uart.h"
 */
int otPlatDebugUart_getc(void)
{
    int ch = -1;

    if (otPlatDebugUart_kbhit())
    {
        /* Step 1 - unmap [1..256] to [0..255], subtract
         * Step 2 - mask upper bits
         */
        ch = (debug_uart_ungetbuf-1) & 0x0ff;
        /* clear flag */
        debug_uart_ungetbuf = 0;
    }

    return ch;
}

/*
 * Documented in "src/core/common/debug_uart.h"
 */
int otPlatDebugUart_kbhit(void)
{
    int r;

    /* if something is in the unget buf... */
    r = debug_uart_ungetbuf;

    if (!r)
    {
        /* nothing in the unget buf, we must call and test
         * This returns with (-1 = nokey) or (data > -1)
         */
        r = (int)UARTCharGetNonBlocking(UART1_BASE);

        if (r < 0)
        {
            r = 0; /* no key pressed */
        }
        else
        {
            /* A key was pressed, we have a special case the value
             * recieved might be 0.  this conflicts with the flag
             * value of 0 = no data.
             *
             * Solution:
             * Now: map [0..255] to [1..256] by adding 1.
             * Later: sub 1 to map [1..256] to [0..255]
             */
            debug_uart_ungetbuf = r+1;
            /* return data present */
            r = 1;
        }
    }

    return r;
}

/*
 * Documented in "src/core/common/debug_uart.h"
 */
void otPlatDebugUart_putchar_raw(int b)
{
    /*
     * Note: This is purposely "polled mode operation.
     * this allows the debug uart to be used inside of
     * an interrupt service routine.
     */
    UARTCharPut(UART1_BASE, b);
}

#endif /* OPENTHREAD_CONFIG_ENABLE_DEBUG_UART */
