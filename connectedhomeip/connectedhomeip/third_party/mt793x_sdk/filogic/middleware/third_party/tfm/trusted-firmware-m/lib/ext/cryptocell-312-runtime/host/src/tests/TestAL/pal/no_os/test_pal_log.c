/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "test_pal_log.h"

/******************************************************************************/
void Test_PalPrintfError(const char *function, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    printf("%s(): ", function);
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

/******************************************************************************/
void Test_PalFprintfError(void *fd, const char *function, const char *format, ...)
{
    (void)fd;
    (void)function;
    (void)format;
}

/******************************************************************************/
void Test_PalPrintfMessage(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    vprintf(format, args);

    va_end(args);
}

/******************************************************************************/
void Test_PalPrintf(const char *function, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    if (function)
        printf("%s(): ", function);
    vprintf(format, args);

    va_end(args);
}

/******************************************************************************/
void Test_PalFprintf(void *fd, const char *function, const char *format, ...)
{
    (void)fd;
    (void)function;
    (void)format;
}

/******************************************************************************/
void Test_PalPrintByteBuff(const char *function, const char *buffName,
                uint8_t *buff, uint32_t size)
{
    unsigned int i = 0;

    Test_PalPrintf(function, "printing %s, byte size %d\n", buffName,
            (unsigned int)size);

    for (i = 0; i < size; i++) {
        if (!(i%16))
            Test_PalPrintf(NULL, "\n\t");
        Test_PalPrintf(NULL, "0x%02X ",
                (unsigned char)(*((unsigned char *)buff+i)));
    }
    Test_PalPrintf(NULL, "\n");
}

/******************************************************************************/
void Test_PalFprintByteBuff(void *fd, const char *function,
            const char *buffName, uint8_t *buff, uint32_t size)
{
    (void)fd;
    (void)function;
    (void)buffName;
    (void)buff;
    (void)size;
}

/******************************************************************************/
void Test_PalFprintfByteBuffMax(void *fd, const char *function,
    const char *buffName, uint8_t *buff, uint32_t size, uint32_t maxSize)
{
    (void)fd;
    (void)function;
    (void)buffName;
    (void)buff;
    (void)size;
    (void)maxSize;
}

/******************************************************************************/
void Test_PalPrintWordBuff(const char *function, const char *buffName,
                uint32_t *buff, uint32_t size)
{
    uint8_t i = 0;

    Test_PalPrintf(function, "printing %s, word size %d\n", buffName,
                            (uint32_t)size);
    for (i = 0; i < size; i++) {
        if (!(i%4))
            Test_PalPrintf(NULL, "\n\t");
        Test_PalPrintf(NULL, "0x%08X  ",
                (unsigned int)(*((unsigned int *)buff+i)));
    }
    Test_PalPrintf(NULL, "\n");
}

/* inline void TEST_PRINT_BYTE_BUFFP(buffName, buff, size);
   inline void TEST_FPRINT_LONG_NUM(const char *fd, const char *buffName,
   uint32_t *buff, uint32_t size); */
