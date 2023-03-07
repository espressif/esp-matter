/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
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

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes

#ifndef DEBUG_IF_H
#define DEBUG_IF_H
#include <stdio.h>
//*****************************************************************************
//                 DEBUG IF DEFINITIONS
//*****************************************************************************
/* Debug Severity Levels Definition */
typedef enum
{
    E_TRACE, E_DEBUG, E_INFO, E_WARNING, E_ERROR, E_FATAL, E_DISABLED
} severity_e;

/* Debug (Printf) Methods Definition */
#define  D_UART_PRINT       0
#define  D_DISPLAY_PRINT    1
#define  D_USER_DEFINED     2

// Generic IF module name (Can be overridden in the module implementation)
#define DEBUG_IF_NAME "IFX"

// Default IF Module severity (can be overridden in the module impl.)
#define DEBUG_IF_SEVERITY E_INFO

/* Color Coding */
#define _CLR_RED_     "\033[0;31m"
#define _CLR_B_RED_   "\033[1;31m"
#define _CLR_GREEN_   "\033[0;32m"
#define _CLR_YELLOW_  "\033[0;33m"
#define _CLR_BLUE_    "\033[0;34m"
#define _CLR_MAGENTA_ "\033[0;35m"
#define _CLR_RESET_   "\033[0m"

//*****************************************************************************
//                DEBUG IF USER SETTINGS
//*****************************************************************************

#include "debug_settings.h"

//*****************************************************************************
//                 DEBUG_IF Print MACROS
//*****************************************************************************

#if (D_DEBUG_METHOD == D_UART_PRINT)
#include "uart_if.h"
#define PRINTF(...) UART_PRINT(__VA_ARGS__);  UART_PRINT("\n\r");
#elif (D_DEBUG_METHOD == D_DISPLAY_PRINT)
#include <ti/display/Display.h>
extern Display_Handle display;
#define PRINTF(...) Display_printf(display, 0, 0, __VA_ARGS__);
#endif

#ifndef PRINTF
#error "Missing definition of PRINTF method"
#endif

#if (D_DEBUG_METHOD != D_UART_PRINT)
extern void Report(const char * msg, ...);
#define Report(...) PRINTF(__VA_ARGS__)
#endif


#define LOG(_severity_, ...) if((_severity_)>= DEBUG_IF_SEVERITY) { PRINTF(_CLR_RESET_   "[" DEBUG_IF_NAME "] " __VA_ARGS__); }
#define LOG_FATAL( ...)      if(E_FATAL >= DEBUG_IF_SEVERITY)     { PRINTF(_FATAL_CLR_   "[" DEBUG_IF_NAME "::FATAL] " __VA_ARGS__); }
#define LOG_ERROR( ...)      if(E_ERROR >= DEBUG_IF_SEVERITY)     { PRINTF(_ERROR_CLR_   "[" DEBUG_IF_NAME "::ERROR] " __VA_ARGS__); }
#define LOG_WARNING( ...)    if(E_WARNING >= DEBUG_IF_SEVERITY)   { PRINTF(_WARNING_CLR_ "[" DEBUG_IF_NAME "::WARN]  " __VA_ARGS__); }
#define LOG_INFO( ...)       if(E_INFO >= DEBUG_IF_SEVERITY)      { PRINTF(_INFO_CLR_    "[" DEBUG_IF_NAME "::INFO]  " __VA_ARGS__); }
#define LOG_DEBUG( ...)      if(E_DEBUG >= DEBUG_IF_SEVERITY)     { PRINTF(_DEBUG_CLR_   "[" DEBUG_IF_NAME "::DEBUG] " __VA_ARGS__); }
#define LOG_TRACE( ...)      if(E_TRACE >= DEBUG_IF_SEVERITY)     { PRINTF(_TRACE_CLR_   "[" DEBUG_IF_NAME "::TRACE] " __VA_ARGS__); }

#define DISPLAY_BANNER(AppName, AppVer) LOG_INFO("\n\r\t\t =================================================\n\r\t\t           %s Example Ver. %s      \n\r\t\t =================================================\n\r", AppName, AppVer);

#define LOG_ERROR_IF_NEG(code, ...) if((code) < 0) { LOG_ERROR(__VA_ARGS__ " (%d)", code); return code;}


#endif // DEBUG_IF_H
