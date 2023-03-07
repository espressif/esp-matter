/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "uart_stdout.h"
#include "stdarg.h"
#include "string.h"

/* Log level = debug */
int CC_PAL_logLevel = 3;
#define CC_PAL_MAX_LOG_LEVEL 3

#include "cc_pal_log.h"


#define CC312_LOG_BUF_SIZE 64

uint32_t CC_PAL_logMask = 0xFFFFFFFF;

void CC_PalLogInit(void){}

void CC_PalLog(int level, const char* format, ...)
{
   char buf[CC312_LOG_BUF_SIZE] = {0};
   va_list args;
   int format_len = strlen(format);

   if (format_len + 2 > CC312_LOG_BUF_SIZE)
   {
       printf("CC312 logging error: Message too long\r\n");
       return;
   }


   va_start(args, format);

   /* CC312 lib doesn't insert CR characters so it's done here */
   strcpy(buf, format);
   buf[format_len] = '\r';

   vprintf(buf, args);
}
