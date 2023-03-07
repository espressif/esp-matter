/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _H_COMMON_H
#define _H_COMMON_H

#include "typedefs.h"
#include "driver_api.h"
#include <stdio.h>

#ifndef MSG
#define MSG dbg_print
#endif /* #ifndef MSG */

extern UINT32 SystemTick;

/// Assertion ////////////////////////////////////////////////////////////////

void AssertionFailed(const char *function, const char *file, int line);
void NotImplemented(const char *function, char *file, int line);

#define ASSERT(expr)                                            \
    do {                                                        \
        if (!(expr))                                            \
            AssertionFailed(__FUNCTION__,__FILE__, __LINE__);   \
    } while (0)                                                 \

#define NOT_IMPLEMENTED()                                   \
    do {                                                    \
        NotImplemented(__FUNCTION__,__FILE__, __LINE__);    \
    } while (0)                                             \

#define STATIC_ASSERT(pred)         STATIC_ASSERT_X(pred, __LINE__)
#define STATIC_ASSERT_X(pred, line) STATIC_ASSERT_XX(pred, line)
#define STATIC_ASSERT_XX(pred, line) \
    extern char assertion_failed_at_##line[(pred) ? 1 : -1]


/// Debug Serial ////////////////////////////////////////////////////////////
void InitDebugSerial(void);
void InitDebugUart(unsigned int baudrate);
void EdbgOutputDebugString(LPCSTR sz, ...);
int WriteDebugByte(UINT8 ch);
int ReadDebugByte(void);

#define DEBUG_SERIAL_READ_NODATA   (-1)
#define DEBUG_SERIAL_COM_ERROR     (-2)

/* Log Level Setting */
#define LOG_LEVEL    2

#if(LOG_LEVEL >= 0)
#undef sys_print
#define sys_print      printf
#else /* #if(LOG_LEVEL >= 0) */
#undef sys_print
#define sys_print(...)
#endif /* #if(LOG_LEVEL >= 0) */
#if(LOG_LEVEL >= 1)
#undef must_print
#define must_print     printf
#else /* #if(LOG_LEVEL >= 1) */
#undef must_print
#define must_print(...)
#endif /* #if(LOG_LEVEL >= 1) */
#if(LOG_LEVEL >= 2)
#undef opt_print
#define opt_print      printf
#define dbg_print      printf
#else /* #if(LOG_LEVEL >= 2) */
#undef opt_print
#undef dbg_print
#undef printf
#define dbg_print(...)
#define opt_print(...)
#endif /* #if(LOG_LEVEL >= 2) */
/* !Log Level Setting */


// Utility /////////////////////////////////////////////////////////////////////

#define ARY_SIZE(x) (sizeof((x)) / sizeof((x[0])))

#ifndef __cplusplus
#define min(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);    \
    _x < _y ? _x : _y; })

#define max(x,y) ({ \
    typeof(x) _x = (x); \
    typeof(y) _y = (y); \
    (void) (&_x == &_y);    \
    _x > _y ? _x : _y; })
#endif /* __cplusplus */

#define WRITE_REG(val,addr)    ((*(volatile unsigned int *)(addr)) = (unsigned int)val)
#define WRITE_REG_W(val,addr)   WRITE_REG(val,addr)
#define WRITE_REG_H(val,addr)    ((*(volatile unsigned short *)(addr)) = (unsigned short)val)
#define WRITE_REG_B(val,addr)    ((*(volatile unsigned char *)(addr)) = (unsigned char)val)
#define READ_REG(addr)          (*(volatile unsigned int *)(addr))

#define dummy_func()    do {\
__asm__ __volatile__("nop" ::);\
}while(0)

#define delay_a_while(count) \
        do {    \
           unsigned int delay; \
           for (delay = count; delay != 0; delay--)    \
              /* NOP */ \
              ; \
        } while (0)


#endif /* #ifndef _H_COMMON_H */

