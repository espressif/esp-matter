/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __TST_HOST_LOG_H__
#define __TST_HOST_LOG_H__

#include <stdint.h>
#include <stdio.h>


#define TEST_PRINTF_ERROR(format, ...)  {\
        fprintf(stderr, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
        fprintf(stderr, "\n");\
        fflush(stderr);\
}
#define TEST_FPRINTF_ERROR(fd, format, ...)  {\
        fprintf(fd, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
        fprintf(fd, "\n");\
        fflush(fd);\
        fprintf(stderr, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
        fprintf(stderr, "\n");\
        fflush(stderr);\
}
#define TEST_PRINTF_MESSAGE(format, ...)  {\
        fprintf(stderr, format, ##__VA_ARGS__);\
        fflush(stderr);\
}

//to print buffers during fips tests without debug compilation
#define TEST_PRINT_WORD_BUFF_TRACE(buffName, size, buff) {\
    unsigned int i = 0;\
    TEST_PRINTF_MESSAGE("printing %s, word size %d", buffName, (unsigned int)size);\
    for (i=0; i< size; i++) {\
        if (!(i%4)) {\
            TEST_PRINTF_MESSAGE("\n\t");\
        }\
        TEST_PRINTF_MESSAGE("  0x%08X  ", (unsigned int)(*((unsigned int *)buff+i)));\
    }\
    TEST_PRINTF_MESSAGE("\n");\
}

#define TEST_PRINT_BYTE_BUFF_TRACE(buffName, size, buff) {\
    unsigned int i = 0;\
    TEST_PRINTF_MESSAGE("printing %s, byte size %d", buffName, (unsigned int)size);\
    for (i=0; i< size; i++) {\
        if (!(i%16)) {\
            TEST_PRINTF_MESSAGE("\n\t");\
        }\
        TEST_PRINTF_MESSAGE("0x%02X ", (unsigned char)(*((unsigned char *)buff+i)));\
    }\
    TEST_PRINTF_MESSAGE("\n");\
}

#define TEST_PRINTF_TRACE(format, ...)  {\
    fprintf(stdout, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
    fprintf(stdout, "\n");\
    fflush(stdout);\
}

#ifdef TEST_DEBUG
#define TEST_FPRINTF(fd, format, ...)  {\
     fprintf(fd, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
     fprintf(fd, "\n");\
     fflush(fd);\
}
#define TEST_PRINTF(format, ...)  {\
     fprintf(stdout, "%s(): " format, __FUNCTION__, ##__VA_ARGS__);\
     fprintf(stdout, "\n");\
     fflush(stdout);\
}

#define TEST_PRINTF1(format, ...)  {\
     fprintf(stdout, " " format, ##__VA_ARGS__);\
     fflush(stdout);\
}

#define TEST_FPRINTF1(fd, format, ...)  {\
     fprintf(fd, " " format, ##__VA_ARGS__);\
     fflush(fd);\
}

#define TEST_PRINT_WORD_BUFF(buffName, size, buff) {\
    unsigned int i = 0;\
    TEST_PRINTF("printing %s, word size %d", buffName, (unsigned int)size);\
    for (i=0; i< size; i++) {\
        if (!(i%4)) {\
            TEST_PRINTF1("\n\t");\
        }\
        TEST_PRINTF1("  0x%08X  ", (unsigned int)(*((unsigned int *)buff+i)));\
    }\
    TEST_PRINTF1("\n");\
}


#define TEST_PRINT_BYTE_BUFF(buffName, size, buff) {\
    unsigned int i = 0;\
    TEST_PRINTF("printing %s, byte size %d", buffName, (unsigned int)size);\
    for (i=0; i< size; i++) {\
        if (!(i%16)) {\
            TEST_PRINTF1("\n\t");\
        }\
        TEST_PRINTF1("0x%02X ", (unsigned char)(*((unsigned char *)buff+i)));\
    }\
    TEST_PRINTF1("\n");\
}

#define TEST_PRINT_BYTE_BUFFP(buffName, size, buff) {\
    unsigned int idxT = 0;\
    uint8_t *buffP = (uint8_t *)(buff);\
    TEST_PRINTF("printing %s, byte size %d", buffName, (unsigned int)size);\
    for (idxT=0; idxT< size; idxT++) {\
        if (!(idxT%16)) {\
            TEST_PRINTF1("\n\t");\
        }\
        TEST_PRINTF1("0x%02X ", buffP[idxT]);\
    }\
    TEST_PRINTF1("\n");\
}

#define TEST_FPRINT_LONG_NUM(fd, buffName, buff, size) {\
    unsigned int i = 0;\
    TEST_FPRINTF(fd, "printing %s, byte size %d", buffName, (unsigned int)size);\
    TEST_FPRINTF1(fd, "0x");\
    for (i=0; i< size; i++) {\
        fprintf(fd, "%02X", (unsigned char)(*((unsigned char *)buff+i)));\
    }\
    fprintf(fd, "\n");\
}

#define TEST_FPRINT_BYTE_BUFF(fd, buffName, buff, size) {\
    unsigned int i = 0;\
    TEST_FPRINTF(fd, "printing %s, byte size %d", buffName, (unsigned int)size);\
    for (i=0; i< size; i++) {\
        if (!(i%16)) {\
            TEST_FPRINTF1(fd, "\n\t");\
        }\
        TEST_FPRINTF1(fd, "0x%02X ", (unsigned char)(*((unsigned char *)buff+i)));\
    }\
    TEST_FPRINTF1(fd, "\n");\
}

#define TEST_FPRINT_BYTE_BUFF_MAX(fd, buffName, buff, size, maxSize) {\
    int i = 0;\
    int minSize = ((size>maxSize)?maxSize:size);\
    TEST_FPRINTF(fd, "printing %s, buff size %d, max size %d", buffName, (unsigned int)size, (unsigned int)maxSize);\
    for (i=0; i< minSize; i++) {\
        if (!(i%16)) {\
            TEST_FPRINTF1(fd, "\n\t");\
        }\
        TEST_FPRINTF1(fd, "0x%02X ", (unsigned char)(*((unsigned char *)buff+i)));\
    }\
    TEST_FPRINTF1(fd, "\n");\
}

#else
#define TEST_FPRINTF(fd, format, ...)  do{ }while(0)
#define TEST_PRINTF(format, ...)  do{ }while(0)
#define TEST_PRINTF1(format, ...)  do{ }while(0)
#define TEST_PRINT_BYTE_BUFF(str, size, buff) do{ }while(0)
#define TEST_PRINT_BYTE_BUFFP(buffName, size, buff) do{ }while(0)
#define TEST_PRINT_WORD_BUFF(str, size, buff) do{ }while(0)
#define TEST_FPRINT_LONG_NUM(fd, buffName, buff, size)  do{ }while(0)
#define TEST_FPRINT_BYTE_BUFF(fd, buffName, buff, size)  do{ }while(0)
#define TEST_FPRINT_BYTE_BUFF_MAX(fd, buffName, buff, size, maxSize)  do{ }while(0)
#endif
#endif /*__TST_HOST_LOG_H__*/
