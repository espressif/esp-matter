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

#ifndef TEST_PAL_LOG_H_
#define TEST_PAL_LOG_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* In order to print the name of the function, while implementing
 * a generic declaration and an OS-dependent definition, we use a macro that
 * calls a PAL function with __FUNCTION__ as an argument.
 */

/******************************************************************************/
/*
 * @brief This variadic function prints to stderr with or without the name of
 * the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * function - The name of the calling function (can be NULL).
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
void Test_PalPrintfError(const char *function, const char *format, ...);

/******************************************************************************/
/*
 * @brief This variadic function prints to stderr and to a file with or without
 * the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * fd - File descriptor.
 * function - The name of the calling function (can be NULL).
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFprintfError(void *fd, const char *function,
        const char *format, ...);

/******************************************************************************/
/*
 * @brief This variadic function prints messages to stderr.
 *
 * @param[in]
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
void Test_PalPrintfMessage(const char *format, ...);

/******************************************************************************/
/*
 * @brief This variadic function prints strings to stdout with or without
 * the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * function - The name of the calling function (can be NULL).
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
void Test_PalPrintf(const char *function, const char *format, ...);

/******************************************************************************/
/*
 * @brief This variadic function prints strings to stdout and to a file
 * with or without the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * fd - File descriptor.
 * function - The name of the calling function (can be NULL).
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFprintf(void *fd, const char *function, const char *format, ...);

/******************************************************************************/
/*
 * @brief This variadic function prints bytes in a buffer to stdout
 * with or without the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * function - The name of the calling function (can be NULL).
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalPrintByteBuff(const char *function, const char *buffName,
                        uint8_t *buff, uint32_t size);

/******************************************************************************/
/*
 * @brief This variadic function prints bytes in a buffer to a file
 * with or without the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * fd - File descriptor.
 * function - The name of the calling function (can be NULL).
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFprintByteBuff(void *fd, const char *function,
            const char *buffName, uint8_t *buff, uint32_t size);

/******************************************************************************/
/*
 * @brief This variadic function prints bytes in a buffer to a file
 * with or without the name of the calling function.
 * When function is NULL, the name of the function is not printed.
 *
 * @param[in]
 * fd - File descriptor.
 * function - The name of the calling function (can be NULL).
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 * maxSize - Maximum size to print.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalFprintfByteBuffMax(void *fd, const char *function,
            const char *buffName, uint8_t *buff, uint32_t size,
            uint32_t maxSize);

/******************************************************************************/
/*
 * @brief This variadic function prints words in a buffer with or without the
 * name of the calling function.
 * When function is NULL, the name of the function is not printed (can be NULL).
 *
 * @param[in]
 * function - The name of the calling function.
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
void Test_PalPrintWordBuff(const char *function, const char *buffName,
            uint32_t *buff, uint32_t size);

/******************************************************************************/
/*
 * @brief This macro prints errors with the name of the calling function.
 *
 * @param[in]
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINTF_ERROR(format, ...)  {\
    Test_PalPrintfError(__FUNCTION__, format, ##__VA_ARGS__);\
}

/******************************************************************************/
/*
 * @brief This macro prints to a file with the name of the calling function.
 *
 * @param[in]
 * fd - File descriptor.
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_FPRINTF_ERROR(fd, format, ...)  {\
    Test_PalFprintfError(fd, __FUNCTION__, format, ##__VA_ARGS__);\
}

/******************************************************************************/
/*
 * @brief This macro prints messages.
 *
 * @param[in]
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINTF_MESSAGE(format, ...)  {\
    Test_PalPrintfMessage(format, ##__VA_ARGS__);\
}

#ifdef TEST_DEBUG

/******************************************************************************/
/*
 * @brief This macro prints strings with the name of the calling function.
 *
 * @param[in]
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINTF(format, ...)  {\
    Test_PalPrintf(__FUNCTION__, format, ##__VA_ARGS__);\
    Test_PalPrintf(NULL, "\n");\
}

/******************************************************************************/
/*
 * @brief This macro prints strings to a file with the name of the calling function.
 *
 * @param[in]
 * fd - File descriptor.
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_FPRINTF(fd, format, ...)  {\
    Test_PalFprintf(fd, __FUNCTION__, format, ##__VA_ARGS__);\
    Test_PalFprintf(fd, NULL, "\n");\
}

/******************************************************************************/
/*
 * @brief This macro prints strings without the name of the calling function.
 *
 * @param[in]
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINTF_NO_FUNC(format, ...)  {\
    Test_PalPrintf(NULL, format, ##__VA_ARGS__);\
    Test_PalPrintf(NULL, "\n");\
}

/******************************************************************************/
/*
 * @brief This macro prints strings to a file without the name of the calling function.
 *
 * @param[in]
 * fd - File descriptor.
 * format - The printed string.
 * ... - Arguments
 *
 * @param[out]
 *
 * @return
 */
#define TEST_FPRINTF_NO_FUNC(fd, format, ...)  {\
    Test_PalFprintf(fd, NULL, format, ##__VA_ARGS__);\
    Test_PalFprintf(fd, NULL, "\n");\
}

/******************************************************************************/
/*
 * @brief This macro prints bytes in a buffer with the name of the calling
 * function.
 *
 * @param[in]
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINT_BYTE_BUFF(buffName, buff, size)  {\
    Test_PalPrintByteBuff(__FUNCTION__, buffName, buff, size);\
}

/******************************************************************************/
/*
 * @brief This macro prints bytes in a buffer to a file with
 * the name of the calling function.
 *
 * @param[in]
 * fd - File descriptor.
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
#define TEST_FPRINT_BYTE_BUFF(fd, buffName, buff, size)  {\
    Test_PalFprintByteBuff(fd, __FUNCTION__, buffName, buff, size);\
}

/******************************************************************************/
/*
 * @brief This macro prints bytes in a buffer to a file with the name of the
 * calling function.
 *
 * @param[in]
 * fd - File descriptor.
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 * maxSize - Maximum size to print.
 *
 * @param[out]
 *
 * @return
 */
#define TEST_FPRINT_BYTE_BUFF_MAX(fd, buffName, buff, size, maxSize) {\
    Test_PalFprintfByteBuffMax(fd, __FUNCTION__, buffName, buff, size,\
                maxSize);\
}

/******************************************************************************/
/*
 * @brief This macro prints words in a buffer with the name of the calling
 * function.
 *
 * @param[in]
 * buffName - The name of the buffer.
 * buff - Buffer address.
 * size - The size of the buffer.
 *
 * @param[out]
 *
 * @return
 */
#define TEST_PRINT_WORD_BUFF(buffName, buff, size) {\
    Test_PalPrintWordBuff(__FUNCTION__, buffName, buff, size);\
}

/* void TEST_PRINT_BYTE_BUFFP(buffName, buff, size);
   void TEST_FPRINT_LONG_NUM(const char *fd, const char *buffName,
   uint32_t *buff, uint32_t size); */
#else
#define TEST_PRINTF(format, ...) do { } while (0)
#define TEST_FPRINTF(fd, format, ...) do { } while (0)
#define TEST_PRINTF_NO_FUNC(format, ...) do { } while (0)
#define TEST_FPRINTF_NO_FUNC(fd, format, ...) do { } while (0)
#define TEST_PRINT_BYTE_BUFF(buffName, buff, size) do { } while (0)
#define TEST_FPRINT_BYTE_BUFF(fd, buffName, buff, size) do { } while (0)
#define TEST_FPRINT_BYTE_BUFF_MAX(fd, buffName, buff, size, maxSize) do { } while (0)
#define TEST_PRINT_WORD_BUFF(buffName, buff, size) do { } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_LOG_H_ */
