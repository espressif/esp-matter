/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LoggerText.h ========
 */

#ifndef ti_utils_runtime_LoggerText__include
#define ti_utils_runtime_LoggerText__include

#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 *  ======== LoggerText_Type_CIRCULAR ========
 */
#define LoggerText_Type_CIRCULAR        101

/*
 *  ======== LoggerText_Type_LINEAR ========
 */
#define LoggerText_Type_LINEAR          102

/*
 *  ======== LoggerText_Header ========
 */
typedef struct LoggerText_Header {
    uint32_t    serial;
    uint32_t    ts_hi;
    uint32_t    ts_lo;
} LoggerText_Header;

/*
 *  ======== LoggerText_Record ========
 */
typedef struct LoggerText_Record {
    LoggerText_Header   hdr;
    char                text[];
} LoggerText_Record;

/*
 *  ======== LoggerText_Instance ========
 */
typedef struct LoggerText_Instance {
    char               *label;
    uint8_t             bufType;
    int8_t              advance;
    uint16_t            numEntries;
    uint32_t            textLen;
    uint32_t            serial;
    LoggerText_Record   *store;
    LoggerText_Record   *curEntry;           /* next record to write */
    LoggerText_Record   *endEntry;
} LoggerText_Instance;

/*
 *  ======== LoggerText_Handle ========
 */
typedef LoggerText_Instance *LoggerText_Handle;

/*
 *  ======== LoggerText_handle ========
 */
extern LoggerText_Handle LoggerText_handle(int index);

/*
 *  ======== LoggerText_write ========
 */
//extern void LoggerText_write(char *text);
extern void LoggerText_write(LoggerText_Handle hndl, char *text);


#if defined (__cplusplus)
}
#endif

#endif /* ti_utils_runtime_LoggerText__include */
