/******************************************************************************

   @file  UartLog.c

   @brief   This file provides a replacement / override for the
          XDC Runtime Log.h API that stores Log_info et al in
          a circular buffer and renders the text strings over
          UART in the Idle loop.

   Group: WCS, BTS
   Target Device: cc13xx_cc26xx

 ******************************************************************************
   
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include "UartLog.h"
#include <ti/drivers/UART.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>

#include <driverlib/aon_rtc.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * CONSTANTS
 */
#ifndef UARTLOG_NUM_EVT_BUF
#  define UARTLOG_NUM_EVT_BUF     32  /* Max log records in real-time buffer  */
#endif

#ifndef UARTLOG_OUTBUF_LEN
#  define UARTLOG_OUTBUF_LEN     168  /* Size of buffer used for log printout */
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
/* Local reduced copy of Log_EventRec */
typedef struct
{
    uint32_t tstamp_cust;                 /* 16.16 fractional from RTC       */
    char *file;
    int line;
    char *fmt;
    uintptr_t a0;
    uintptr_t a1;
    uintptr_t a2;
    uintptr_t a3;
    uintptr_t a4;
    uint16_t level;
    uint16_t serial_cust;                 /* Only 16-bit serial in this impl.*/
} uartLog_EventRec;

/*********************************************************************
 * LOCAL VARIABLES
 */
#if defined(UARTLOG_ENABLE)
static UART_Handle hUart = NULL;

char uartLog_outBuf[UARTLOG_OUTBUF_LEN + 4];
uartLog_EventRec uartLog_evBuf[UARTLOG_NUM_EVT_BUF];
uint8_t uartLog_tail = 0;
uint8_t uartLog_head = 0;
uint8_t uartLog_evBufIsEmpty = true;
uint16_t uartLog_evtNum = 1;
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#if defined(UARTLOG_ENABLE)
static void uartLog_doPrint(uartLog_EventRec *er);
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      UartLog_init
 *
 * @brief   Initializes module with the handle to the UART.
 *
 * @param   handle - UART driver handle to an initialized and opened UART.
 *
 * @return  None.
 */
void UartLog_doInit(UART_Handle handle)
{
#if defined(UARTLOG_ENABLE)
    hUart = handle;
#endif
}

void UartLog_log(char *file, int line, uint16_t level, char *fmt, uintptr_t a0,
                 uintptr_t a1, uintptr_t a2, uintptr_t a3,
                 uintptr_t a4)
{
#if defined(UARTLOG_ENABLE)
    unsigned int key;
    uint8_t moveTail = 0;

    /* Disable interrupts while adding record */
    key = Hwi_disable();

    /* Copy into current head */
    //uartLog_evBuf[uartLog_head].tstamp = pRec->tstamp; /* If real Log_EvtRec */
    uartLog_evBuf[uartLog_head].tstamp_cust = AONRTCCurrentCompareValueGet();
    uartLog_evBuf[uartLog_head].serial_cust = uartLog_evtNum;
    uartLog_evtNum++;

    uartLog_evBuf[uartLog_head].file = file;
    uartLog_evBuf[uartLog_head].line = line;
    uartLog_evBuf[uartLog_head].level = level;
    uartLog_evBuf[uartLog_head].fmt = fmt;

    uartLog_evBuf[uartLog_head].a0 = a0;
    uartLog_evBuf[uartLog_head].a1 = a1;
    uartLog_evBuf[uartLog_head].a2 = a2;
    uartLog_evBuf[uartLog_head].a3 = a3;
    uartLog_evBuf[uartLog_head].a4 = a4;

    /* Discard oldest if buffer is full */
    if(!uartLog_evBufIsEmpty && (uartLog_head == uartLog_tail))
    {
        moveTail = 1;
    }

    /* Increment head with wrap */
    uartLog_head += 1;
    if(uartLog_head == UARTLOG_NUM_EVT_BUF)
    {
        uartLog_head = 0;
    }

    if(moveTail)
    {
        uartLog_tail = uartLog_head;
    }

    /* This is used to discern whether head==tail means empty or full*/
    uartLog_evBufIsEmpty = false;

    /* Let mayhem commence */
    Hwi_restore(key);
#endif
}

/*********************************************************************
 * SYSTEM HOOK FUNCTIONS
 */

/*********************************************************************
 * @fn      uartLog_flush
 *
 * @brief   Log-buffer flush function
 *
 *          In this implementation it is intended to be called by the
 *          Idle task when nothing else is running.
 *
 *          This is achieved by setting up the Idle task in the TI-RTOS
 *          configuration script like so:
 *
 *          var Idle = xdc.useModule('ti.sysbios.knl.Idle');
 *          Idle.addFunc('&uartLog_flush');
 *
 *    NOTE: This must be added _before_ the Power driver is included, in order
 *          to output the pending log messages before going to sleep.
 *
 *          Uses a utility function to convert a log record to a user-friendlier
 *          string which is then printed to UART.
 *
 * @param   None. Relies on global state.
 *
 * @return  None.
 *
 * @post    ::uartLog_tail is incremented to where uartLog_head is, then returns
 */
void uartLog_flush()
{
#if defined(UARTLOG_ENABLE)
    unsigned int key;

    /* Local copy of current event record. To keep atomic section short. */
    uartLog_EventRec curRec;

    /* If we don't have UART, don't bother. */
    if(NULL == hUart)
    {
        return;
    }

    /* In the Idle function (this) send all messages. Will be preempted. */
    while(!uartLog_evBufIsEmpty)
    {
        /* Atomic section while manipulating the buffer. */
        key = Hwi_disable();

        /* Extract oldest and move tail */
        curRec = uartLog_evBuf[uartLog_tail];
        uartLog_tail = (uartLog_tail + 1) % UARTLOG_NUM_EVT_BUF;
        if(uartLog_tail == uartLog_head)
        {
            uartLog_evBufIsEmpty = true;
        }

        /* Let the others play. */
        Hwi_restore(key);

        /* Prepare log string from record, and print to UART. */
        uartLog_doPrint(&curRec);
    }
#endif
}

/*********************************************************************
 * INTERNAL FUNCTIONS
 */
/*********************************************************************
 * @fn      uartLog_doPrint
 *
 * @brief   Converts log records to strings.
 *
 *          This is a copy of ti.xdc.runtime.Log's doPrint method, but
 *          instead of calling System_printf, it writes into a static buffer
 *          which is then sent to the UART driver as an atomic unit.
 *
 * @param   er - Log_EventRecord to be parsed and output.
 *
 * @return  None.
 */
#if defined(UARTLOG_ENABLE)
static void uartLog_doPrint(uartLog_EventRec *er)
{
    char       *fmt;
    char       *bufPtr = uartLog_outBuf;
    char       *bufEndPtr = uartLog_outBuf + UARTLOG_OUTBUF_LEN - 2; // Less 2 for \r\n

    /* Print serial number */
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), "#%06u ", er->serial_cust);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    uint16_t seconds = er->tstamp_cust >> 16;
    uint16_t ifraction = er->tstamp_cust & 0xFFFF;
    int fraction = (int)((double)ifraction / 65536 * 1000);  // Get 3 decimals

    // Print time
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), "[ %d.%03u ] ", seconds,
                    fraction);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    // Print level, file and line
    char *infoStr = NULL;
    switch(er->level)
    {
    case LEVEL_ERROR:
        infoStr = "\x1b[31;1mERROR: \x1b[30;1m(%s:%d) \x1b[0m";
        break;
    case LEVEL_WARNING:
        infoStr = "\x1b[33;1mWARNING: \x1b[30;1m(%s:%d) \x1b[0m";
        break;
    case LEVEL_INFO:
    default:
        infoStr = "\x1b[32;1mINFO: \x1b[30;1m(%s:%d) \x1b[0m";
    }
    System_snprintf(bufPtr, (bufEndPtr - bufPtr), infoStr, er->file, er->line);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    /* Ouput everything till now and start over in the buffer. */
    UART_write(hUart, uartLog_outBuf, (bufPtr - uartLog_outBuf));
    bufPtr = uartLog_outBuf;

    /* Log_write() event */
    fmt = (char *) er->fmt;

    System_snprintf(bufPtr, (bufEndPtr - bufPtr), fmt, er->a0, er->a1,
                    er->a2, er->a3, er->a4);
    bufPtr = uartLog_outBuf + strlen(uartLog_outBuf);

    *bufPtr++ = '\r';
    *bufPtr++ = '\n';

    UART_write(hUart, uartLog_outBuf, (bufPtr - uartLog_outBuf));
}

#endif
