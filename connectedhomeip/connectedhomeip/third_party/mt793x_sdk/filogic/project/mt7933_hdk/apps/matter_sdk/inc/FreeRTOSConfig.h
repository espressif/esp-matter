/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
	See http://www.freertos.org/a00110.html for an explanation of the
	definitions contained in this file.
******************************************************************************/
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 * http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "syslog.h"
#endif

#include "memory_map.h"

// Select the HEAP in PSRAM as default
#define configUSE_HEAP_REGION_DEFAULT                  eHeapRegion_PLATFORM


#ifdef MTK_OS_HEAP_EXTEND
//#define configTOTAL_HEAP_SIZE                   ((size_t)(389 * 1024))
#define configTOTAL_HEAP_SIZE                   ((size_t)(421 * 1024))
#define configPlatform_HEAP_SIZE                ((size_t)(1024 * 1024))
#else
#define configTOTAL_HEAP_SIZE                   ((size_t)(437 * 1024))
#endif

/* Matter - xTaskCreateStatic */
#define configSUPPORT_STATIC_ALLOCATION 1

/* vApplicationDaemonTaskStartupHook */
#define configUSE_DAEMON_TASK_STARTUP_HOOK          1

#include <exception_handler.h>
#define configASSERT( x ) do { if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); } } while (0)

#define configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES   1

#define configUSE_TICKLESS_IDLE                     0

#endif /* FREERTOS_CONFIG_H */
