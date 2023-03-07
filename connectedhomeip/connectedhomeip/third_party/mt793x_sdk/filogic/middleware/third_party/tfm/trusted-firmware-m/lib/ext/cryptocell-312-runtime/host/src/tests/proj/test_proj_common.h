/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _TEST_PROJ_COMMON_H_
#define _TEST_PROJ_COMMON_H_



/* Error types */
/*! Defines test proj base error. */
#define TEST_PROJ_BASE_ERROR     (0x00FFFF00)

#define TEST_OK                 (0x00000000)
#define TEST_INVALID_PARAM_ERR  (TEST_PROJ_BASE_ERROR + 0x00000001)
#define TEST_COMPARE_ERR        (TEST_PROJ_BASE_ERROR + 0x00000002)
#define TEST_HW_FAIL_ERR        (TEST_PROJ_BASE_ERROR + 0x00000003)
#define TEST_MAPPING_ERR        (TEST_PROJ_BASE_ERROR + 0x00000004)
#define TEST_EXECUTE_FAIL       (TEST_PROJ_BASE_ERROR + 0x00000005)
#define TEST_MAX_THREADS        16
#define TEST_MAX_FILE_NAME      256
#define THREAD_STACK_SIZE       (128*1024) /* stack has 128KB for 64bit CPU */


#ifdef BIG__ENDIAN
#define TEST_CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord) {\
    outWord = (*inPtr<<24);\
    outWord |= (*(inPtr+1)<<16);\
    outWord |= (*(inPtr+2)<<8);\
    outWord |= (*(inPtr+3));\
}
#else
#define TEST_CONVERT_BYTE_ARR_TO_WORD(inPtr, outWord) {\
    outWord = (*(inPtr+3))<<24;\
    outWord |= (*(inPtr+2))<<16;\
    outWord |= (*(inPtr+1))<<8;\
    outWord |= (*inPtr);\
}
#endif

#endif /* _TEST_PROJ_COMMON_H_ */
