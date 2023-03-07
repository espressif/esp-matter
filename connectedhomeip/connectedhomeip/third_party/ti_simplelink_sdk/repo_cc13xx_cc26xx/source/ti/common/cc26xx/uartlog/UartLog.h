/******************************************************************************

   @file  UartLog.h

   @brief   This file provides a replacement / override for the
          XDC Runtime Log.h API that stores Log_info et al in
          a circular buffer and renders the text strings over
          UART in the Idle loop.

          To enable this Log replacement, you must
              1) Add the global precompiler define `UARTLOG_ENABLE`
              2) #include <UartLog.h> *instead of* <xdc/runtime/Log.h>
                 if you have it in your application.

          If you want Log statements to be owned by xdc runtime, undefine
          UARTLOG_ENABLE globally and include <xdc/runtime/Log.h> instead.

          To remove all Log calls,
              1) Add precompiler symbol `xdc_runtime_Log_DISABLE_ALL`
              2) Remove precompiler symbol `UARTLOG_ENABLE`

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

#ifndef UART_LOG_H
#define UART_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif
/*********************************************************************
 * INCLUDES
 */
#include <ti/drivers/UART.h>

/*********************************************************************
 * CONSTANTS
 */
#define LEVEL_INFO     0x01
#define LEVEL_WARNING  0x02
#define LEVEL_ERROR    0x03

/*********************************************************************
 * MACROS
 */
// Precompiler define for CCS to get only filename: uartlog_FILE="\"${InputFileName}\""
// IAR project should have extra option --no_path_in_file_macros to only have base file name.
#if !defined(uartlog_FILE)
#  define uartlog_FILE __FILE__
#endif

#if defined(UARTLOG_ENABLE)
#  define UartLog_init(x) UartLog_doInit(x)
#else
#  define UartLog_init(x)
#endif

#if defined(UARTLOG_ENABLE)
#  define Log_info0(fmt) _Log_info((char *)fmt, 0, 0, 0, 0, 0)
#  define Log_info1(fmt, a0) _Log_info((char *)fmt, a0, 0, 0, 0, 0)
#  define Log_info2(fmt, a0, a1) _Log_info((char *)fmt, a0, a1, 0, 0, 0)
#  define Log_info3(fmt, a0, a1, a2) _Log_info((char *)fmt, a0, a1, a2, 0, 0)
#  define Log_info4(fmt, a0, a1, a2, a3) _Log_info((char *)fmt, a0, a1, a2, a3, \
                                                   0)
#  define Log_info5(fmt, a0, a1, a2, a3, a4) _Log_info((char *)fmt, a0, a1, a2, \
                                                       a3, \
                                                       a4)
#  define _Log_info(fmt, ...) UartLog_log(uartlog_FILE, __LINE__, LEVEL_INFO, \
                                          fmt, \
                                          __VA_ARGS__)

#  define Log_warning0(fmt) _Log_warning((char *)fmt, 0, 0, 0, 0, 0)
#  define Log_warning1(fmt, a0) _Log_warning((char *)fmt, a0, 0, 0, 0, 0)
#  define Log_warning2(fmt, a0, a1) _Log_warning((char *)fmt, a0, a1, 0, 0, 0)
#  define Log_warning3(fmt, a0, a1, a2) _Log_warning((char *)fmt, a0, a1, a2, 0, \
                                                     0)
#  define Log_warning4(fmt, a0, a1, a2, a3) _Log_warning((char *)fmt, a0, a1, \
                                                         a2, a3, \
                                                         0)
#  define Log_warning5(fmt, a0, a1, a2, a3, a4) _Log_warning((char *)fmt, a0, \
                                                             a1, a2, a3, \
                                                             a4)
#  define _Log_warning(fmt, ...) UartLog_log(uartlog_FILE, __LINE__, \
                                             LEVEL_WARNING, fmt, \
                                             __VA_ARGS__)

#  define Log_error0(fmt) _Log_error(fmt, 0, 0, 0, 0, 0)
#  define Log_error1(fmt, a0) _Log_error(fmt, a0, 0, 0, 0, 0)
#  define Log_error2(fmt, a0, a1) _Log_error(fmt, a0, a1, 0, 0, 0)
#  define Log_error3(fmt, a0, a1, a2) _Log_error(fmt, a0, a1, a2, 0, 0)
#  define Log_error4(fmt, a0, a1, a2, a3) _Log_error(fmt, a0, a1, a2, a3, 0)
#  define Log_error5(fmt, a0, a1, a2, a3, a4) _Log_error(fmt, a0, a1, a2, a3, \
                                                         a4)
#  define _Log_error(fmt, ...) UartLog_log(uartlog_FILE, __LINE__, LEVEL_ERROR, \
                                           fmt, \
                                           __VA_ARGS__)
#else
#  define Log_info0(fmt)
#  define Log_info1(fmt, a0)
#  define Log_info2(fmt, a0, a1)
#  define Log_info3(fmt, a0, a1, a2)
#  define Log_info4(fmt, a0, a1, a2, a3)
#  define Log_info5(fmt, a0, a1, a2, a3, a4)

#  define Log_warning0(fmt)
#  define Log_warning1(fmt, a0)
#  define Log_warning2(fmt, a0, a1)
#  define Log_warning3(fmt, a0, a1, a2)
#  define Log_warning4(fmt, a0, a1, a2, a3)
#  define Log_warning5(fmt, a0, a1, a2, a3, a4)

#  define Log_error0(fmt)
#  define Log_error1(fmt, a0)
#  define Log_error2(fmt, a0, a1)
#  define Log_error3(fmt, a0, a1, a2)
#  define Log_error4(fmt, a0, a1, a2, a3)
#  define Log_error5(fmt, a0, a1, a2, a3, a4)

#endif
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      UartLog_doInit
 *
 * @brief   Initializes module with the handle to the UART.
 *
 * @param   handle - UART driver handle to an initialized and opened UART.
 *
 * @return  None.
 */
void UartLog_doInit(UART_Handle handle);

void UartLog_log(char *file,
                 int line,
                 uint16_t level,
                 char *fmt,
                 uintptr_t a0,
                 uintptr_t a1,
                 uintptr_t a2,
                 uintptr_t a3,
                 uintptr_t a4);

#ifdef __cplusplus
}
#endif

#endif // UART_LOG_H
