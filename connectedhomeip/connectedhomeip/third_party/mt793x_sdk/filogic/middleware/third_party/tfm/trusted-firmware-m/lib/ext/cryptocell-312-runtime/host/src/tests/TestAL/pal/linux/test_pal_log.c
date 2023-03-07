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

    if (function)
        fprintf(stderr, "%s(): ", function);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(args);
}


/******************************************************************************/
void Test_PalFprintfError(void *fd, const char *function, const char *format, ...)
{
    FILE *fd_l = (FILE *)fd;
    va_list args;

    va_start(args, format);

    if (function)
        fprintf(fd_l, "%s(): ", function);
    vfprintf(fd_l, format, args);
    fprintf(fd_l, "\n");
    fflush(fd_l);

    if (function)
        fprintf(stderr, "%s(): ", function);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(args);
}

/******************************************************************************/
void Test_PalPrintfMessage(const char *format, ...)
{
    va_list args;

    va_start(args, format);

    vfprintf(stderr, format, args);
    fflush(stderr);

    va_end(args);
}

/******************************************************************************/
void Test_PalPrintf(const char *function, const char *format, ...)
{
    va_list args;

    va_start(args, format);

    if (function)
        fprintf(stdout, "%s(): ", function);
    vfprintf(stdout, format, args);
    fflush(stdout);

    va_end(args);
}

/******************************************************************************/
void Test_PalFprintf(void *fd, const char *function, const char *format, ...)
{
    FILE *fd_l = (FILE *)fd;
    va_list args;

    va_start(args, format);

    if (function)
        fprintf(fd_l, "%s(): ", function);
    vfprintf(fd_l, format, args);
    fflush(fd_l);

    if (function)
        fprintf(stdout, "%s(): ", function);
    vfprintf(stdout, format, args);
    fflush(stdout);

    va_end(args);
}

/******************************************************************************/
void Test_PalPrintByteBuff(const char *function, const char *buffName,
                    uint8_t *buff, uint32_t size)
{
    unsigned int i = 0;

    Test_PalPrintf(function, "printing %s, byte size %d\n",
                buffName, (unsigned int)size);
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
    FILE *fd_l = (FILE *)fd;
    unsigned int i = 0;

    Test_PalFprintf(fd_l, function, "printing %s, byte size %d", buffName,
                            (unsigned int)size);
    for (i = 0; i < size; i++) {
        if (!(i%16))
            Test_PalFprintf(fd_l, NULL, "\n\t");
        Test_PalFprintf(fd_l, NULL, "0x%02X ",
                (unsigned char)(*((unsigned char *)buff+i)));
    }
    Test_PalFprintf(fd_l, NULL, "\n");
}

/******************************************************************************/
void Test_PalFprintfByteBuffMax(void *fd, const char *function,
    const char *buffName, uint8_t *buff, uint32_t size, uint32_t maxSize)
{
    int i = 0;
    FILE *fd_l = (FILE *)fd;
    int minSize = ((size > maxSize) ? maxSize:size);

    Test_PalFprintf(fd_l, function, "printing %s, buff size %d, max size %d",
            buffName, (unsigned int)size, (unsigned int)maxSize);

    for (i = 0; i < minSize; i++) {
        if (!(i%16))
            Test_PalFprintf(fd_l, NULL, "\n\t");
        Test_PalFprintf(fd_l, NULL, "0x%02X ",
                (unsigned char)(*((unsigned char *)buff+i)));
    }
    Test_PalFprintf(fd_l, NULL, "\n");
}

/******************************************************************************/
void Test_PalPrintWordBuff(const char *function, const char *buffName,
                uint32_t *buff, uint32_t size)
{
    uint8_t i = 0;

    Test_PalPrintf(function, "printing %s, word size %d\n", buffName,
                            (unsigned int)size);
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
