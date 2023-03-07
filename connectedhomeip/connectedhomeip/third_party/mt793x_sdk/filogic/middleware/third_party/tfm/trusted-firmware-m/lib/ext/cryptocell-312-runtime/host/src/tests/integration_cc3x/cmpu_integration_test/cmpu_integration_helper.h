/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CMPU_INTEGRATION_HELPER_H_
#define _CMPU_INTEGRATION_HELPER_H_

#include "test_pal_mem.h"
#include "test_pal_mem_s.h"

#ifdef ARCH_V8M
#define CMPUIT_TEST_PAL_ALLOC    Test_PalDMAContigBufferAlloc_s
#define CMPUIT_TEST_PAL_FREE     Test_PalDMAContigBufferFree_s
#else
#define CMPUIT_TEST_PAL_ALLOC    Test_PalDMAContigBufferAlloc
#define CMPUIT_TEST_PAL_FREE     Test_PalDMAContigBufferFree
#endif

#define FREE_IF_NOT_NULL(_buff)    \
                do { \
                    if (_buff != NULL) {     \
                        CMPUIT_TEST_PAL_FREE(_buff);    \
                        _buff = NULL; \
                    } \
                }while(0)

#define ALLOC32(_pBuff, _pBuffAligned, _wantedSize) \
                do { \
                    uint32_t address = 0; \
                    uint32_t remainder = 0; \
                    _pBuff = (void*)CMPUIT_TEST_PAL_ALLOC(_wantedSize); \
                    address = (uint32_t)_pBuff; \
                    remainder = (sizeof(uint32_t) - (address % sizeof(uint32_t))) % sizeof(uint32_t); \
                    if (_pBuff == NULL) goto bail; \
                    _pBuffAligned = (void*)(address + remainder); \
                }while(0)
/**
 *
 * Assert Macros
 *
 */
#define CMPUIT_ASSERT_WITH_RESULT(_c, _exp)                                            \
                do {                                                                  \
                    int _ret = 0;                                                     \
                    if( ( _ret = (int)_c) != _exp )                                   \
                    {                                                                 \
                        CMPUIT_PRINT_ERROR( "failed with rc 0x%08x\n", _ret );         \
                        rc = CMPUIT_ERROR__FAIL;                                       \
                        goto bail;                                                    \
                    }                                                                 \
                } while(0)

#define CMPUIT_ASSERT(_c)                                                              \
                do {                                                                  \
                    if( !(_c))                                                        \
                    {                                                                 \
                        CMPUIT_PRINT_ERROR( "failed\n" );                              \
                        rc = CMPUIT_ERROR__FAIL;                                       \
                        goto bail;                                                    \
                    }                                                                 \
                }                                                                     \
                while(0)

#define CMPUIT_API(_c)                                                                 \
                do {                                                                  \
                    CMPUIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _c;                                                               \
                } while(0)

#define CMPUIT_API_ASSIGNMENT(_res, _c)                                                \
                do {                                                                  \
                    CMPUIT_PRINT_DBG("running %-30.30s\n", #_c);                       \
                    _res = _c;                                                        \
                } while(0)

#define CMPUIT_ASSERT_API(_c)                                                          \
                do {                                                                  \
                    CMPUIT_ASSERT(_c);                                                 \
                } while(0)

/************************************************************
 *
 * type decelerations
 *
 ************************************************************/

/************************************************************
 *
 * externs
 *
 ************************************************************/

/************************************************************
 *
 * function prototypes
 *
 ************************************************************/

#endif //_CMPU_INTEGRATION_HELPER_H_
