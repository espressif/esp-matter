/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _UTIL_LOG_H
#define _UTIL_LOG_H

#include "stdio.h"
#include <stdint.h>


#define CC_COMMON_OK   0
#define WORD_BYTE_SIZE (sizeof(uint32_t))

#define UTIL_LOG_ERR(format, args...) \
        fprintf(stderr, "%s(): " format, __FUNCTION__, ##args)

#ifdef UTIL_DEBUG
#define UTIL_LOG_INFO UTIL_LOG_ERR
#define UTIL_LOG_BYTE_BUFF(str, buff, size) {\
    uint32_t i=0;\
    unsigned long buff_addr = (unsigned long)buff;\
    UTIL_LOG_ERR("\nprinting %s, %lu bytes", str, (unsigned long)size);\
    for (i=0; i<size; i++){\
        if (!(i%16)) {\
            fprintf(stderr, "\n");\
        }\
        fprintf(stderr, " 0x%02X ", *((uint8_t *)buff_addr + i));\
    }\
    fprintf(stderr, "\n");\
}
#define UTIL_LOG_WORD_BUFF(str, buff, wordSize) {\
    uint32_t i=0;\
    unsigned long buff_addr = (unsigned long)buff;\
    UTIL_LOG_ERR("\nprinting %s, %lu words", str, (unsigned long)wordSize);\
    for (i=0; i<wordSize; i++){\
        if (!(i%4)) {\
            fprintf(stderr, "\n");\
        }\
        fprintf(stderr, " 0x%08lX ", *((unsigned long *)buff_addr + i));\
    }\
    fprintf(stderr, "\n");\
}
#else
#define UTIL_LOG_INFO(format...)  do{ }while(0)
#define UTIL_LOG_BYTE_BUFF(str, buff, size) do{ }while(0)
#define UTIL_LOG_WORD_BUFF(str, buff, size) do{ }while(0)
#endif


#define UTIL_REVERSE_WORD_ORDER(buff, buffSize) {\
    uint32_t i = 0;\
    uint8_t  tmpByte = 0;\
    if (buffSize % WORD_BYTE_SIZE) {\
        memset(buff, 0, buffSize);\
    } else {\
        while ((i+3)<buffSize) {\
            tmpByte = buff[i+3];\
            buff[i+3] = buff[i];\
            buff[i] = tmpByte;\
            tmpByte = buff[i+2];\
            buff[i+2] = buff[i+1];\
            buff[i+1] = tmpByte;\
            i += 4;\
        }\
    }\
}

#endif
